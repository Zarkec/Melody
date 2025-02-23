cmake_minimum_required(VERSION 3.5)

project(Melody VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Network Multimedia Sql Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Network Multimedia Sql Widgets REQUIRED)

set(SOURCES
    listwidgetlocal.cpp
    main.cpp
    musiceditdialog.cpp
    showitem.cpp
    showtableitem.cpp
    usemysql.cpp
    usenetwork.cpp
    usesqlite.cpp
    widget.cpp
)

set(HEADERS
    listwidgetlocal.h
    music.h
    musiceditdialog.h
    showitem.h
    showtableitem.h
    usemysql.h
    usenetwork.h
    usesqlite.h
    widget.h
)

set(FORMS
    musiceditdialog.ui
    showitem.ui
    showtableitem.ui
    widget.ui
)

set(RESOURCES
    res.qrc
)

qt5_wrap_ui(UI_HEADERS ${FORMS})
qt5_add_resources(RESOURCES_RCC ${RESOURCES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Melody
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
        ${UI_HEADERS}
        ${RESOURCES_RCC}
    )
else()
    if(ANDROID)
        add_library(Melody SHARED
            ${SOURCES}
            ${HEADERS}
            ${UI_HEADERS}
            ${RESOURCES_RCC}
        )
    else()
        add_executable(Melody
            ${SOURCES}
            ${HEADERS}
            ${UI_HEADERS}
            ${RESOURCES_RCC}
        )
    endif()
endif()

target_link_libraries(Melody PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(Melody PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Melody)
endif()

# 设置安装路径
if(QNX)
    set(INSTALL_PATH "/tmp/${PROJECT_NAME}/bin")
elseif(UNIX AND NOT ANDROID)
    set(INSTALL_PATH "/opt/${PROJECT_NAME}/bin")
endif()

if(INSTALL_PATH)
    install(TARGETS Melody DESTINATION ${INSTALL_PATH})
endif()
