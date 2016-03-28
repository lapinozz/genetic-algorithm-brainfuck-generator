#cmake ver req
cmake_minimum_required(VERSION 3.1.0)

#project name
set(project_name ai_code_generator)
project(${project_name} C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -Wall -Wextra -Wshadow -Wconversion -Wno-long-long -pedantic -Wno-unused-parameter")

include_directories("src/interpretor" "src/GeneticAlgorithm" "src/ThreadPool" "src/RandomGenerator")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package ( Threads REQUIRED)

file(GLOB_RECURSE source_files "src/*.cpp")
file(GLOB_RECURSE header_files "src/*.hpp")
add_executable(${project_name} ${header_files} ${source_files})
target_link_libraries(${project_name} Threads::Threads)
