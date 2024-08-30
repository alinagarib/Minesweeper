cmake_minimum_required(VERSION 3.24)
project(untitled2)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled2 main.cpp
        main.cpp
        TextureManager.h
        TextureManager.cpp
        welcome.h
        welcome.cpp
        test.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(c:/SFML/include/SFML)
target_link_libraries(untitled2 sfml-system sfml-window sfml-graphics sfml-audio)
