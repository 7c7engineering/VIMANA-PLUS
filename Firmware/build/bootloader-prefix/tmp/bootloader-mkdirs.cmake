# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/max/esp/esp-idf/components/bootloader/subproject"
  "/Users/max/GIT/Vimana/Firmware/build/bootloader"
  "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix"
  "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix/tmp"
  "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix/src"
  "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/max/GIT/Vimana/Firmware/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
