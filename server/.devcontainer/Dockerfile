FROM ubuntu:22.04

# Install required dependencies
RUN apt update && apt install -y \
    sudo \
    build-essential \
    cmake \
    ninja-build \
    clang \
    clangd \
    ccls \
    gdb \
    git \
    python3 \
    python3-pip \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user for development
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && mkdir -p /etc/sudoers.d \
    && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && usermod -aG sudo $USERNAME

USER $USERNAME
WORKDIR /workspace

