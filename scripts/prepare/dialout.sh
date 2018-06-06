#!/bin/bash

sudo adduser $USER dialout

# Super unsafe write permission acquisition
sudo chmod 666 /dev/ttyS*
sudo chmod 666 /dev/ttyUSB*
