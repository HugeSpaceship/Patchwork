# Patchwork

> TLDR: if you're looking to play LBP online, *safely*.
> Check out the latest version of [Refresher](https://github.com/LittleBigRefresh/Refresher/releases/latest),
> Bonsai's official patcher.

Patchwork is an SPRX module that patches the LittleBigPlanet series of games
to fix some of the many security vulnerabilities the games have.

## Current Fixes

1. **Disabling P2P script syncing**  

   Removes remote script downloading between clients;  
   patching the ability to send malicious scripts to joined players.
 
2. **Join Request verification**  

   Improves logic around the handling of join requests;  
   patching force-join by disallowing auto-join for those not explicitly invited.

3. **Custom XXTea key for packets** (Optional)  

   Sets a custom key used to encrypt network packets;  
   prevents anyone without the same key from being able to join/be joined by others.

Additional features include being able to patch the game's server URL and digest key.

## Future Fixes
> [!NOTE]
> All known major security vulnerabilities are addressed by Patchwork.

If you identify new vulnerabilities, please follow the steps in [SECURITY.MD](/SECURITY.MD) or [Report an Advisory](https://github.com/HugeSpaceship/Patchwork/security/advisories/new).

## Building

Patchwork has a couple of dependencies:

- The official PS3 SDK. Version 4.75 is recommended, but others can probably be used.
- A working version of `make`. This can generally be any build of `make`, but in the case of Windows, MSYS2 MINGW64's `make` is known to work.
- `scetool` is required for signing, if desired

Once you've got everything set up, it should just be as easy as running `make` in the project directory, which should build an SPRX.

The CI pipeline should be a clearer example of how to do this.

## Installation

As mentioned previously, the easiest and most recommended way of installing or configuring Patchwork is via
[Refresher](https://github.com/LittleBigRefresh/Refresher/releases/latest).
The following instructions are intended for advanced users only.

Note that Refresher can still be used to install Patchwork with your own build. Just put your `patchwork.sprx` file next to the Refresher executable
and Refresher should say 'Found custom patchwork.sprx next to exe, uploading that instead` when (re)patching.

The game's EBOOT needs to be patched to allow it to load the SPRX.
[SPRXPatcher](https://github.com/NotNite/SPRXPatcher) is a tool that enables this quite well.

Once you've patched your EBOOT, all you need to do is put the SPRX file in the same place you specified when running SPRXPatcher.
Generally, this location should be `/dev_hdd0/plugins/patchwork.sprx` but you can put it elsewhere if desired.

Optionally, put a `patchwork_lobby_password.txt` in `/dev_hdd0/tmp` if you'd like to play with friends.
You can also use `patchwork_url.txt` and `patchwork_digest.txt` to patch to a custom server of your choice.

## Credits

[jvyden](https://github.com/jvyden) For helping me get set up with the SDK, providing emotional support, and testing.

[jackcaver](https://github.com/jackcaver) For helping to test HEN, and for figuring out a race condition with SPRX loading.

[ennuo](https://github.com/ennuo) For knowing what libraries don't work in SPRXs.

[aldostools](https://github.com/aldostools) For making notifications relatively easy to display,
and for having a workable example of reading/writing process memory using various syscalls.


Media Molecule, for making games worth patching.
