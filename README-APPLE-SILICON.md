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

The unsigned package is written to
`build-arm64/HapAdobePlugin-<version>-macOS-arm64.pkg`.

## Signing and Notarization

```sh
./scripts/sign-package.sh \
  "Developer ID Installer: YOUR NAME (TEAMID)"

./scripts/notarize-package.sh \
  your-notarytool-keychain-profile
```

The signed and notarized release package uses the same versioned name:
`dist/HapAdobePlugin-<version>-macOS-arm64.pkg`.

## Initialize a Git Repo

```sh
git init
git add .
git commit -m "Initial Apple Silicon Hap Adobe plugin source"
```

`build-*`, `dist`, logs, packages, and local signing files are ignored.
