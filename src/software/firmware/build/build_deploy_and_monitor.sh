#!/bin/sh

rm output/respirator*
rm -rf /tmp/arduino-sketch-*
arduino-cli compile --port ${BOARD_PORT} --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE srcs/respirator.cpp --output output/respirator
arduino-cli upload  --port ${BOARD_PORT} --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE --input output/respirator
pio device monitor -b 115200
