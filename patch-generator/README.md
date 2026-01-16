# Hook Generator
This tool generates RPCS3 patches from patchwork binaries.
It uses the labels from the asm hooks in the patchwork binaries.

## Usage

```
hook-generator -input patchwork.prx -config /path/to/lbpdef.yamls -output patchwork_patch.yml
```

Then the patch yaml need to be added to the imported_patch.yml in RPCS3. It will add a "Patchwork Combined Patch",
which should contain the ASM patch for each game as it's own patch.