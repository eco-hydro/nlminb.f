cmake_minimum_required(VERSION 3.2)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(nlminb VERSION 0.1.0)

# set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "g++")

find_package(BLAS)
# FindBLAS()
enable_language(Fortran)

add_library(nlminb SHARED #STATIC
    src/port2.c
    src/portsrc.f
    src/d1mach.f
)

target_include_directories(nlminb
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries (nlminb ${BLAS_LIBRARIES})

install(TARGETS nlminb DESTINATION lib)
