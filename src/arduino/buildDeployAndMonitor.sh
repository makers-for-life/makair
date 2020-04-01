#!/bin/sh

rm respi-prod.cpp.STM32.stm32.Nucleo_64.*
rm -rf /tmp/arduino-sketch-*
arduino-cli compile -p ${BOARD_PORT} --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE respi-prod.cpp
arduino-cli upload  --port ${BOARD_PORT} --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE --input respi-prod.cpp.STM32.stm32.Nucleo_64.bin
pio device monitor -b 115200