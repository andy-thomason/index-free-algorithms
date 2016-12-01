cmake_minimum_required (VERSION 3.1.0)


project(index-free-algorithms)
add_executable(array_inversion array_inversion.cpp)
set_property(TARGET array_inversion PROPERTY CXX_STANDARD 11)
