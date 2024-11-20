# cmake version
cmake_minimum_required(VERSION 3.22.1)
# project name
project(test)

# set c++ version
set(CMAKE_CXX_STANDARD 17)

# include dir add,split by<space>
include_directories(../third_party/cpptest)

# link dir add
# link_directories()
# link_libraries(-lpthread)

# add source
add_executable(test ./main.cpp)
