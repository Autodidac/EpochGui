#!/usr/bin/env bash
set -euo pipefail
: "${VCPKG_ROOT:?VCPKG_ROOT is not set}"
command -v glslc >/dev/null || { echo "Install the Vulkan SDK or add glslc to PATH."; exit 1; }
cmake --preset linux-release
cmake --build --preset linux-release
ctest --test-dir build/linux-release --output-on-failure
echo "Built: build/linux-release/EpochRunner"
