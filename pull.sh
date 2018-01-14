#!/bin/bash
cd "${0%/*}"

git pull
git submodule update --init --recursive --remote
