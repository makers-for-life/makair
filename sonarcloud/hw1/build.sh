#!/bin/bash

sed -Ei 's/#define HARDWARE_VERSION [0-9]+/#define HARDWARE_VERSION 1/' src/software/firmware/includes/config.h
sed -Ei 's/#define MODE .+/#define MODE MODE_PROD/' src/software/firmware/includes/config.h
arduino-cli compile --build-path $PWD/buildProdhw1 --fqbn STM32:stm32:Nucleo_64:opt=o3std,pnum=NUCLEO_F411RE --verbose src/software/firmware/srcs/respirator.cpp --output src/software/firmware/output/respirator-production
