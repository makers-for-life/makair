#!/bin/sh

rm out.csv
rm respi-prod.cpp.STM32.stm32.Nucleo_64.*
rm -rf /tmp/arduino-sketch-*
arduino-cli compile -p /dev/ttyACM0 --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE respi-prod.cpp
arduino-cli upload  --port /dev/ttyACM0 --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE --input respi-prod.cpp.STM32.stm32.Nucleo_64.bin
pio device monitor -b 115200 | tee out.csv