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
        )

# Check if Qt6 was found
IF(NOT Qt6_FOUND)
    MESSAGE(FATAL_ERROR "Qt6 not found. Please install Qt6 and set the CMAKE_PREFIX_PATH to its installation directory.")
ENDIF(NOT Qt6_FOUND)


SET(PRISMUI_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_mainwgt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_status.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_statistics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_hosts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ss_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/searchable_treeview.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/searchable_treeview_model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/searchable_treeWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/http_server.cpp
)

ADD_EXECUTABLE(PRISMUI ${PRISMUI_SRC})

TARGET_LINK_LIBRARIES(PRISMUI
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        shadowsocks-crypto
        )

