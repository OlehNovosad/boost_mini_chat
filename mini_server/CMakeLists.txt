cmake_minimum_required(VERSION 3.16)
project(mini_server)

set(CMAKE_CXX_STANDARD 20)

# Point CMake to Homebrew Boost
set(BOOST_ROOT /opt/homebrew/opt/boost)
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

find_package(Boost REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    # Add the executable target
    add_executable(mini_server main.cpp)

    # Add compile definitions to the correct target
    target_compile_definitions(mini_server
            PRIVATE ASYNC_SERVER
    )

    # Link Boost libraries
    target_link_libraries(mini_server ${Boost_LIBRARIES})
endif ()
