find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_standard_project_setup()

add_subdirectory(TimeFlipApiClient)

set(RESOURCES
    resources/icons.qrc
)

set(HEADERS
    Config.h
    Configuration.h
    Summary.h
    TrayApplication.h
)

set(SOURCES
    Config.cpp
    Configuration.cpp
    main.cpp
    Summary.cpp
    TrayApplication.cpp
)

set(FORMS
    Configuration.ui
    Summary.ui
)

qt_add_executable(TimeFlipAgent
    WIN32 MACOSX_BUNDLE
    ${HEADERS}
    ${SOURCES}
    ${FORMS}
    ${RESOURCES}
)

target_link_libraries(TimeFlipAgent
    PRIVATE
        TimeFlipApiClient
        Qt6::Core
        Qt6::Widgets
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
