cmake_minimum_required(VERSION 3.13)
project(neither)

set(CMAKE_CXX_STANDARD 14)

include_directories(neither/include)

add_executable(neither
        neither/include/either.hpp
        neither/include/lift.hpp
        neither/include/maybe.hpp
        neither/include/neither.hpp
        neither/include/traits.hpp
        neither/include/try.hpp
        neither/include/writer.hpp)
