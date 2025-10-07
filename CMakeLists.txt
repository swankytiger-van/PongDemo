set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
cmake_minimum_required(VERSION 3.20)
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
project(PongDemo)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(main ${SOURCES} "src/Paddle.cpp" "src/Paddle.h" "src/Ball.h" "src/Ball.cpp" "src/AiPaddle.h" "src/AiPaddle.cpp")

# SFML 3.0 组件名大写
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System)
