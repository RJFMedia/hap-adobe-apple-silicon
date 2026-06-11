![Hap logo](asset/hap-icon.png)

# Hap Adobe Plugin for Apple Silicon

This repository maintains a macOS Apple Silicon build of the Hap plugin for
Adobe Creative Cloud.

It provides:

- Hap export through Adobe Media Encoder and Premiere Pro.
- Hap import and playback in Premiere Pro and After Effects.
- Native `arm64` plugin bundles for current Apple Silicon Macs.

The original public Hap exporter was Intel-only on macOS. This fork keeps the
export workflow working on Apple Silicon and adds a CPU decoder path so Hap
movies can be imported and previewed in current Adobe applications.

## Download

Download the notarized macOS installer from the GitHub releases page:

https://github.com/RJFMedia/hap-adobe-apple-silicon/releases/latest

Current package:

`HapAdobePlugin-2.0.2-macOS-arm64.pkg`

## Supported Platform

- macOS on Apple Silicon (`arm64`)
- Adobe Media Encoder, Premiere Pro, and After Effects

Windows is intentionally out of scope for this fork because the existing Intel
Windows plugin still works.

## Usage

After installation, Hap export is available from Adobe Media Encoder and
Premiere Pro as the `HAP Video` format.

Supported Hap variants:

| Codec | Alpha | Notes |
| --- | --- | --- |
| Hap | No | Lowest data rate, reasonable image quality |
| Hap Alpha | Yes | Hap with alpha |
| Hap Q | No | Higher image quality, larger files |
| Hap Q Alpha | Yes | Hap Q with alpha |
| Hap Alpha-Only | Yes | Alpha-only Hap stream |

Hap movie import and playback are available in Premiere Pro and After Effects.

## Building

The public repository does not include Adobe SDK payloads. To build locally,
provide your own Adobe SDK copies at:

```sh
external/foundation/external/adobe/AfterEffectsSDK
external/foundation/external/adobe/premiere
```

Then build the unsigned package:

```sh
./scripts/build-arm64-package.sh
```

The unsigned package is written to:

```text
build-arm64/HapAdobePlugin-<version>-macOS-arm64.pkg
```

For signing and notarization:

```sh
./scripts/sign-package.sh "Developer ID Installer: YOUR NAME (TEAMID)"
./scripts/notarize-package.sh your-notarytool-keychain-profile
```

More detailed build notes are in [README-APPLE-SILICON.md](README-APPLE-SILICON.md).

## Project Notes

- The plugin is macOS-only in this source tree.
- The package installs Adobe plugin bundles into Adobe's shared MediaCore
  plugin location.
- FFmpeg is statically linked for MOV reading/writing. See
  [docs/release/ffmpeg-lgpl-compliance.md](docs/release/ffmpeg-lgpl-compliance.md)
  before publishing binary builds.
- Release packages should include the corresponding FFmpeg source archive from
  `scripts/make-ffmpeg-source-archive.sh`.

## Credits

This project builds on:

- [disguise-one/hap-encoder-adobe-cc](https://github.com/disguise-one/hap-encoder-adobe-cc)
- [codec-foundation/adobe-cc](https://github.com/codec-foundation/adobe-cc)
- [Vidvox/HapInAVFoundation](https://github.com/Vidvox/hap-in-avfoundation)
- The Hap codec work by Tom Butterworth and Vidvox

See [NOTICE.md](NOTICE.md) for full attribution and third-party notices.

## License

License terms and third-party notices are in [license.txt](license.txt),
[NOTICE.md](NOTICE.md), and the dependency license files included in this
repository.
