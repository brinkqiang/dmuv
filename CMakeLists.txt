cmake_minimum_required(VERSION 3.8)

PROJECT(dmuv)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_DEFINITIONS(-DUSE_UVPP)
ADD_DEFINITIONS(-DNOMINMAX)

ModuleImport("libuv" "thirdparty/libuv")

ExeImport("test" "uv_a")
