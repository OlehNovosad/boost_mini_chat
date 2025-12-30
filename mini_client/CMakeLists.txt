cmake_minimum_required(VERSION 3.16)
project(mini_server)

set(CMAKE_CXX_STANDARD 20)

# Use modern CMake variable naming
set(Boost_ROOT /opt/homebrew/opt/boost)

# Find Boost - system is now header-only, so don't request it as a component
find_package(Boost 1.90.0 REQUIRED COMPONENTS thread chrono)

if (Boost_FOUND)
    add_executable(mini_server main.cpp)
    target_link_libraries(mini_server
            Boost::thread
            Boost::chrono
    )
endif ()