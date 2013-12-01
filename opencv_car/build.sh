#!/bin/bash
export OPENNI2_INCLUDE=~/OpenNI/Include/
export OPENNI2_REDIST=.
#make clean
PLATFORM=x64 make ALLOW_WARNINGS=1
