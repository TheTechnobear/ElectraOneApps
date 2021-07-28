project(e1_firmware)

set(FW_DIR ${CMAKE_SOURCE_DIR}/firmware)

include_directories(
        "${FW_DIR}/src"
        "${FW_DIR}/src/Windows"
        "${FW_DIR}/src/Components"
        "${FW_DIR}/lib/SdFat/src"
        "${FW_DIR}/lib/RA8876"
)

file(GLOB_RECURSE E1_SRC_C RELATIVE ${CMAKE_SOURCE_DIR}/firmware "*.c")
file(GLOB_RECURSE E1_SRC_CPP RELATIVE ${CMAKE_SOURCE_DIR}/firmware "*.cpp")




message(STATUS "FW: ${CMAKE_SOURCE_DIR}/firmware ${E1_SRC_CPP}" )
add_library(e1_firmware STATIC ${E1_SRC_C} ${E1_SRC_CPP})





target_include_directories(e1_firmware PUBLIC firmware/src)