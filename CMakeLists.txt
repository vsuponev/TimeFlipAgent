cmake_minimum_required(VERSION 3.19)
project(TimeFlipAgent LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "TimeFlipAgent")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(RESOURCES resources/icons.qrc)

set(HEADERS src/Summary.h
            src/TrayApplication.h)

set(SOURCES src/main.cpp
            src/Summary.cpp
            src/TrayApplication.cpp)

set(FORMS src/Summary.ui)

qt_add_executable(TimeFlipAgent
    WIN32 MACOSX_BUNDLE
    ${HEADERS}
    ${SOURCES}
    ${FORMS}
    ${RESOURCES}
    src/Configuration.h src/Configuration.cpp src/Configuration.ui
    src/Config.h src/Config.cpp
)

target_link_libraries(TimeFlipAgent
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS TimeFlipAgent
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TimeFlipAgent
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
