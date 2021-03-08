cmake_minimum_required(VERSION 3.6)

# Use QTDIR environment variable with find_package,
#  e.g. set QTDIR=/home/user/Qt/5.9.6/gcc_64/
if(NOT "$ENV{QTDIR}" STREQUAL "")
  set(QTDIR $ENV{QTDIR})
  list(APPEND CMAKE_PREFIX_PATH $ENV{QTDIR})
elseif(QTDIR)
  list(APPEND CMAKE_PREFIX_PATH ${QTDIR})
endif()

# Set the default build type to release
if( NOT CMAKE_BUILD_TYPE )
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# project(MyCoffeeMachine LANGUAGES CXX)
#Global compile options, since we only compile on and for Linux we skip the check for MSVC
# add_compile_options(-Wall -Wextra -Werror)
#=======
project(MyCoffeeMachine LANGUAGES CXX)
#>>>>>>> 7363821b629df287f4a6092f88b4688dd1032d1e

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt / CMake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.12 COMPONENTS Core Gui Quick Widgets REQUIRED)

add_subdirectory(third-party/libcoffeemaker)
add_subdirectory(third-party/libcoffeeweb)

add_executable(CoffeeMachine main.cc
  coffee_app.cc coffee_app.h
  qml/qml.qrc
)

target_link_libraries(CoffeeMachine
  PRIVATE
    Qt5::Core Qt5::Quick Qt5::Widgets
    coffeemaker coffeeweb
)

target_compile_options(CoffeeMachine
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Werror>
)
