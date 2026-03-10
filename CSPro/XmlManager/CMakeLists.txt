cmake_minimum_required(VERSION 3.16)

# 项目名称，可以保持不变或改得更清晰
project(XmlManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)  # 如果使用 Qt 的元对象系统
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt 组件（xmlmanager 需要 Qt 的 XML 模块）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Xml)

# 定义库的源文件（注意：没有 main.cpp！）
set(XMLMANAGER_SOURCES
    xmlmanager.cpp
    # 如果有其他相关的源文件，也加在这里
)

set(XMLMANAGER_HEADERS
    xmlmanager.h
    # 如果有其他相关的头文件，也加在这里
)

# 创建静态库（或者共享库，根据需求选择）
# STATIC: 静态库，编译到主程序中
# SHARED: 动态库，单独的文件
add_library(${PROJECT_NAME} STATIC
    ${XMLMANAGER_SOURCES}
    ${XMLMANAGER_HEADERS}
)

# 链接必要的 Qt 库
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Xml
)

# 设置包含目录，这样其他项目引用时可以找到头文件
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 可选：如果需要导出符号（如果使用共享库）
# target_compile_definitions(${PROJECT_NAME} PRIVATE XMLMANAGER_LIBRARY)

# 安装规则（可选）
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 静态库需要这个
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # 仅用于动态库的 .dll 文件
)

# 如果有测试代码，可以这样添加（可选）
option(BUILD_XMLMANAGER_TESTS "Build XmlManager tests" OFF)

if(BUILD_XMLMANAGER_TESTS)
    add_executable(${PROJECT_NAME}_test main.cpp)
    target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME})
    target_include_directories(${PROJECT_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
