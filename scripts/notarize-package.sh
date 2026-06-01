#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
  echo "usage: $0 <signed.pkg> <notarytool-keychain-profile>" >&2
  exit 2
fi

PKG="$1"
KEYCHAIN_PROFILE="$2"

xcrun notarytool submit "${PKG}" \
  --keychain-profile "${KEYCHAIN_PROFILE}" \
  --wait

xcrun stapler staple "${PKG}"
xcrun stapler validate "${PKG}"
spctl --assess --type install -vv --ignore-cache "${PKG}"
