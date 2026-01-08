
cmake_minimum_required(VERSION 3.10)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(VeDirectFrameHandler VERSION 1.0.1 DESCRIPTION "VE.Direct FrameHandler")

SET(VEDIRECT_BUILD_STATIC FALSE CACHE BOOL "Build static library")

IF (VEDIRECT_BUILD_STATIC)
    add_library(VeDirectFrameHandler STATIC VeDirectFrameHandler.cpp)
ELSE()    
    add_library(VeDirectFrameHandler SHARED VeDirectFrameHandler.cpp)
ENDIF()

set_target_properties(VeDirectFrameHandler PROPERTIES PUBLIC_HEADER VeDirectFrameHandler.h)
