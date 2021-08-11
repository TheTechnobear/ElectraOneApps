# ElectraOneApps

firmware for the Electra.One (see https://electra.one)





where :
TOOLSPATH = location of E1 tools
COMPILERPATH : location of arm-none-eabi compiler
APPPATH : where you cloned this repo into
BUILDDIR : location for build

see firmware Makefile for more details


IMPORTANT NOTE: 
CMake is **NOT** supported at this time, I use the firmware makefile as above.

the CMake files are currently only used in conjuction with my IDE for editing purposes! 
perhaps this may change in the future, perhaps not!

if you want to make it work, I'd love a PR  :) 




#orac
to connect to Orac (via MEC)

for building download electra.one firmware sdk
then from firmware directory you will need to do something like..


`
make "TOOLSPATH=${HOME}/projects/E1/tools" COMPILERPATH="/opt/homebrew/bin" APPPATH="../OracE1Firmware/orac" BUILDDIR="../OracE1Firmware/build"
`
