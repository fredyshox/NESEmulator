#!/bin/sh

mkdir -p ./Build

cd ./Build

cmake ../../../Emulator -GXcode
