cmake_minimum_required(VERSION 3.16)
project(Dinosaur)

add_executable(dino 
    src/main.cpp
    src/player.cpp
    src/obstacle.cpp
    src/game.cpp
)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
target_link_libraries(dino sfml-graphics sfml-audio)

target_include_directories(dino
    PRIVATE include
)
