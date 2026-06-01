# Hap Adobe Plugin for Apple Silicon

This is a cleaned, self-contained macOS source tree for building the Hap Adobe
Creative Cloud plugin on Apple Silicon.

The plugin exports Hap through Adobe Media Encoder and includes an Apple Silicon
CPU decoder path for playback/import in Premiere Pro and After Effects.

For upstream project credits, third-party notices, and redistribution license
notes, see [NOTICE.md](NOTICE.md) and [license.txt](license.txt).

## Layout

- `codec/`: Hap encoder/decoder implementation.
- `external/foundation/`: Adobe plugin framework, importer/exporter glue, and
  installer framework.
- `external/foundation/external/adobe/`: local Adobe SDK mount point. The
  `AfterEffectsSDK` and `premiere` SDK payloads are ignored so they are not
  accidentally published.
- `external/foundation/external/ffmpeg/`: vendored FFmpeg headers and static
  libraries used by the movie reader/writer.
- `external/hap`, `external/snappy`, `external/squish`, `external/ycocg`:
  codec dependencies.
- `asset/`: Adobe Media Encoder presets and plugin artwork.
- `installer/`: codec-specific installer metadata.
- `scripts/`: build, signing, and notarization helpers.
- `docs/release/`: Apple Silicon port notes and source references.

## Build

Before building, provide local Adobe SDK inputs:

```sh
ln -s "/path/to/AfterEffectsSDK" \
  external/foundation/external/adobe/AfterEffectsSDK

ln -s "/path/to/Premiere Pro 26.0 C++ SDK/Examples" \
  external/foundation/external/adobe/premiere
```

```sh
./scripts/build-arm64-package.sh
```

To create an unsigned package after the build:

```sh
(cd build-arm64 && COPYFILE_DISABLE=1 cpack -D CPACK_PRODUCTBUILD_IDENTITY_NAME=)
```

## Signing and Notarization

```sh
./scripts/sign-package.sh \
  build-arm64/HapEncoder-1.2.0-rc2-macOS.pkg \
  dist/HapEncoder-1.2.0-rc2-macOS-arm64.pkg \
  "Developer ID Installer: YOUR NAME (TEAMID)"

./scripts/notarize-package.sh \
  dist/HapEncoder-1.2.0-rc2-macOS-arm64.pkg \
  your-notarytool-keychain-profile
```

## Initialize a Git Repo

```sh
git init
git add .
git commit -m "Initial Apple Silicon Hap Adobe plugin source"
```

`build-*`, `dist`, logs, packages, and local signing files are ignored.
