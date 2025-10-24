cmake_minimum_required(VERSION 3.20)

add_library(image_processing STATIC 
    src/image_processor.cpp
)

target_include_directories(image_processing 
    PUBLIC 
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/stb
)

target_compile_features(image_processing PUBLIC cxx_std_23)
