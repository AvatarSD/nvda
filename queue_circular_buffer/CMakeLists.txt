cmake_minimum_required(VERSION 3.10 )
project(QueueCircularBuffer)


# Set C++ standard
set(CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)  # For gnu++23

# Define the executable
add_executable(${PROJECT_NAME} main.cpp)