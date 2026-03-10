# NetManager/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# 这个子项目的名称
project(NetManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt 依赖（网络模块）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

# 定义网络管理库的源文件
set(NETMANAGER_SOURCES
    server.cpp
    tcpserver.cpp
    udpserver.cpp
    connectionmanager.cpp
    connectionlog.cpp
    servermanager.cpp
)

set(NETMANAGER_HEADERS
    server.h
    tcpserver.h
    udpserver.h
    connectionmanager.h
    connectionlog.h
    servermanager.h
    common.h
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC
    ${NETMANAGER_SOURCES}
    ${NETMANAGER_HEADERS}
)

# 关键修改：使用 PUBLIC 或 PRIVATE 取决于是否需要传递依赖
# 如果 NetManager 的公共头文件（如 servermanager.h）中引用了 XmlManager 的内容
# 那么需要使用 PUBLIC，这样链接 NetManager 的项目会自动链接 XmlManager
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Network
    XmlManager  # 如果 XmlManager 是静态库，使用 PUBLIC 确保传递依赖
    Common
)

# 如果 NetManager 只在内部使用 XmlManager（.cpp 文件中使用），可以用 PRIVATE
# target_link_libraries(${PROJECT_NAME} PRIVATE
#     Qt${QT_VERSION_MAJOR}::Network
#     XmlManager
# )

# 设置包含目录
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/XmlManager
    ${CMAKE_SOURCE_DIR}/Common    # 这是头文件路径
)

# 可选：添加调试信息，查看链接是否正确
message(STATUS "NetManager will link against: Qt${QT_VERSION_MAJOR}::Network and XmlManager")
message(STATUS "XmlManager include path: ${CMAKE_SOURCE_DIR}/XmlManager")


# ===== 修复路径格式问题 =====

# 方法1：使用 file(REAL_PATH) 规范化路径
file(REAL_PATH "${CMAKE_SOURCE_DIR}/XmlManager" XMLMANAGER_REAL_PATH)
message(STATUS "XmlManager real path: ${XMLMANAGER_REAL_PATH}")

# 方法2：使用 cmake_path 命令（CMake 3.20+ 可用）
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
    cmake_path(NATIVE_PATH CMAKE_SOURCE_DIR CMAKE_SOURCE_DIR_NATIVE)
    cmake_path(CONVERT "${CMAKE_SOURCE_DIR}/XmlManager" TO_NATIVE_PATH_LIST XMLMANAGER_NATIVE_PATH NORMALIZE)
    message(STATUS "Native path: ${XMLMANAGER_NATIVE_PATH}")
endif()

# 方法3：使用 get_filename_component 规范化
get_filename_component(XMLMANAGER_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/XmlManager" ABSOLUTE)
get_filename_component(XMLMANAGER_CANONICAL_PATH "${CMAKE_SOURCE_DIR}/XmlManager" REALPATH)

message(STATUS "Absolute path: ${XMLMANAGER_ABSOLUTE_PATH}")
message(STATUS "Canonical path: ${XMLMANAGER_CANONICAL_PATH}")

# 设置包含目录 - 使用规范化的路径
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${XMLMANAGER_ABSOLUTE_PATH}  # 使用绝对路径
)

# 检查路径是否存在（使用规范化后的路径）
if(EXISTS "${XMLMANAGER_ABSOLUTE_PATH}")
    message(STATUS "✅ Path exists: ${XMLMANAGER_ABSOLUTE_PATH}")

    # 检查头文件
    set(XMLMANAGER_HEADER "${XMLMANAGER_ABSOLUTE_PATH}/xmlmanager.h")
    if(EXISTS "${XMLMANAGER_HEADER}")
        message(STATUS "✅ Header found: ${XMLMANAGER_HEADER}")
    else()
        message(WARNING "❌ Header not found: ${XMLMANAGER_HEADER}")
        # 列出目录内容
        file(GLOB XML_FILES "${XMLMANAGER_ABSOLUTE_PATH}/*")
        message(STATUS "Files in directory: ${XML_FILES}")
    endif()
else()
    message(WARNING "❌ Path does NOT exist: ${XMLMANAGER_ABSOLUTE_PATH}")
endif()
