cmake_minimum_required(VERSION 3.10)

# 项目信息
project(motor_test)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/Inc)

# 查找源文件
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/Src/*.cpp")

# 添加可执行文件
add_executable(motor_test ${SOURCES})

# 链接库文件
target_link_libraries(motor_test pthread
    ${PROJECT_SOURCE_DIR}/lib/libECanVci.so
    ${PROJECT_SOURCE_DIR}/lib/libusb.so)
