## Launchiine (WIP)

A simple Wii U Menu replacement, still in early development and not ready for a day to day usage

## Usage (Replace Wii U Menu via Mocha Payload):
- Place the `men.rpx` on the sd card in the directory `sd:/wiiu`.
- Load the [MochaPayload](https://github.com/wiiu-env/MochaPayload) via the [EnvironmentLoader](https://github.com/wiiu-env/EnvironmentLoader) (e.g. [Tiramisu](https://github.com/wiiu-env/Tiramisu))
- Load the Wii U Menu and launchiine should show up instead.

## Known Issues
- Random crashes
- The Keyboard input is implemented, but result is ignored.
- nn::spm is not initalized and no quick start menu support. For the it's relying on the [AutobootModule](https://github.com/wiiu-env/AutobootModule) doing this.
- No sound on splash screen.
- Probably a lot more

## TODOs
- Non-touch controls
- Sound on splashscreen
- Folder support
- Preserve app order after closing/opening launchiine.
- Display applets like the original Wii U Menu 
- Implement Account selection when no default account is set. 
- Implement update check/no way to update games
- Properly implement nn::spm and nn:sl (external storage and quick start menu)
- Fix search
- Implement all the other stuff the Wii U Menu offers (Account creationg, switching between Accounts, set default account etc.)
- Implement ways to launch the original Wii U Menu.

## Building
Install the following dependencies:
- [wut](https://github.com/devkitPro/wut)
- [libgui](https://github.com/wiiu-env/libgui)

Then build via `make`.

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t launchiine-builder

# make 
docker run -it --rm -v ${PWD}:/project launchiine-builder make

# make clean
docker run -it --rm -v ${PWD}:/project launchiine-builder make clean
```
