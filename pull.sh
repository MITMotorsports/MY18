#!/bin/bash
ROOT="${0%/*}"

cd ${ROOT}

git pull
git submodule update --init --remote

cd ${ROOT}/lib/CANlib

# Normal update, to get commit-pegged ParseCAN.
git submodule update --init
