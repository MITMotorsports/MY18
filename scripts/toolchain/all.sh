#!/bin/bash

cd "${0%/*}"

./apt.sh

./pip.sh

./lpc.sh
