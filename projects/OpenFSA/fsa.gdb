target remote localhost:2331
file OpenFsa.elf
monitor speed 5000
monitor endian little
monitor flash breakpoints = 1
monitor flash download = 1
monitor halt
load
monitor reset
set breakpoint pending on
break main
