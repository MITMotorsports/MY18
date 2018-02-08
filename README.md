# MY18
This repository contains all the code that physically runs on MY18.
External libraries are imported via submodules and found under the [`lib`](lib) directory.

Development environment and toolchain installation scripts are also provided.

# Getting Latest Changes
Run `make pull` to get the latest version of this 
repository and the appropriate submodules.

# Development Environment
On MY18, we are standardizing the build and flash toolchain for all embedded
devices on the car.
To be compliant with the standard, you will need a Linux OS to compile and
flash code from.

## Native
On Ubuntu or Ubuntu Bash on Windows 10, you can run the included
set up scripts, that will fetch the toolchain using `apt` and `git`.
Run the [`scripts/toolchain/all.sh`](scripts/toolchain/all.sh) script as **root** (with `sudo`) to set up the toolchain.
Equivalently, if you already have make run `sudo make toolchain`.

Although discouraged, check [here](https://github.com/MITMotorsports/MY18/wiki/Installing-the-toolchain-on-Windows)
for Windows installation instructions, if Ubuntu Bash on Windows 10 is not an option.
Be aware that free upgrades to Windows 10 Pro are available to MIT students
through [here](https://e5.onthehub.com/WebStore/Welcome.aspx?ws=33ef38ee-a79b-e011-969d-0030487d8897&vsro=8),
thus enabling you to use Ubuntu Windows 10.

## Docker
On other Unix-based systems, including macOS and different Linux distros, you can use Docker.

- Get latest image from DockerHub: `docker pull mitmotorsports/my18`
- Run development environment: `docker run -it mitmotorsports/my18`
- Build latest image: `docker build github.com/MITMotorsports/MY18 --tag mitmotorsports/my18`
