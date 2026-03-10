cmake_minimum_required(VERSION 3.16)
project(CSPro VERSION 0.1 LANGUAGES CXX)

# Qt 自动工具
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Network Xml Charts Sql Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network Xml Charts Sql Quick)

set(TS_FILES CSPro_zh_CN.ts)

set(PROJECT_SOURCES
    main.cpp
    csproject.cpp
    csproject.h
    csproject.ui
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CSPro
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        server_main.cpp
        client.h client.cpp
        resources.qrc
        serverwidget.h serverwidget.cpp serverwidget.ui
        serversetting.h serversetting.cpp serversetting.ui
        dashboardwidget.h dashboardwidget.cpp dashboardwidget.ui
        systemmonitor.h systemmonitor.cpp
        sqlmanager.h sqlmanager.cpp
        usermanagerwidget.h usermanagerwidget.cpp usermanagerwidget.ui
        FloatingWindow.qml
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(CSPro SHARED ${PROJECT_SOURCES})
    else()
        add_executable(CSPro ${PROJECT_SOURCES})
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# 添加子目录（注意顺序：Common 最先，因为它可能被其他库依赖）
add_subdirectory(Common)
add_subdirectory(NetManager)
add_subdirectory(XmlManager)
add_subdirectory(FlotingWin)

# 设置全局包含路径（可选，但建议用 target_include_directories 传播）
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NetManager
    ${CMAKE_CURRENT_SOURCE_DIR}/XmlManager
)

# 链接库
target_link_libraries(CSPro PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Quick
    NetManager
    XmlManager
    Common
    FlotingWin
)

if(WIN32)
    target_link_libraries(CSPro PRIVATE Pdh.lib)
endif()

# iOS/macOS 设置
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CSPro)
endif()
set_target_properties(CSPro PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CSPro
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CSPro)
endif()
