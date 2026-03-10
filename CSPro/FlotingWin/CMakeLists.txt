cmake_minimum_required(VERSION 3.16)
project(FlotingWin VERSION 0.1 LANGUAGES CXX)


# 统一输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)   # 静态库
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)   # 动态库的导入库（.lib/.a）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   # 可执行文件和 .dll

# Qt 自动工具
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickWidgets)

qt_standard_project_setup(REQUIRES 6.8)

# 导出宏定义
add_definitions(-DFLOTINGWIN_LIBRARY)

# 库的源文件（只列 .cpp，头文件通过 target_include_directories 暴露）
set(SOURCES
    floatingwindow.cpp
)

# 创建动态库
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# 链接 Qt 模块
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickWidgets
)

# 包含目录（暴露头文件）
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置输出目录（方便主程序找到 .dll）
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加 QML 模块
qt_add_qml_module(${PROJECT_NAME}
    URI FlotingWin
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES floatingwindow_global.h
    SOURCES floatingwindow.h
)

# 安装
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
