cmake_minimum_required (VERSION 3.1.0)


project(index-free-algorithms)
add_executable(array_inversion array_inversion.cpp)
set_property(TARGET array_inversion PROPERTY CXX_STANDARD 11)

IF(WIN32)
ELSE(WIN32)
  find_package (Threads)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  target_link_libraries (array_inversion ${CMAKE_THREAD_LIBS_INIT})
ENDIF(WIN32)

