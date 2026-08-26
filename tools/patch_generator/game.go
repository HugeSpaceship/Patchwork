package patch_generator

type PatchType string

const (
	Hook      PatchType = "hook"
	Overwrite PatchType = "overwrite"
)

type Patch struct {
	// Symbol to search for in the prx that denotes the start of the patch
	Symbol string
	// Length of the patch, if left unset it will try to auto-detect by looking for an absolute branch instruction
	Length uint64
	// Type determines whether the patch is a hook that should be in its own memory or a set of instruction to overwrite existing code
	Type PatchType
	// Offset is the offset in the game to start patching from, in virtual memory
	Offset uint32
}

type Game struct {
	Offsets      GameAddressOffsets `yaml:"offsets"`
	UserAgent    string             `yaml:"user_agent"`
	Name         string             `yaml:"name"`
	GameVersions []string           `yaml:"versions"`
	TitleIDs     []string           `yaml:"title_ids"`
	PPUHash      string             `yaml:"ppu_hash"`
	Patches      []Patch            `yaml:"patches"`
}

type GameAddressOffsets struct {
	UserAgent   uint32 `yaml:"user_agent"`
	HttpURL     uint32 `yaml:"http_url"`
	HttpsURL    uint32 `yaml:"https_url"`
	PresenceURL uint32 `yaml:"presence_url"`
	LiveURL     uint32 `yaml:"live_url"`
	DigestKey   uint32 `yaml:"digest_key"`
	NetworkKey  uint32 `yaml:"network_key"`
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
