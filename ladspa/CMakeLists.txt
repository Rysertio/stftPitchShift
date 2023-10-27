cmake_minimum_required(VERSION 3.1...3.18)

project(Pitcher)

add_library(pitchshift SHARED "${CMAKE_CURRENT_LIST_DIR}/pitcher.cpp")

# at least C++ 11 is mandatory
target_compile_features(pitchshift PRIVATE cxx_std_11)

# just the case of default build e.g. if LibStftPitchShift.cmake is already included
target_link_libraries(pitchshift stftpitchshift)

# if installed via ppa:jurihock/stftpitchshift on Ubuntu
# target_link_libraries(example stftpitchshift)

# if installed via vcpkg
# find_package(stftpitchshift CONFIG REQUIRED)
# target_link_libraries(example stftpitchshift::stftpitchshift)
