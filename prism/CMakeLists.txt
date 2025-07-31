# Minimum required version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
PROJECT(PRISMUI LANGUAGES CXX)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
#SET(CMAKE_AUTOUIC ON)


FIND_PACKAGE(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        Concurrent
        )

# Check if Qt6 was found
IF(NOT Qt6_FOUND)
    MESSAGE(FATAL_ERROR "Qt6 not found. Please install Qt6 and set the CMAKE_PREFIX_PATH to its installation directory.")
ENDIF(NOT Qt6_FOUND)


SET(PRISMUI_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_mainwgt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_statistics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_hosts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_flow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dump.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hosts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/flow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/if_raw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/http_server.cpp
)

ADD_EXECUTABLE(PRISMUI ${PRISMUI_SRC})

TARGET_COMPILE_FEATURES(PRISMUI
        PRIVATE
        cxx_std_17
)


TARGET_LINK_LIBRARIES(PRISMUI
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Concurrent
        CommonWidgets
        socks5-crypto
        )

# Additional include directories
TARGET_INCLUDE_DIRECTORIES(PRISMUI
        PRIVATE
        ${COMMON_DIR}/lib/include
)

IF(WIN32)
    SET_TARGET_PROPERTIES(PRISMUI PROPERTIES WIN32_EXECUTABLE TRUE)
ENDIF()
IF(APPLE)
    SET_TARGET_PROPERTIES(PRISMUI PROPERTIES MACOSX_BUNDLE TRUE)
ENDIF()
