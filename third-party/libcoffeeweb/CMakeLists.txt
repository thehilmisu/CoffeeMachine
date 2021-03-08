cmake_minimum_required(VERSION 3.6)

# Qt / CMake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.12 COMPONENTS Core REQUIRED)

add_library(coffeeweb STATIC EXCLUDE_FROM_ALL
  src/coffeeweb.cc  include/coffeeweb/coffeeweb.h
  src/json.qrc
)

target_link_libraries(coffeeweb PUBLIC Qt5::Core)

target_include_directories(coffeeweb
  PRIVATE
    "include/coffeeweb"
  INTERFACE
    "include"
)