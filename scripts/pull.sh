#!/bin/bash
ROOT="${0%/*}/.."

cd ${ROOT}

git pull
git submodule update --init --recursive
