cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(Barcode CXX)
set(CMAKE_VS_PLATFORM_NAME "x64")

# if(NOT OPENCV_BOOST AND (CMAKE_CONFIGURATION_TYPES STREQUAL "Python37" OR
#     CMAKE_CONFIGURATION_TYPES STREQUAL "Python38" OR
#     CMAKE_CONFIGURATION_TYPES STREQUAL "PythonDebug")
#     message(FATAL_ERROR "${OPENCV_INCLUDE} arch is not supported!")
# endif()


################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "dll"
    "Python"
    "PythonDebug"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_DLL "")
    set(CMAKE_CXX_FLAGS_PYTHON "")
    set(CMAKE_CXX_FLAGS_PYTHONDEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_TEST "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DLL "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DLL "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DLL "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DLL "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DLLCVLESS "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DLLCVLESS "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DLLCVLESS "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DLLCVLESS "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_PYTHON37 "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_PYTHON37 "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_PYTHON37 "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_PYTHON37 "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_PYTHON38 "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_PYTHON38 "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_PYTHON38 "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_PYTHON38 "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_PYTHONDEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_PYTHONDEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_PYTHONDEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_PYTHONDEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_STL "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_STL "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_STL "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_STL "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_STLCVLESS "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_STLCVLESS "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_STLCVLESS "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_STLCVLESS "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_TEST "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_TEST "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_TEST "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_TEST "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PROJECT_NAME Barlib)

################################################################################
# Source groups
################################################################################
# set(no_group_source_files
#    "source/Source.cpp"
# )
source_group("" FILES ${no_group_source_files})

set(classes
    "include/barclasses.h"
    "include/barImg.h"
    "include/barline.h"
    "include/barscalar.h"
    "include/barstrucs.h"
    "source/barclasses.cpp"
)
source_group("classes" FILES ${classes})

set(include
    "extra/include_py.h"
    "include/presets.h"
)
source_group("include" FILES ${include})

set(main
    "include/barcodeCreator.h"
    "include/component.h"
    "include/hole.h"
    "source/barcodeCreator.cpp"
    "source/component.cpp"
    "source/hole.cpp"
)
source_group("main" FILES ${main})

set(pylib
    "cpp.hint"
    "main.def"
    "extra/Barpy.cpp"
)
source_group("pylib" FILES ${pylib})

set(ALL_FILES
    ${no_group_source_files}
    ${classes}
    ${include}
    ${main}
    ${pylib}
)

################################################################################
# Target
################################################################################
# add_executable(${PROJECT_NAME} ${ALL_FILES})
add_library(${PROJECT_NAME} MODULE ${ALL_FILES})

set(ROOT_NAMESPACE Barcode)

set_target_properties(${PROJECT_NAME} PROPERTIES
    COMMON_LANGUAGE_RUNTIME_DLLCVLESS   "No"
    COMMON_LANGUAGE_RUNTIME_DLL         "No"
    COMMON_LANGUAGE_RUNTIME_PYTHON    "No"
    COMMON_LANGUAGE_RUNTIME_PYTHONDEBUG "No"
)
################################################################################
# Target name
################################################################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DLL         "barcode"
    TARGET_NAME_PYTHON      "barpy"
    TARGET_NAME_PYTHONDEBUG "barpy"
    TARGET_NAME_TEST        "barcode"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DLL         "${CMAKE_SOURCE_DIR}/build/ddl/"
    OUTPUT_DIRECTORY_PYTHON37    "${CMAKE_SOURCE_DIR}/build/Python/"
    OUTPUT_DIRECTORY_PYTHONDEBUG "${CMAKE_SOURCE_DIR}/build/PythonDebug/"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_DLL         "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_PYTHON      "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_PYTHONDEBUG "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASE     "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(CMAKE_CONFIGURATION_TYPES MATCHES "Python" OR CMAKE_CONFIGURATION_TYPES MATCHES "PythonDebug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE "_PYD")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/modules/pybind11/include)



if(CMAKE_CONFIGURATION_TYPES STREQUAL "Debug")
    set(OUT_FOLTE "Debug")

    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
else()
    set(OUT_FOLTE "Release")

    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wall -DNDEBUG -g)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Build/${CMAKE_CONFIGURATION_TYPES}/"
)
