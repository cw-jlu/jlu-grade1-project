cmake_minimum_required(VERSION 3.10.0)
project(final VERSION 0.1.0 LANGUAGES C CXX)

add_executable(final)
target_sources(final PRIVATE 最终整合版.c)
target_include_directories(final PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
