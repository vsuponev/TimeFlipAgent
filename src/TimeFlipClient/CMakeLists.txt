find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network)

set(HEADERS
    Credentials.h
    Error.h
    ResponseResult.h
    TimeFlipApiClient.h
    UserInfo.h
)

set(SOURCES
    TimeFlipApiClient.cpp
)

add_library(TimeFlipApiClient STATIC
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(TimeFlipApiClient
    PUBLIC
        Qt6::Core
        Qt6::Network
)

target_include_directories(TimeFlipApiClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
