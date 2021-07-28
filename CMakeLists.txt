cmake_minimum_required(VERSION 3.0.0)
project(ORAC_E1)

message(STATUS "this is NOT intended to build !!! ")
message(STATUS "This is just being used with CLion to enable editing features")


if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif ()
endif ()

set(PROJECT_NAME ORAC_E1)
set(ORAC_E1_VERSION_MAJOR 0)
set(ORAC_E1_VERSION_MINOR 1)
set(ORAC_E1_VERSION_UPDATE 1)
set(ORAC_E1_VERSION "${ORAC_E1_VERSION_MAJOR}.${ORAC_E1_VERSION_MINOR}.${ORAC_E1_VERSION_UPDATE}")


set(CMAKE_CXX_STANDARD 11)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # here we will later do further checks for crosscompile environment
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^armv6l")
    endif ()
else ()
    message(STATUS "cross compilation required ")
endif ()


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/bin)

add_subdirectory(external firmware)

add_subdirectory(orac/lib/kontrol)
add_subdirectory(orac/src)
