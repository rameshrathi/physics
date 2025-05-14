#!/bin/bash

set -e

# Script to download external dependencies
EXTERNAL_DIR="./external"

# Boost settings
BOOST_VERSION="1.88.0"
BOOST_DIR="${EXTERNAL_DIR}/boost"
BOOST_FILENAME="boost_${BOOST_VERSION//./_}"
BOOST_ARCHIVE="${BOOST_FILENAME}.tar.gz"
BOOST_URL="https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOST_ARCHIVE}"

# JSON settings
JSON_DIR="${EXTERNAL_DIR}/json"
JSON_VERSION="v3.11.2"
JSON_URL="https://github.com/nlohmann/json/releases/download/${JSON_VERSION}/json.hpp"

# FTXUI settings
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

if ! command_exists git; then
    echo "Error: git is required but not installed. Please install git and try again."
    exit 1
fi

# ==========================================
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

# Download nlohmann/json if it doesn't exist
echo "Checking for nlohmann/json at ${JSON_DIR}..."
if [ ! -d "${JSON_DIR}" ]; then
    echo "nlohmann/json not found. Downloading..."

    mkdir -p "${JSON_DIR}"
    echo "  -> Downloading from ${JSON_URL}"
    curl -fSL "${JSON_URL}" -o "${JSON_DIR}/json.hpp"
    echo "nlohmann/json has been downloaded to ${JSON_DIR}"
else
    echo "nlohmann/json directory already exists at ${JSON_DIR}"
fi

# Download FTXUI if it doesn't exist
echo "Checking for FTXUI at ${FTXUI_DIR}..."
if [ ! -d "${FTXUI_DIR}" ]; then
    echo "FTXUI not found. Cloning..."

    git clone --depth 1 "${FTXUI_REPO}" "${FTXUI_DIR}"
    echo "FTXUI has been cloned to ${FTXUI_DIR}"
else
    echo "FTXUI directory already exists at ${FTXUI_DIR}"
fi

# ========== Installing boost ====================
echo "=== All dependencies downloaded now installing boost ==="
cd external/boost
# Bootstrap Boost.Build and set install prefix to this directory
./bootstrap.sh --prefix="$(pwd)"

echo "=== All dependencies are set up ==="