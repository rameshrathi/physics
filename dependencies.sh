#!/bin/bash

set -e

EXTERNAL_DIR="./external"

# Boost settings
BOOST_VERSION="1.88.0"
BOOST_DIR="${EXTERNAL_DIR}/boost"
BOOST_FILENAME="boost_${BOOST_VERSION//./_}"
BOOST_ARCHIVE="${BOOST_FILENAME}.tar.gz"
BOOST_URL="https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOST_ARCHIVE}"

FTXUI_DIR="${EXTERNAL_DIR}/ftxui"
FTXUI_REPO="https://github.com/ArthurSonzogni/FTXUI.git"

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required commands
if ! command_exists curl; then
    echo "Error: curl is required but not installed. Please install curl and try again."
    exit 1
fi

# Create external folder if it doesn't exist
echo "Checking for external directory..."
if [ ! -d "$EXTERNAL_DIR" ]; then
    echo "Creating external directory at ${EXTERNAL_DIR}"
    mkdir -p "$EXTERNAL_DIR"
else
    echo "External directory already exists."
fi

# Download Boost if it doesn't exist
echo "Checking for Boost at ${BOOST_DIR}..."
if [ ! -d "${BOOST_DIR}" ]; then
    echo "Boost not found. Downloading Boost ${BOOST_VERSION}..."

    mkdir -p "${BOOST_DIR}"
    echo "  -> Downloading and extracting directly into ${BOOST_DIR}"
    curl -fSL "${BOOST_URL}" | \
      tar -xz --strip-components=1 -C "${BOOST_DIR}"
    echo "Boost ${BOOST_VERSION} downloaded and extracted to ${BOOST_DIR}"
else
    echo "Boost already exists at ${BOOST_DIR}, Installing it"
fi

# Download ImgUI if it doesn't exist
if [ ! -d "imgui" ]; then
    echo "Cloning Dear ImGui..."
    git clone --depth=1 https://github.com/ocornut/imgui.git external
else
    echo "ImGui already cloned."
fi

# ========== Installing boost ====================
echo "=== All dependencies downloaded now installing boost ==="
cd external/boost
./bootstrap.sh --prefix="$(pwd)"
./b2
echo "Installation completed..."