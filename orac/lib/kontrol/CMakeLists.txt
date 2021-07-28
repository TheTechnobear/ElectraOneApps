###############################
# MEC library
project(mec-kontrol-api)

set(KONTROL_API_SRC
        Entity.cpp
        Rack.cpp
        Module.cpp
        Parameter.cpp
        ParamValue.cpp
        KontrolModel.cpp
        ChangeSource.cpp
        )


add_library(mec-kontrol-api STATIC ${KONTROL_API_SRC})

target_include_directories(mec-kontrol-api PUBLIC .)

