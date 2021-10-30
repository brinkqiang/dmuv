cmake_minimum_required(VERSION 2.8)

PROJECT(dmuv)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

ADD_SUBDIRECTORY(thirdparty/libuv)

ADD_DEFINITIONS(-DUSE_UVPP)
ADD_DEFINITIONS(-DNOMINMAX)

ExeImport("test" "uv_a")
