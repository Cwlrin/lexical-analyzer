cmake_minimum_required(VERSION 3.28)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} C)

set(CMAKE_C_STANDARD 17)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CompilerIdC/CMakeCCompilerId.c")

list(APPEND SOURCE_FILES ${HEADER_FILES})

add_executable(main ${SOURCE_FILES})