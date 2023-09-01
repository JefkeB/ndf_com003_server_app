#!/bin/bash
export PATH=$PATH:$PWD/tools
#source ~/Work/comol/exports_rpi_compiler

export ARCH=arm
export PATH=/home/jef/Work/comol/armv6-rpi-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=rmv6-rpi-linux-gnueabihf-

code .
