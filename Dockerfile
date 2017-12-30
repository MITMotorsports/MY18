FROM debian:stretch-slim

WORKDIR /root/

COPY toolchain/ toolchain/

# Installing packages
RUN toolchain/apt.sh clean

# Getting and making lpc21isp
RUN toolchain/lpc.sh

# Getting repos
COPY ./ MY18/

ENTRYPOINT ["/bin/bash"]
