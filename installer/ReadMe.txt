Hap Adobe Plugin for Apple Silicon

This package installs Hap import, playback, and export support for Adobe Creative Cloud on Apple Silicon Macs.

Export:
- Adobe Media Encoder
- Adobe Premiere Pro

Import and playback:
- Adobe Premiere Pro
- Adobe After Effects

Optional cleanup:
The installer includes a selectable cleanup item for the legacy Vidvox QuickTime Hap.component.

That older QuickTime component was installed at /Library/QuickTime/Hap.component by the original Hap codec package and was built for Intel-era QuickTime. On Apple Silicon Macs, current Adobe apps may still scan it and warn that an Intel-based plug-in named "HAP" is incompatible, even when this native Adobe plug-in is installed and working.

Leave "Remove legacy QuickTime Hap component" selected if you want the installer to scan for Intel-only Hap.component bundles and move them out of QuickTime's load path. The cleanup only targets Intel-only legacy QuickTime components; it does not remove the Apple Silicon Adobe MediaCore plug-ins installed by this package.

Credits:
Built from the original Hap Exporter for Adobe Creative Cloud, the Codec Foundation Adobe CC framework, and Hap codec work by Tom Butterworth and Vidvox.

This software uses FFmpeg libraries under LGPL v2.1-or-later. See the license page and public source tree for full notices.
