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
	HttpURL       uint32 `yaml:"http_url"`
	HttpsURL      uint32 `yaml:"https_url"`
	PresenceURL   uint32 `yaml:"presence_url"`
	LiveURL       uint32 `yaml:"live_url"`
	DigestKey     uint32 `yaml:"digest_key"`
}

type Server struct {
	PrimaryURL   string `yaml:"primary_url"`
	HttpsURL     string `yaml:"https_url"`
	CustomDigest string `yaml:"custom_digest"`
	Name         string `yaml:"name"`
	Owner        string `yaml:"owner"`
	Description  string `yaml:"description"`
}

type Servers struct {
	Servers []Server `yaml:"servers"`
}
