# Patchwork

> TLDR: if you're looking to play LBP online, *safely*.
> Check out the latest version of [Refresher](https://github.com/LittleBigRefresh/Refresher/releases/latest),
> Bonsai's official patcher.

Patchworks is an SPRX module that patches the LittleBigPlanet series of games
to fix some of the many security vulnerabilities the games have.

## Current Fixes

Currently, the only fix is changing the XXTea encryption key used for network packets,
this fixes the force-join exploit by making it impossible to join/be joined by a player
without having the same lobby password. Additionally it allows you to patch
the game's URL, and digest key.

## Future Fixes

The ideal patches for something like this are as follows:

1. A real fix for force-join, this could come in the form of 
   validating a list of users that have had invites sent to them,
   and rejecting all users not on that list.

2. A fix for P2P script syncing, as this is the biggest threat to the online security
   of LittleBigPlanet
    

## Building

Patchworks needs the PS3 SDK to build; I use 4.75, though I doubt that it's the only version that would work.
You also need make working on your machine, this was a bit of a challenge to set up for me as I am using Windows on ARM.

Once you've got everything set up though it should just be as easy as running Make in the project directory, which should build an SPRX.

By default it will just create a fake-signed SPRX file, though there is a makefile target to sign with SCEtool. 
The CI pipeline should have an example of how to do this.  

## Installation

As mentioned at the top of this file, the easiest and most recommended way of installing Patchworks is via
[Refresher](https://github.com/LittleBigRefresh/Refresher/releases/latest); the following instructions are intended for
advanced users only.

The game's EBOOT needs to be patched to allow it to load the SPRX, [SPRXPatcher](https://github.com/NotNite/SPRXPatcher)
is a tool that seemingly enables this quite well.

Once you've patched your EBOOT all you should need to do is put the SPRX file in the place you specified when running SPRXPatcher
and optionally put a `patchwork_lobby_password.txt` in `/dev_hdd0/tmp` if you want to play with friends.
If you don't specify a password file, the library will randomly generate a password on boot to ensure safety 
(effectively disabling multiplayer).

## Credits

[jvyden](https://github.com/jvyden) For helping me get set up with the SDK, providing emotional support, and testing.

[jackcaver](https://github.com/jackcaver) For helping to test HEN, and for figuring out a race condition with SPRX loading.

[ennuo](https://github.com/ennuo) For knowing what libraries don't work in SPRXs.

[aldostools](https://github.com/aldostools) For making notifications relatively easy to display,
and for having a workable example of reading/writing process memory using various syscalls.


Media Molecule, for making games worth patching.