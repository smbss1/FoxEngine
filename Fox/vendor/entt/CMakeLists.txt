cmake_minimum_required(VERSION 3.8)

add_library(entt INTERFACE include/entt.hpp)
target_include_directories(entt INTERFACE include)
