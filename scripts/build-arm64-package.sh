#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-arm64"
MACOSX_SDK="$(xcrun --sdk macosx --show-sdk-path)"

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_SYSROOT="${MACOSX_SDK}" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0

cmake --build "${BUILD_DIR}" --parallel "$(sysctl -n hw.ncpu)"

echo "Built plugins in ${BUILD_DIR}"
echo "To package without signing:"
echo "  (cd \"${BUILD_DIR}\" && COPYFILE_DISABLE=1 cpack -D CPACK_PRODUCTBUILD_IDENTITY_NAME=)"
