ESC Firmware
=====

**Electric Speed Controller for driving the blower motor**

Based on a STm B-G431B-ESC1 board with:
- 20kHz sensorless FOC
- Speed control via PWM input (490Hz min 1060µs-1860µs max)

---

# How to build:

First, download and install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) for your OS.

Then, open the _.project_ file and 'Build' the _Release_ flavor.

The resulting binary can be flashed from the IDE, or using an external tool such as [STM32CubeProgrammer](https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-programmers/stm32cubeprog.html)

---

# How to tune:

You need to download and install:
- STM32Cube initialization code generator [STm32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- STM32 Motor Control Software Development Kit [X-CUBE-MCSDK](https://www.st.com/en/embedded-software/x-cube-mcsdk.html)

---

# TODO

🚧 _Add Hall Sensor support_
🚧 _Fine Tune Speed/Current Loops_
