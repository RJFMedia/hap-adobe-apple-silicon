#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FFMPEG_DIR="$ROOT_DIR/external/foundation/external/ffmpeg/FFmpeg"
OUT_DIR="${1:-$ROOT_DIR/dist}"

if [ ! -d "$FFMPEG_DIR" ]; then
  echo "FFmpeg source tree not found: $FFMPEG_DIR" >&2
  exit 1
fi

VERSION=$(
  awk -F\" '/FFMPEG_VERSION/ { print $2; found=1 } END { if (!found) exit 1 }' \
    "$FFMPEG_DIR/libavutil/ffversion.h" 2>/dev/null || echo "4.0.2"
)

mkdir -p "$OUT_DIR"
ARCHIVE="$OUT_DIR/ffmpeg-$VERSION-lgpl-source.tar.gz"

COPYFILE_DISABLE=1 tar \
  --exclude './ffbuild' \
  --exclude '*.a' \
  --exclude '*.o' \
  --exclude '*.d' \
  -czf "$ARCHIVE" \
  -C "$(dirname -- "$FFMPEG_DIR")" \
  FFmpeg

echo "$ARCHIVE"
