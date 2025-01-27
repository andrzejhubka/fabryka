cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
# Nazwa projektu
project(fabryka)

# Ustawienie katalogu wyjściowego dla plików wykonywalnych
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dodaj katalog z plikami nagłówkowymi
include_directories(include)


add_executable(factory src/factory.cpp src/utilities.cpp src/supplier.cpp src/warehouse.cpp src/machine.cpp src/director.cpp)

set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
