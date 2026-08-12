cmake_minimum_required(VERSION 3.22.1)

project("simplecube")

add_library(
        native-lib
        SHARED
        native-lib.cpp
        Matrix.cpp
       Texture.cpp
)

find_library(
        log-lib
        log
)

target_link_libraries(
        native-lib
        ${log-lib}
        GLESv2
)