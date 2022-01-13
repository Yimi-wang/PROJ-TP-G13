#!/bin/bash

./Phase2 --section-start .text=0x58 --section-start .data=0x1000 -o $1 tests/$1.o
printf "\n Disassembly of section .text \n"
arm-none-eabi-objdump -Dx $1 | awk '/Disassembly of section .text:/{flag=1;next}/Disassembly/{flag=0}flag'
printf "\n Disassembly of section .data \n"
arm-none-eabi-objdump -Dx $1 | awk '/Disassembly of section .data:/{flag=1;next}/Disassembly/{flag=0}flag'
