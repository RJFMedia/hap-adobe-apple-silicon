#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PACKAGE_VERSION="$(sed -nE 's/^set\(CPACK_PACKAGE_VERSION "([^"]+)"\).*/\1/p' "${ROOT_DIR}/installer/CodecInstaller.cmake")"
PACKAGE_FILE="HapAdobePlugin-${PACKAGE_VERSION}-macOS-arm64.pkg"

if [[ $# -eq 1 ]]; then
  PKG="${ROOT_DIR}/dist/${PACKAGE_FILE}"
  KEYCHAIN_PROFILE="$1"
elif [[ $# -eq 2 ]]; then
  PKG="$1"
  KEYCHAIN_PROFILE="$2"
else
  echo "usage: $0 <notarytool-keychain-profile>" >&2
  echo "   or: $0 <signed.pkg> <notarytool-keychain-profile>" >&2
  exit 2
fi

xcrun notarytool submit "${PKG}" \
  --keychain-profile "${KEYCHAIN_PROFILE}" \
  --wait

xcrun stapler staple "${PKG}"
xcrun stapler validate "${PKG}"
spctl --assess --type install -vv --ignore-cache "${PKG}"
