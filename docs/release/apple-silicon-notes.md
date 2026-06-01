# Apple Silicon Notes

This tree is the cleaned macOS Apple Silicon build of the Hap exporter/importer
for Adobe Creative Cloud.

## What changed from the original exporter

- Builds as `arm64` with macOS 11.0 as the deployment target.
- Avoids x86-only SSE flags in the top-level project and squish build.
- Enables a CPU Hap decoder so Adobe can play Hap media on Apple Silicon.
- Uses HapInAVFoundation-style decode behavior:
  - Hap frame data is expanded to rounded 4x4 DXT/RGTC texture buffers.
  - DXT and RGTC blocks are decoded block-by-block.
  - Decoded pixels are cropped back to the real frame size.
  - Hap1 and HapY are forced opaque.
- Reports alpha metadata by subtype:
  - `Hap1`, `HapY`: no alpha.
  - `Hap5`, `HapM`, `HapA`: straight alpha.
- Registers the importer for QuickTime movie files and accepts Hap stream tags
  `Hap1`, `Hap5`, `HapY`, `HapM`, and `HapA`.

## Known build assumptions

- macOS build only.
- Xcode command line tools are installed.
- Homebrew Boost is installed and visible to CMake.
- The included Adobe SDK header subsets are enough for this plugin build.
- FFmpeg is vendored as prebuilt static libraries under
  `external/foundation/external/ffmpeg/FFmpeg`.

## Release Validation

Before publishing a package, validate the build locally, sign with your own
Developer ID Installer certificate, submit it with `notarytool`, staple the
ticket, and run `spctl --assess --type install` on the final `.pkg`.
