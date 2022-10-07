set(PROJECT_NAME BarcodeTests)

################################################################################
# Source groups
################################################################################
set(_
    "BarcodeTests.cpp"
)
source_group("Исходные файлы" FILES ${_})

set(ALL_FILES
    ${_}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE BarcodeTests)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE     "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE     "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE     "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{VCInstallDir}UnitTest/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{VCInstallDir}UnitTest/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{VCInstallDir}UnitTest/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:dllcvless>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:dll>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Python37>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Python38>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:PythonDebug>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "WIN32;"
            "NDEBUG"
        ">"
        "$<$<CONFIG:stlcvless>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:stl>:"
            "WIN32;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Test>:"
            "_DEBUG"
        ">"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:dllcvless>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:dll>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Python37>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Python38>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:PythonDebug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:stlcvless>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:stl>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Test>:"
            "_DEBUG"
        ">"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:dllcvless>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:dll>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Python37>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Python38>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:PythonDebug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:stlcvless>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:stl>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Test>:"
            "_DEBUG"
        ">"
        "WIN32;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:dllcvless>:
                /INCREMENTAL
            >
            $<$<CONFIG:dll>:
                /INCREMENTAL
            >
            $<$<CONFIG:Python37>:
                /INCREMENTAL
            >
            $<$<CONFIG:Python38>:
                /INCREMENTAL
            >
            $<$<CONFIG:PythonDebug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:stlcvless>:
                /INCREMENTAL
            >
            $<$<CONFIG:stl>:
                /INCREMENTAL
            >
            $<$<CONFIG:Test>:
                /INCREMENTAL
            >
            /DEBUG:FULL;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:dllcvless>:
                /INCREMENTAL
            >
            $<$<CONFIG:dll>:
                /INCREMENTAL
            >
            $<$<CONFIG:Python37>:
                /INCREMENTAL
            >
            $<$<CONFIG:Python38>:
                /INCREMENTAL
            >
            $<$<CONFIG:PythonDebug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:stlcvless>:
                /INCREMENTAL
            >
            $<$<CONFIG:stl>:
                /INCREMENTAL
            >
            $<$<CONFIG:Test>:
                /INCREMENTAL
            >
            /DEBUG:FULL;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:dllcvless>:
                /INCREMENTAL
            >
            $<$<CONFIG:dll>:
                /INCREMENTAL
            >
            $<$<CONFIG:Python37>:
                /INCREMENTAL
            >
            $<$<CONFIG:Python38>:
                /INCREMENTAL
            >
            $<$<CONFIG:PythonDebug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:stlcvless>:
                /INCREMENTAL
            >
            $<$<CONFIG:stl>:
                /INCREMENTAL
            >
            $<$<CONFIG:Test>:
                /INCREMENTAL
            >
            /DEBUG:FULL;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    Barcode
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Test>:"
            "barcode"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "barcode"
        ">"
        "$<$<CONFIG:dllcvless>:"
            "barcode"
        ">"
        "$<$<CONFIG:dll>:"
            "barcode"
        ">"
        "$<$<CONFIG:Python37>:"
            "barcode"
        ">"
        "$<$<CONFIG:Python38>:"
            "barcode.obj"
        ">"
        "$<$<CONFIG:PythonDebug>:"
            "barcode"
        ">"
        "$<$<CONFIG:stlcvless>:"
            "barcode.obj"
        ">"
        "$<$<CONFIG:stl>:"
            "barcode"
        ">"
        "$<$<CONFIG:Test>:"
            "barcode"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Test>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$ENV{VCInstallDir}UnitTest/lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$<$<CONFIG:dllcvless>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$<$<CONFIG:dll>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$<$<CONFIG:Python37>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$<$<CONFIG:Python38>:"
            "D:/Programs/C++/Barcode/x64/Debug/"
        ">"
        "$<$<CONFIG:PythonDebug>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$<$<CONFIG:stlcvless>:"
            "D:/Programs/C++/Barcode/x64/Debug/"
        ">"
        "$<$<CONFIG:stl>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$<$<CONFIG:Test>:"
            "D:/Programs/C++/Barcode/x64/dll opencv less"
        ">"
        "$ENV{VCInstallDir}UnitTest/lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$ENV{VCInstallDir}UnitTest/lib"
    )
endif()

