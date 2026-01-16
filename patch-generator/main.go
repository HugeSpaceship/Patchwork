package main

import (
	"bytes"
	"debug/elf"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"os"
	"path"

	"github.com/goccy/go-yaml"
)

var inputElf = flag.String("input", "", "Input ELF file")
var configDir = flag.String("config", "", "Config directory")
var outputYaml = flag.String("output", "", "Output YAML file")

const baMask uint32 = 0x48000002

func GetHook(r io.ReaderAt, hookOffset uint64) ([]byte, error) {
	baFound := false
	i := hookOffset
	outBuf := make([]byte, 0, 64)
	insBuf := make([]byte, 4)
	for {
		n, err := r.ReadAt(insBuf, int64(i))
		if err != nil {
			return nil, err
		}
		if n != 4 {
			return nil, fmt.Errorf("unexpected read size %d", n)
		}
		outBuf = append(outBuf, insBuf...)
		instruction := binary.BigEndian.Uint32(insBuf)
		if instruction&baMask == baMask && insBuf[3]&1 != 1 { // if the last bit is 1 it's a linked branch
			if baFound { // on the second BA, write out the data
				return outBuf, nil
			}
			baFound = true
		}
		i += 4
	}
}

func main() {
	flag.Parse()

	configFiles, err := os.ReadDir(*configDir)
	if err != nil {
		panic(err)
	}

	rawElfFile, err := os.Open(*inputElf)
	if err != nil {
		panic(err)
	}
	elfFile, err := elf.NewFile(rawElfFile)
	if err != nil {
		panic(err)
	}
	syms, err := elfFile.Symbols()
	if err != nil {
		panic(err)
	}

	patches := make(map[string]map[string]RPCS3Patch)

	for _, file := range configFiles {
		if file.IsDir() {
			continue
		}

		configBytes, err := os.ReadFile(path.Join(*configDir, file.Name()))
		if err != nil {
			panic(err)
		}

		game := Game{}
		err = yaml.Unmarshal(configBytes, &game)
		if err != nil {
			panic(err)
		}

		var patchEntries []PatchEntry
		for _, sym := range syms {
			if sym.Name == game.SymbolName {
				gameHook, err := GetHook(elfFile.Section(".text"), sym.Value)
				if err != nil {
					panic(err)
				}
				patchEntries = MakeCodePatch(game.Offsets.ResourceCheck, gameHook)
			}
		}

		patchEntries = append(patchEntries, PatchEntry{
			Type:    "c_utf8",
			Address: game.Offsets.UserAgent,
			Value:   game.UserAgent,
		})

		titleIDVersionMap := make(map[string][]string)
		for _, d := range game.TitleIDs {
			titleIDVersionMap[d] = game.GameVersions
		}

		gamePatch := RPCS3Patch{
			Games: map[string]map[string][]string{
				game.Name: titleIDVersionMap,
			},
			Author:       "HugeSpaceship contributors",
			Notes:        "Combined patch from patchwork sources",
			PatchVersion: 1.0,
			Patch:        patchEntries,
		}
		patches[game.PPUHash] = make(map[string]RPCS3Patch)
		patches[game.PPUHash]["Patchwork Combined Patch"] = gamePatch
	}

	buf := new(bytes.Buffer)
	enc := yaml.NewEncoder(buf, yaml.IndentSequence(true))
	patchFile := PatchFile{Version: 1.2, Patches: patches}
	err = enc.Encode(&patchFile)
	if err != nil {
		panic(err)
	}
	err = os.WriteFile(*outputYaml, buf.Bytes(), 0644)
	if err != nil {
		panic(err)
	}
}
