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

# Ensure PATH includes ARM tools
ENV PATH="/usr/local/bin:${PATH}"