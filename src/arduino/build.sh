#!/bin/sh

arduino-cli compile -p /dev/ttyACM0 --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE respi-prod.cpp