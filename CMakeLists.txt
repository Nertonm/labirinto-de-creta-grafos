cmake_minimum_required(VERSION 3.15)

project(LabirintoDeCreta CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")


add_library(labirinto_lib
    src/Grafo.cpp
    src/Prisioneiro.cpp
    src/Minotauro.cpp
    src/Simulador.cpp
)

target_include_directories(labirinto_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(simulador src/main.cpp)

target_link_libraries(simulador PRIVATE labirinto_lib)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

message(STATUS "Projeto configurado. Para compilar, use 'make' (ou seu build tool).")