cmake_minimum_required(VERSION 3.6)

# Qt / CMake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.12 COMPONENTS Core REQUIRED)

add_library(coffeemaker STATIC EXCLUDE_FROM_ALL
  src/coffeemaker.cc  include/coffeemaker/coffeemaker.h
)

target_link_libraries(coffeemaker PUBLIC Qt5::Core)

target_include_directories(coffeemaker
  PRIVATE
    "include/coffeemaker"
  INTERFACE
    "include"
)