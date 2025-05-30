#!/usr/bin/env bash

set -euo pipefail

IFS=$'\n\t'

BUILD_DIR="build"
ELF_FILE="$BUILD_DIR/Run/RUN"

# Build the project
build() {
	echo "[+] building project..."
	cmake -S . -B "$BUILD_DIR"
	cmake --build "$BUILD_DIR"
}

# Run all tests
test() {
	echo "[+] running tests..."
	if [[ ! -d "$BUILD_DIR" ]]; then
		echo "[!] Build directory not found. Run './lab build' first."
		exit 1
	fi
	cd "$BUILD_DIR/Tests"
	ctest --output-on-failure
}

# Run app
run() {
	echo "[+] running application..."
  if [[ ! -f "$ELF_FILE" ]]; then
      echo "[!] Executable not found at : $ELF_FILE  => Run './script.sh build' first."
      exit 1
  fi
  "$ELF_FILE"
}

clear() {
  echo "removing build files..."
  rm -rf "$BUILD_DIR"
}

usage() {
	echo "Usage : $0 < build | test | run | clean >"
	exit 1
}

main() {
	[[ $# -eq 0 ]] && usage
	case "$1" in
	build) build;;
  b) build;;
	test) test;;
  t) test;;
	run) run;;
  r) run;;
  clear) clear;;
  c) clear;;
	*) usage;;
	esac
}

main "$@"
