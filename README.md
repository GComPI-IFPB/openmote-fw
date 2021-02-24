To use this project properly you will need to install some packages and software.

## Project submodule
First make sure that the cc2538-bsl submodule is installed.
Run the following commands on a terminal at the openmote-fw root folder.
```sh
git submodule init
git submodule update
```
## SCONS
Install SCONS.
```sh
pip3 install scons
```
Install intelhex package, this package is for scons to understand and write .hex files in openmote.
```sh
pip3 install intelhex
```
## Dependencies
For ubuntu/debian systems install this packages.
```sh
sudo apt-get install gcc-arm-none-eabi gdb-arm-none-eabi openocd lm4flash libstdc++-arm-none-eabi-newlib
```
## SCONS usage
To generate a .hex file in `./build/openmote-fw/projects/project-name` you should run the following command:
```sh
scons board=openmote-b project=experiment-tx compiler=gcc verbose=0
```
If you want to write the .hex file in the openmote-b run the following command:
<!-- sudo python3 ./tools/cc2538-bsl/cc2538-bsl.py -e -w --bootloader-invert-lines -b 115200 -p /dev/ttyUSB1 ./build/openmote-b/projects/experiment-tx/experiment-tx.hex -->
```sh
scons board=openmote-b project=experiment-tx compiler=gcc verbose=0 bootload=/dev/ttyUSB1
```
On bootload device you can pass a list, a range or a wildcard character.
```
list  -> /dev/ttyUSB0,ttyUSB1,/dev/ttyUSB2
range -> /dev/ttyUSB[0-2] = /dev/ttyUSB0,ttyUSB1,/dev/ttyUSB2
all   -> /dev/ttyUSBX = /dev/ttyUSB0,ttyUSB1,/dev/ttyUSB2
```

To erase the build files run the command with `-c` flag 
```sh
scons board=openmote-b project=experiment-tx compiler=gcc verbose=0 -c
```

You can find more information in [openmote-b user guide](https://www.industrialshields.com/web/content?model=ir.attachment&field=datas&id=208800&). 

The original [github project](https://github.com/openmote/openmote-fw).