# Ventilator Firmware

## Versions

See [CHANGELOG.md](CHANGELOG.md).

Pre-compiled binaries are available in the [Releases](https://github.com/makers-for-life/makair/releases) section.

## Documentation

Code documentation can be [found there](https://makers-for-life.github.io/makair/docs/software/firmware/html/files.html).

## How To Build?

In order to setup your environment and build the code, please follow the following commands (for MacOS):

1. `brew install arduino-cli`
2. `arduino-cli config init --additional-urls https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json`
3. `arduino-cli core update-index`
4. `arduino-cli core install STM32:stm32`
5. `arduino-cli lib install LiquidCrystal && arduino-cli lib install "Analog Buttons" && arduino-cli lib install OneButton`

Then, compile the project:

```sh
arduino-cli compile --fqbn STM32:stm32:Nucleo_64:opt=o3std,pnum=NUCLEO_F411RE --verbose src/software/firmware/srcs/respirator.cpp --output src/software/firmware/output/respirator-production
```

## Configuration

High-level configuration options are available and documented in [includes/config.h](includes/config.h).

Low-level configuration options can be found in [includes/parameters.h](includes/parameters.h).

## Sonar analysis

The following version of software are build and analysed.

| Software Mode    | Hardware | Version | Valve version | Link                                                                                                                                    |
| ---------------- | -------- | ------- | ------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| integration-test | HW1      | dev     | pinch valve   | [https://sonarcloud.io/organizations/makers-for-life/projects/respirator-integration-test-HW1-dev](respirator-integration-test-HW1-dev) |
| integration-test | HW2      | dev     | pinch valve   | [https://sonarcloud.io/organizations/makers-for-life/projects/respirator-integration-test-HW2-dev](respirator-integration-test-HW2-dev) |
| production       | HW1      | dev     | pinch valve   | [https://sonarcloud.io/organizations/makers-for-life/projects/respirator-production-HW1-dev](respirator-production-HW1-dev)             |
| production       | HW2      | dev     | pinch valve   | [https://sonarcloud.io/organizations/makers-for-life/projects/respirator-production-HW2-dev](respirator-production-HW2-dev)             |
| qualification    | HW1      | dev     | pinch valve   | [https://sonarcloud.io/organizations/makers-for-life/projects/respirator-qualification-HW1-dev](respirator-qualification-HW1-dev)       |
| qualification    | HW2      | dev     | pinch valve   | [https://sonarcloud.io/organizations/makers-for-life/projects/respirator-qualification-HW2-dev](respirator-qualification-HW2-dev)       |
