cmake_minimum_required(VERSION 3.10)

# Set up vcpkg toolchain if VCPKG_ROOT is defined
if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(BoostExample)
set(CMAKE_CXX_STANDARD 17)
find_package(Boost CONFIG REQUIRED)

add_definitions(-D_GNU_SOURCE)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE Boost::boost)
