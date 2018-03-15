FROM debian:stretch-slim

WORKDIR /root/

COPY scripts/ scripts/

# Installing packages
RUN scripts/toolchain/apt.sh clean

# Getting and making lpc21isp
RUN scripts/toolchain/lpc.sh

# Getting repos
COPY ./ MY18/

ENTRYPOINT ["/bin/bash"]
