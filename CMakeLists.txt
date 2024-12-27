cmake_minimum_required(VERSION 3.10)

# Nazwa projektu
project(fabryka)

# Ustawienie katalogu wyjściowego dla plików wykonywalnych
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dodaj katalog z plikami nagłówkowymi
include_directories(include)

# Dodanie pliku wykonywalnego dla aplikacji director
add_executable(director src/director.cpp)

# Można dodać inne aplikacje (np. producer), analogicznie:
add_executable(supplier src/supplier.cpp)
add_executable(utilities src/utilities.cpp)
add_executable(warehouse src/warehouse.cpp)
add_executable(factory src/factory.cpp)

set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
