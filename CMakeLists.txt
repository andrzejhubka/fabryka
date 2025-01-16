cmake_minimum_required(VERSION 3.10)

# Nazwa projektu
project(fabryka)

# Ustawienie katalogu wyjściowego dla plików wykonywalnych
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dodaj katalog z plikami nagłówkowymi
include_directories(include)


# Można dodać inne aplikacje (np. producer), analogicznie:
add_executable(supplier src/supplier.cpp src/utilities.cpp src/warehouse.cpp)
#add_executable(warehouse src/warehouse.cpp src/utilities.cpp)
add_executable(factory src/factory.cpp src/utilities.cpp src/warehouse.cpp)
add_executable(director src/director.cpp src/utilities.cpp)
add_executable(utils src/utilities.cpp)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
