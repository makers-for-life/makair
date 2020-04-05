#!/bin/sh

arduino-cli compile -p tty.usbmodem143103 --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE srcs/respirator.cpp --output output/respirator