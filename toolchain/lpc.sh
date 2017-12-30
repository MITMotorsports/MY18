#!/bin/bash

echo "Installing lpc21isp..."

git clone https://github.com/MITEVT/lpc21isp lpc21isp
pushd lpc21isp
make

# Install
mv lpc21isp /usr/local/bin/lpc21isp
popd

rm -rf lpc21isp
