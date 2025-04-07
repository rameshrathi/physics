# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set non-interactive frontend to avoid prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Update and install basic tools (including xz-utils)
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    gdb \
    cmake \
    make \
    git \
    curl \
    xz-utils \
    && apt-get clean

# Install ARM GCC Toolchain for 32-bit ARM (Cortex-M, Cortex-R)
RUN curl -Lo arm-gnu-toolchain-12.2.rel1-aarch64-arm-none-eabi.tar.xz "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-aarch64-arm-none-eabi.tar.xz" \
  && tar -xJf arm-gnu-toolchain-12.2.rel1-aarch64-arm-none-eabi.tar.xz -C /opt \
  && rm arm-gnu-toolchain-12.2.rel1-aarch64-arm-none-eabi.tar.xz \
  && ln -s /opt/arm-gnu-toolchain-12.2.rel1-aarch64-arm-none-eabi/bin/* /usr/local/bin/

# Install Python toolchain
RUN apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    && ln -s /usr/bin/python3 /usr/bin/python \
    && pip3 install --upgrade pip

# Install additional tools for IntelliSense and development
RUN apt-get install -y \
    clang \
    clang-format \
    clang-tidy \
    && pip3 install pylint

# Set up working directory
WORKDIR /workspace

# Ensure PATH includes ARM tools
ENV PATH="/usr/local/bin:${PATH}"