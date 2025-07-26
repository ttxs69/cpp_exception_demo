cmake_minimum_required(VERSION 3.10)

# Set up vcpkg toolchain if VCPKG_ROOT is defined
if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(BoostExample)
set(CMAKE_CXX_STANDARD 17)

# Find the Boost libraries, including the necessary stacktrace component
find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)

add_definitions(-D_GNU_SOURCE)

add_executable(my_app main.cpp)

# Add -g flag for debug builds to get source info in stack traces
target_compile_options(my_app PRIVATE $<$<CONFIG:Debug>:-g>)

# Link to the Boost stacktrace library and libdl for symbol resolution
target_link_libraries(my_app PRIVATE Boost::stacktrace_backtrace dl)
target_link_options(my_app PRIVATE -Wl,-dead_strip_dylibs)
