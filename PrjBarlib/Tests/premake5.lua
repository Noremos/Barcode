-- premake5.lua
workspace "Barcode Research"
   configurations { "Debug", "Release" }

   filter { "configurations:Debug" }
      defines { "DEBUG" }

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"

   project "gtest_main"
      language "C++"
      kind "StaticLib"
      targetdir "Build"

      includedirs {
         "../../../../vcpkg/packages/gtest_arm64-osx",
         "../../../../vcpkg/packages/gtest_arm64-osx/include"
      }

      files { "../../../../vcpkg/packages/gtest_arm64-osx/src/*.cc", "/../../../../vcpkg/packages/gtest_arm64-osx/src/*.h" }
      cppdialect "C++20"

   project "BarTests"
      kind "ConsoleApp"
      language "C++"
      targetdir "Build"

      includedirs {
         "../include",
         "../../../../vcpkg/packages/gtest_arm64-osx/include"
      }

      files {
         "../source/*.cpp",
         "src/BarcodeTests.cpp",
         "src/TiffTests.cpp"
      }

      links { "gtest_main"}
      cppdialect "C++20"
