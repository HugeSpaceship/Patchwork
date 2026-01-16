package main

type Game struct {
	SymbolName   string             `yaml:"symbol_name"`
	Offsets      GameAddressOffsets `yaml:"offsets"`
	UserAgent    string             `yaml:"user_agent"`
	Name         string             `yaml:"name"`
	GameVersions []string           `yaml:"versions"`
	TitleIDs     []string           `yaml:"title_ids"`
	PPUHash      string             `yaml:"ppu_hash"`
}

type GameAddressOffsets struct {
	UserAgent     uint32 `yaml:"user_agent"`
	ResourceCheck uint32 `yaml:"resource_check"`
}
