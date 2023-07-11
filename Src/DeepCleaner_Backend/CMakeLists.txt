cmake_minimum_required(VERSION 3.21)
project(DeepCleaner_Backend)

set(CMAKE_CXX_STANDARD 20)

add_executable(DeepCleaner_Backend main.cpp
particle.cpp particle.h
robot.cpp robot.h
state.cpp state.h
timeline.cpp timeline.h json.hpp position.cpp position.h utils.cpp utils.h
        trajectory.cpp trajectory.h
)
