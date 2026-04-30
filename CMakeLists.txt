cmake_minimum_required(VERSION 3.16)

# ===== vcpkg 工具链配置 =====
# 必须在 project() 之前设置
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    # 尝试从环境变量 VCPKG_ROOT 获取路径
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_ROOT $ENV{VCPKG_ROOT})
    else()
        message(FATAL_ERROR "Please set VCPKG_ROOT environment variable:\n  export VCPKG_ROOT=~/Documents/Code/vcpkg\n\nSee SETUP.md for details.")
    endif()

    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain")
    message(STATUS "Using vcpkg: ${VCPKG_ROOT}")
endif()

project(RoujiamoLegend VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== 依赖 =====

# Raylib (通过 vcpkg 安装)
find_package(raylib CONFIG REQUIRED)

# 线程支持
find_package(Threads REQUIRED)

# ===== 源代码 =====
set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ===== 链接库 =====

# 获取 vcpkg installed 目录 - 使用绝对路径展开 ~
if(DEFINED ENV{VCPKG_ROOT})
    file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}" VCPKG_ROOT_ABS)
    set(VCPKG_INSTALL_DIR "${VCPKG_ROOT_ABS}/installed/arm64-osx")
else()
    # 默认路径
    set(VCPKG_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/arm64-osx")
endif()

message(STATUS "VCPKG_INSTALL_DIR: ${VCPKG_INSTALL_DIR}")

# Link Raylib and its dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    "${VCPKG_INSTALL_DIR}/lib/libglfw3.a"
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    Threads::Threads
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Copy assets directory to build (if exists)
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
endif()
