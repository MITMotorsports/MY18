#!/bin/bash
ROOT="${0%/*}/.."

cd ${ROOT}

git pull
git submodule update --init --remote

cd ${ROOT}/lib/CANlib

# Normal update, to get commit-pegged ParseCAN.
# Maybe I can repeat a git submodule update --init --recursive instead of cding.
git submodule update --init
