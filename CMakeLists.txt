cmake_minimum_required(VERSION 3.5)

# This is a minimal CMake configuration for an ESP‑IDF based project.
# The real project would specify component dependencies and build
# settings; here we simply declare the project and rely on the IDF
# build system when present.

set(EXTRA_COMPONENT_DIRS "components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(reptile_manager)