cmake_minimum_required(VERSION 3.1...3.18)

project(LibStftPitchShift)

include(GNUInstallDirs)

add_library(LibStftPitchShift)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION)
string(STRIP "${VERSION}" VERSION)

set_target_properties(LibStftPitchShift
  PROPERTIES VERSION "${VERSION}"
)

set_target_properties(LibStftPitchShift
  PROPERTIES SOVERSION 1
)

set_property(
  TARGET LibStftPitchShift
  PROPERTY INTERFACE_LibStftPitchShift_MAJOR_VERSION 1)

set_property(
  TARGET LibStftPitchShift APPEND
  PROPERTY COMPATIBLE_INTERFACE_STRING LibStftPitchShift_MAJOR_VERSION)

set_target_properties(LibStftPitchShift
  PROPERTIES OUTPUT_NAME "stftpitchshift"
)

set(HEADERS
  "${CMAKE_CURRENT_LIST_DIR}/Cepster.h"
  "${CMAKE_CURRENT_LIST_DIR}/Dump.h"
  "${CMAKE_CURRENT_LIST_DIR}/FFT.h"
  "${CMAKE_CURRENT_LIST_DIR}/Pitcher.h"
  "${CMAKE_CURRENT_LIST_DIR}/Resampler.h"
  "${CMAKE_CURRENT_LIST_DIR}/RFFT.h"
  "${CMAKE_CURRENT_LIST_DIR}/STFT.h"
  "${CMAKE_CURRENT_LIST_DIR}/StftPitchShift.h"
  "${CMAKE_CURRENT_LIST_DIR}/StftPitchShiftCore.h"
  "${CMAKE_CURRENT_LIST_DIR}/Timer.h"
  "${CMAKE_CURRENT_LIST_DIR}/Version.h"
  "${CMAKE_CURRENT_LIST_DIR}/Vocoder.h"
)

set(SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/StftPitchShift.cpp"
)

target_sources(LibStftPitchShift
  PRIVATE ${HEADERS} ${SOURCES}
)

set_target_properties(LibStftPitchShift
  PROPERTIES PUBLIC_HEADER "${HEADERS}"
)

target_include_directories(LibStftPitchShift
  PUBLIC    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>"
  INTERFACE "$<INSTALL_INTERFACE:include/StftPitchShift>"
)

target_compile_features(LibStftPitchShift
  PRIVATE cxx_std_11
)

install(
  TARGETS LibStftPitchShift
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/StftPitchShift"
)
