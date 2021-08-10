project(orac_e1)

set(FW_DIR ${PROJECT_SOURCE_DIR}/../../external/firmware)

include_directories(
        "${PROJECT_SOURCE_DIR}/../lib/kontrol"
        "${FW_DIR}/src"
        "${FW_DIR}/src/Windows"
        "${FW_DIR}/src/Components"
        "${FW_DIR}/lib/SdFat/src"
        "${FW_DIR}/lib/RA8876"
)

file(GLOB_RECURSE ORAC_E1_SRC RELATIVE "${PROJECT_SOURCE_DIR}" "*.cpp")
file(GLOB_RECURSE ORAC_E1_H RELATIVE "${PROJECT_SOURCE_DIR}" "*.h")

message(STATUS "INC :${FW_DIR}/src")
add_library(orac_e1 STATIC ${ORAC_E1_SRC} ${ORAC_E1_H})
