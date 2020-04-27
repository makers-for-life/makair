# Control Unit

## Versions

| Version | Last Changelog | Ready? |
| ------- | -------------- | ------ |
| V1.0.x | Project started | ❌

## How To Build?

In order to setup your environment and build the code, please follow the following commands (for MacOS):

1. Install [Rustup](https://rustup.rs/)
2. Ènsure you are using the Rust stable toolchain: `rustup default stable`
3. Build the project: `cargo build`

## How To Run?

To run the Control UI, please ensure that your device first has an open serial connection with the motherboard.

Take note of the serial port used as an input on your Control UI board, and call:

1. `./makair-control --port=0` (where `--port` is your serial port ID)

## How To Build A Release? (Cross-Compile For ARM)

To cross-compile a new release for an ARM target (using MUSL; statically-linked libraries), you can call the `release_binaries.sh` script:

1. `./scripts/release_binaries.sh --version=1.0.0`

_Make sure to replace the `version` script argument with the current release version. This is used for file naming purposes only._
