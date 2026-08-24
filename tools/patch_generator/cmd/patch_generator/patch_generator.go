package main

import (
	"bytes"
	"debug/elf"
	"flag"
	"os"
	"path"

	"git.valtek.uk/HugeSpaceship/patchwork/tools/patch_generator"
	"git.valtek.uk/HugeSpaceship/patchwork/tools/patch_generator/rpcs3_patch"
	"github.com/goccy/go-yaml"
)

var inputElf = flag.String("input", "", "Input ELF file")
var configDir = flag.String("config", "", "Config directory")
var outputYaml = flag.String("output", "", "Output YAML file")

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

	var _servers patch_generator.Servers
	var servers []patch_generator.Server
	for _, file := range configFiles { // find servers if it exists first
		if file.Name() == "servers.yaml" {
			serverBytes, err := os.ReadFile(path.Join(*configDir, "servers.yaml"))
			if err != nil {
				panic(err)
			}
			err = yaml.Unmarshal(serverBytes, &_servers)
			if err != nil {
				panic(err)
			}
			servers = _servers.Servers // AAAAGH GO GET SOME ANNOTATIONS OR SOMETHING SO I DON'T HAVE TO DO THIS SHIT
			break
		}
	}

	patches := make(rpcs3_patch.PatchMap)

	for _, file := range configFiles {
		if file.IsDir() {
			continue
		}
		if file.Name() == "servers.yaml" {
			continue
		}

		configBytes, err := os.ReadFile(path.Join(*configDir, file.Name()))
		if err != nil {
			panic(err)
		}

		game := patch_generator.Game{}
		err = yaml.Unmarshal(configBytes, &game)
		if err != nil {
			panic(err)
		}

		for k, v := range rpcs3_patch.MakePatches(elfFile, game, servers) {
			patches[k] = v
		}
	}

	buf := new(bytes.Buffer)
	enc := yaml.NewEncoder(buf, yaml.IndentSequence(true))
	defer enc.Close()
	err = enc.Encode(&rpcs3_patch.PatchFile{Version: 1.2, Patches: patches})
	if err != nil {
		panic(err)
	}
	err = os.WriteFile(*outputYaml, buf.Bytes(), 0644)
	if err != nil {
		panic(err)
	}
}
