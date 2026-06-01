#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 3 ]]; then
  echo "usage: $0 <unsigned.pkg> <signed.pkg> <developer-id-installer-identity>" >&2
  exit 2
fi

UNSIGNED_PKG="$1"
SIGNED_PKG="$2"
INSTALLER_IDENTITY="$3"

productsign --sign "${INSTALLER_IDENTITY}" "${UNSIGNED_PKG}" "${SIGNED_PKG}"
pkgutil --check-signature "${SIGNED_PKG}"
