# Control Unit

## Versions

| Version | Last Changelog | Ready? |
| ------- | -------------- | ------ |
| V1.1.x | Operational initial release | ✅

## How To Build?

In order to setup your environment and build the code, please follow the following commands (for MacOS):

1. Install [Rustup](https://rustup.rs/)
2. Ènsure you are using the Rust stable toolchain: `rustup default stable`
3. On Linux, make sure you have cmake installed and those libraries (debian):
  `libxcb-shape0 libxcb-shape0-dev libxcb-xfixes0 libxcb-xfixes0-dev libfontconfig libfontconfig1-dev`
4. Build the project: `cargo build`

Wayland support may be available but it hasn't been tested. You need a working X11 server.

_Tested at Rust version: `rustc 1.43.0 (4fb7144ed 2020-04-20)`_

## How To Run?

To run the Control UI, please ensure that your device first has an open serial connection with the motherboard.

Take note of the serial port used as an input on your Control UI board, and call:

1. `./makair-control --port=0` (where `--port` is your serial port ID, maybe /dev/ttyUSB0)

You may also play a pre-recorded file, by passing it as an input (this is handy while developing):

1. `./makair-control --input=../telemetry/records/few_cycles` (where `--input` is an UNIX file path)

## How To Build A Release? (Cross-Compile For ARM)

To cross-compile a new release for an ARM target (using MUSL; statically-linked libraries), you can call the `release_binaries.sh` script:

1. `./scripts/release_binaries.sh --version=1.0.0`

_Make sure to replace the `version` script argument with the current release version. This is used for file naming purposes only._

## Optional Features

The Control UI behavior can be tuned at compile time, by enabling some optional features while building the binary:

* **Automatic graph scaler:**
  * Feature name: `graph-scaler`
  * Build command: `cargo build --features=graph-scaler`
  * Purpose: enables automatic graph maximum calculation, and re-adjustments as the target peak pressure is adjusted by an operator, or the largest point pressure value on screen overflows the graph. This overrides the default static maximum pressure value, that is based on the theoretical saturation maximum in an human subject. While the dynamic graph scaler system was previously the default, based on medical staff feedback it has been made static in order to improve machine usability in real-world scenarios. The auto-scaler feature was confusing for them, as traditional respirator systems use an hardcoded maximum.

## Troubleshooting

In case of an issue, please review the following points and check if any could help:

* If you are on Linux, you need to be a member of the `dialout` group for your user to be able to use the device created by the serial port;
