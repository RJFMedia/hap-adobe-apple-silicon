#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PACKAGE_VERSION="$(sed -nE 's/^set\(CPACK_PACKAGE_VERSION "([^"]+)"\).*/\1/p' "${ROOT_DIR}/installer/CodecInstaller.cmake")"
PACKAGE_FILE="HapAdobePlugin-${PACKAGE_VERSION}-macOS-arm64.pkg"

if [[ $# -eq 1 ]]; then
  UNSIGNED_PKG="${ROOT_DIR}/build-arm64/${PACKAGE_FILE}"
  SIGNED_PKG="${ROOT_DIR}/dist/${PACKAGE_FILE}"
  INSTALLER_IDENTITY="$1"
elif [[ $# -eq 3 ]]; then
  UNSIGNED_PKG="$1"
  SIGNED_PKG="$2"
  INSTALLER_IDENTITY="$3"
else
  echo "usage: $0 <developer-id-installer-identity>" >&2
  echo "   or: $0 <unsigned.pkg> <signed.pkg> <developer-id-installer-identity>" >&2
  exit 2
fi

mkdir -p "$(dirname "${SIGNED_PKG}")"

productsign --sign "${INSTALLER_IDENTITY}" "${UNSIGNED_PKG}" "${SIGNED_PKG}"
pkgutil --check-signature "${SIGNED_PKG}"
