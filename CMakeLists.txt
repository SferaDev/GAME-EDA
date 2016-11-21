cmake_minimum_required(VERSION 3.6)
project(GAME_EDA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(GAME_EDA ALL COMMAND make all WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )