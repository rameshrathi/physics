#!/bin/bash

set -e

EXTERNAL_DIR="./external"

# Boost settings
BOOST_VERSION="1.88.0"
BOOST_DIR="${EXTERNAL_DIR}/boost"
BOOST_FILENAME="boost_${BOOST_VERSION//./_}"
BOOST_ARCHIVE="${BOOST_FILENAME}.tar.gz"
BOOST_URL="https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOST_ARCHIVE}"

# Create external folder if it doesn't exist
if [ ! -d "$EXTERNAL_DIR" ]; then
    echo "Creating external directory at ${EXTERNAL_DIR}"
    mkdir -p "$EXTERNAL_DIR"
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
    cd external/boost
    ./bootstrap.sh --prefix="$(pwd)"
    ./b2
    cd ../..
else
    echo "Boost already installed..."
fi

# Download ImgUI if it doesn't exist
if [ ! -d "external/imgui" ]; then
    echo "Cloning Dear ImGui..."
    git clone -b docking --depth=1 https://github.com/ocornut/imgui.git "external/imgui"
    echo "ImGui fetched into external"
fi

# clone or update GLFW
if [ ! -d "external/glfw" ]; then
  git clone https://github.com/glfw/glfw.git "external/glfw"
fi

# generate CMakeLists.txt for ImGui
if [ ! -f "external/imgui/CMakeLists.txt" ]; then
  cat > "external/imgui/CMakeLists.txt" << 'EOF'
  cmake_minimum_required(VERSION 3.14)
  project(imgui)

  add_library(imgui
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
  )

  target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
  )

  find_package(OpenGL REQUIRED)
  target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
EOF
fi

echo "=== All dependencies installed ==="
