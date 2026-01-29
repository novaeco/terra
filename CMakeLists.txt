cmake_minimum_required(VERSION 3.5)

# This is a minimal CMake configuration for an ESP‑IDF based project.
# The real project would specify component dependencies and build
# settings; here we simply declare the project and rely on the IDF
# build system when present.

# Optional override: set REPTILE_NO_PSRAM=1 to append the no-PSRAM defaults.
if(DEFINED ENV{REPTILE_NO_PSRAM} AND "$ENV{REPTILE_NO_PSRAM}" STREQUAL "1")
    set(SDKCONFIG_DEFAULTS "sdkconfig.defaults;sdkconfig.defaults.no_psram")
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(reptile_manager)
