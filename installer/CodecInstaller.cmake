cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

set(CPACK_PACKAGE_NAME "HapAdobePlugin")
set(CPACK_PACKAGE_VENDOR "HapCommunity")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hap import and export plugin for Adobe Creative Cloud on Apple Silicon")
set(CPACK_PACKAGE_VERSION "2.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "HapEncoderPlugin")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_LIST_DIR}/../license.txt)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/ReadMe.txt)

if (APPLE)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}" ${CMAKE_MODULE_PATH})
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-macOS-arm64")
    set(CPACK_PREFLIGHT_PLUGIN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/preinstall")
    set(CPACK_POSTFLIGHT_LEGACYQUICKTIMECLEANUP_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/legacy-quicktime-cleanup-postinstall")
    set(CPACK_COMPONENT_LEGACYQUICKTIMECLEANUP_DISPLAY_NAME "Remove legacy QuickTime Hap component")
    set(CPACK_COMPONENT_LEGACYQUICKTIMECLEANUP_DESCRIPTION "Optional, recommended cleanup. Scans for the old Vidvox QuickTime Hap.component in /Library/QuickTime and user QuickTime folders. Intel-only QuickTime components can trigger Adobe's incompatible Intel plug-in warning on Apple Silicon, even though this Adobe plug-in is native. This option moves any Intel-only legacy Hap.component out of QuickTime's load path and forgets the old Vidvox installer receipt.")
endif (APPLE)

# NSIS specific settings
set(CPACK_NSIS_URL_INFO_ABOUT "https://hap.video")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_PACKAGE_NAME "Hap Adobe Plugin")
set(CPACK_NSIS_HELP_LINK "https://github.com/disguise-one/hap-encoder-adobe-cc")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/disguise-one/hap-encoder-adobe-cc/tree/master/doc/user_guide/")
set(CPACK_NSIS_CONTACT "happlugin@disguise.one")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
  "Delete \\\"$PROGRAMFILES64\\\\Adobe\\\\Common\\\\Plug-ins\\\\7.0\\\\MediaCore\\\\CodecPluginFoundation.prm\\\""
  "\\nDelete \\\"$PROGRAMFILES64\\\\Adobe\\\\Common\\\\Plug-ins\\\\7.0\\\\MediaCore\\\\HapEncoderPlugin.prm\\\""
)

# set(bitmap_path ${CMAKE_CURRENT_LIST_DIR}/../asset/install_image.bmp)
# STRING(REPLACE "/" "\\" bitmap_path  ${bitmap_path}) 
# set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP ${bitmap_path})
