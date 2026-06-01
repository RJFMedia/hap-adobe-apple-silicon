# FFmpeg LGPL Compliance Notes

This is an engineering checklist for this repository's FFmpeg usage. It is not
legal advice.

Authoritative references:

- FFmpeg legal checklist: https://www.ffmpeg.org/legal.html
- GNU LGPL 2.1 text: https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
- Local FFmpeg LGPL text:
  `external/foundation/external/ffmpeg/FFmpeg/COPYING.LGPLv2.1`

## Current FFmpeg Build Facts

- FFmpeg version: `n4.0.2-3-g9cc5337247`
- Release file: `4.0.2`
- License reported by the configured tree: `LGPL version 2.1 or later`
- Linked archives:
  - `external/foundation/external/ffmpeg/FFmpeg/libavcodec/libavcodec.a`
  - `external/foundation/external/ffmpeg/FFmpeg/libavformat/libavformat.a`
  - `external/foundation/external/ffmpeg/FFmpeg/libavutil/libavutil.a`
- Archive architecture: `arm64`
- Link mode: static archives imported by CMake and linked through
  `CodecFoundationSession`.
- Enabled functionality: MOV muxer and MOV demuxer.
- Explicitly disabled in the configuration: network, zlib, iconv, x86 asm.
- GPL/nonfree status from `config.h`:
  - `CONFIG_GPL 0`
  - `CONFIG_NONFREE 0`
  - `CONFIG_VERSION3 0`
  - `CONFIG_GPLV3 0`
  - `CONFIG_LIBX264 0`
  - `CONFIG_LIBX265 0`
  - `CONFIG_OPENSSL 0`

Configure line recorded in `config.h`:

```sh
--cc=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang --arch=arm64 --target-os=darwin --sysroot=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.5.sdk --disable-x86asm --disable-network --disable-everything --enable-muxer=mov --enable-demuxer=mov --disable-zlib --disable-iconv --enable-pic --extra-cflags='-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.5.sdk -mmacosx-version-min=11.0' --extra-cxxflags='-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.5.sdk -mmacosx-version-min=11.0' --extra-objcflags='-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.5.sdk -mmacosx-version-min=11.0' --extra-ldflags='-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.5.sdk -mmacosx-version-min=11.0'
```

## Static Linking Implication

The current build statically links FFmpeg libraries into the Adobe plugin
binaries. FFmpeg's own checklist says dynamic linking is the easiest compliance
path, but the LGPL 2.1 also allows static linking when recipients receive the
materials needed to modify FFmpeg and relink the combined work.

For this public source release, keep the full plugin source, build scripts, and
the exact FFmpeg source available to recipients. The Adobe SDK cannot be
redistributed here, so binary release notes should say that rebuilding requires
the user to obtain the Adobe SDK separately. Before broad binary distribution,
have counsel review whether the Adobe SDK dependency creates any issue for the
LGPL relinkability requirement.

## Release Checklist

For every public binary installer release:

- Keep FFmpeg configured without `--enable-gpl` and without `--enable-nonfree`.
- Keep the configure line visible in `config.h` and this document.
- Include `NOTICE.md`, `license.txt`, and FFmpeg LGPL/GPL license files in the
  public source tree.
- Publish the exact source tree used to build the binary.
- Publish an FFmpeg source archive next to the installer:

  ```sh
  ./scripts/make-ffmpeg-source-archive.sh
  ```

- Put this notice on the GitHub release page wherever the installer is
  downloadable:

  ```text
  This software uses libraries from the FFmpeg project under the LGPLv2.1.
  The corresponding FFmpeg source used for this build is available in the
  release assets and in this repository under
  external/foundation/external/ffmpeg/FFmpeg.
  ```

- Do not use an installer EULA or release terms that forbid reverse engineering
  for debugging modifications to the LGPL-covered FFmpeg portions.
- Do not imply the FFmpeg project endorses this plugin or installer.
- Re-run the checks below before publishing.

## Verification Commands

```sh
rg -n 'FFMPEG_LICENSE|FFMPEG_CONFIGURATION|CONFIG_GPL|CONFIG_NONFREE|CONFIG_VERSION3|CONFIG_GPLV3|CONFIG_LIBX264|CONFIG_LIBX265|CONFIG_OPENSSL|CONFIG_MOV_MUXER|CONFIG_MOV_DEMUXER' \
  external/foundation/external/ffmpeg/FFmpeg/config.h

for f in \
  external/foundation/external/ffmpeg/FFmpeg/libavcodec/libavcodec.a \
  external/foundation/external/ffmpeg/FFmpeg/libavformat/libavformat.a \
  external/foundation/external/ffmpeg/FFmpeg/libavutil/libavutil.a
do
  file "$f"
  lipo -info "$f"
done
```

## Lower-Risk Future Option

If this project starts shipping frequent public binaries, consider switching the
macOS build to link FFmpeg dynamically and ship clearly named FFmpeg dylibs.
That would align more closely with FFmpeg's preferred checklist and reduce the
need to think about relinkable static objects for each release.
