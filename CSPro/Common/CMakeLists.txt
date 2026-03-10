# Common/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(Common VERSION 0.1 LANGUAGES CXX)

# Qt 自动工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)  # 重要！

# **关键：查找 Qt 包**
# 首先查找 Qt6 或 Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets NetWork)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets NetWork)

# 公共工具源文件
set(COMMON_SOURCES
    qsshelper.cpp
)

set(COMMON_HEADERS
    qsshelper.h
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    commstruct.h
)

# **关键：链接 Qt 库**
# 使用 PUBLIC 因为头文件中包含了 QObject
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# **关键：设置包含目录**
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 输出调试信息
message(STATUS "Common configured with Qt${QT_VERSION_MAJOR}")
message(STATUS "Common include dir: ${CMAKE_CURRENT_SOURCE_DIR}")
