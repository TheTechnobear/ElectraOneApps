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




# orac
to connect to Orac (via MEC)

for building download electra.one firmware sdk
then from firmware directory you will need to do something like..


`
make "TOOLSPATH=${HOME}/projects/E1/tools" COMPILERPATH="/opt/homebrew/bin" APPPATH="../ElectraOneApps/orac" BUILDDIR="../ElectraOneApps/build"
`


released version of Orac will work

but you will need a recent build of MEC, (currently electraone.1 branch)  
see 
https://github.com/thetechnobear/MEC/


run orac, then run mec-app, using e1kontrol.json
config file might need change depending upon your midi setup , as it details the electra one control port
