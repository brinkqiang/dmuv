cmake_minimum_required(VERSION 3.8)

PROJECT(dmuv)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

ADD_SUBDIRECTORY(thirdparty/libuv)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
ADD_DEFINITIONS(-DUSE_UVPP)
ADD_DEFINITIONS(-DNOMINMAX)
set( CMAKE_CXX_STANDARD 17)
ADD_DEFINITIONS(-DASIO_STANDALONE)

ModuleInclude("asio" "thirdparty/asio")

ModuleImport("libuv" "thirdparty/libuv")

LibImportDepends("uvcpp" "src/uvcpp" "uv_a")

ExeImport("test" "uv_a")
