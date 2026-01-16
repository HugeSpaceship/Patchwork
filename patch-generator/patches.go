package main

import (
	"fmt"
	"strconv"
	"strings"
)

type PatchGames map[string]map[string][]string

type RPCS3Patch struct {
	Games        PatchGames   `yaml:"Games"`
	Author       string       `yaml:"Author"`
	Notes        string       `yaml:"Notes"`
	PatchVersion float64      `yaml:"Patch Version"`
	Patch        []PatchEntry `yaml:"Patch"`
}

type PatchEntry struct {
	Type    string `yaml:""`
	Address uint32 `yaml:""`
	Value   string `yaml:""`
}

type PatchFile struct {
	Version float32                          `yaml:"Version"`
	Patches map[string]map[string]RPCS3Patch `yaml:",inline"`
}

func (e PatchEntry) MarshalYAML() ([]byte, error) {
	if e.Address == 0 {
		return []byte(fmt.Sprintf("[%s, 0x0, %s]", e.Type, e.Value)), nil
	}
	if _, err := strconv.ParseFloat(e.Value, 64); err != nil {
		return []byte(fmt.Sprintf("[%s, %#08x, %s]", e.Type, e.Address, strconv.Quote(e.Value))), nil
	}
	return []byte(fmt.Sprintf("[%s, %#08x, %s]", e.Type, e.Address, e.Value)), nil
}

func (e PatchGames) MarshalYAML() ([]byte, error) {
	sb := strings.Builder{}
	for game, titles := range e {
		sb.WriteString(fmt.Sprintf("\"%s\":\n", game))
		for s, i := range titles {
			sb.WriteString(fmt.Sprintf("  %s: [%s]\n", s, strings.Join(i, ", ")))
		}
	}

	return []byte(sb.String()), nil
}

func MakeCodePatch(hookOffset uint32, patch []byte) []PatchEntry {
	if len(patch)%4 != 0 {
		panic("invalid patch size")
	}
	out := make([]PatchEntry, len(patch)/4+1)
	out[0] = PatchEntry{Type: "calloc", Address: hookOffset, Value: fmt.Sprintf("%d", len(patch)/4)}

	for i := 0; i < len(patch); i += 4 {
		out[i/4+1] = PatchEntry{Type: "be32", Address: 0, Value: fmt.Sprintf("%#08x", patch[i:i+4])}
	}

	return out
}
