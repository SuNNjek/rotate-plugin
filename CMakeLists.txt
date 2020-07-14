cmake_minimum_required(VERSION 3.16)

set(PluginName "rotate")

project(${PluginName})
set(CMAKE_CXX_STANDARD 17)

file(GLOB CODE "src/*.cpp")
add_library(${PluginName} SHARED ${CODE} main.cpp)

find_package(PkgConfig REQUIRED)

pkg_check_modules(AVISYNTH avisynth)
if(AVISYNTH_FOUND)
    include_directories(${AVISYNTH_INCLUDE_DIRS})
else()
    include_directories(include/avisynth)
endif()

include(GNUInstallDirs)
INSTALL(TARGETS ${PluginName}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/avisynth")
