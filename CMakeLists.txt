cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(Barcode CXX)


################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "dll"
    "Python310"
    "PythonDebug"
    "Release"
    CACHE STRING "" FORCE
)


################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(PrjBarlib)
add_subdirectory(PrjTests)

