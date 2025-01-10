find_package(Qt6 6.5 REQUIRED COMPONENTS Core Bluetooth)

set(HEADERS
    TimeFlipBTClient.h
)

set(SOURCES
    TimeFlipBTClient.cpp
)

add_library(TimeFlipBTClient STATIC
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(TimeFlipBTClient
    PUBLIC
        Qt6::Core
        Qt6::Bluetooth
)

target_include_directories(TimeFlipBTClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
