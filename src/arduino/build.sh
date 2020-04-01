#!/bin/sh

arduino-cli compile -p ${BOARD_PORT} --fqbn STM32:stm32:Nucleo_64:pnum=NUCLEO_F411RE respi-prod.cpp