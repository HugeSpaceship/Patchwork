# Patchworks

> TLDR: if you're looking to play LBP online, *safely*.
> Check out the latest version of [Refresher](https://github.com/LittleBigRefresh/Refresher/releases/latest),
> Bonsai's official patcher.

Patchworks is an SPRX module that patches the LittleBigPlanet series of games
to fix some of the many security vulnerabilities the games have.

## Current Fixes

Currently, the only fix is changing the XXTea encryption key used for network packets,
this fixes the force-join exploit by making it impossible to join/be joined by a player
without having the same lobby password.

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

> TODO: Potentially add documentation about additional signing requirements for HEN

## Installation

As mentioned at the top of this file, the easiest and most recommended way of installing Patchworks is via
[Refresher](https://github.com/LittleBigRefresh/Refresher/releases/latest), the following instructions are intended for
advanced users only.

The game's EBOOT needs to be patched to allow it to load the SPRX, [SPRXPatcher](https://github.com/NotNite/SPRXPatcher)
is a tool that seemingly enables this quite well.

Once you've patched your EBOOT all you should need to do is put the SPRX file in the place you specified when running SPRXPatcher
and optionally put a `patchworks_lobby_password` in `/dev_hdd0/tmp` if you want to play with friends.
If you don't specify a password file, the library will randomly generate a password on boot to ensure safety 
(effectively disabling multiplayer).

