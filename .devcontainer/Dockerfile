# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set non-interactive frontend to avoid prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Update and install development tools and libraries
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    gdb \
    clang \
    cmake \
    make \
    git \
    curl \
    wget \
    xz-utils \
    zip \
    unzip \
    pkg-config \
    ninja-build \
    libtool \
    autoconf \
    automake \
    libssl-dev \
    libffi-dev \
    python3 \
    python3-dev \
    python3-pip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install additional C/C++ libraries
RUN apt-get update && apt-get install -y \
    libboost-all-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set up ccache for faster rebuilds (optional)
RUN apt-get update && apt-get install -y ccache \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Ensure PATH includes all tools
ENV PATH="/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin:${PATH}"

# Set the default shell to bash
SHELL ["/bin/bash", "-c"]

# Add a non-root user to work with (optional, uncomment if needed)
# ARG USERNAME=vscode
# ARG USER_UID=1000
# ARG USER_GID=$USER_UID
# RUN groupadd --gid $USER_GID $USERNAME \
#     && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
#     && apt-get update \
#     && apt-get install -y sudo \
#     && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
#     && chmod 0440 /etc/sudoers.d/$USERNAME \
#     && apt-get clean \
#     && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspaces/project