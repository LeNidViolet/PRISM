CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
PROJECT(prismui)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
#SET(CMAKE_AUTOUIC ON)

SET(CMAKE_PREFIX_PATH $ENV{HOME}/Qt5.14.2/5.14.2/clang_64)

SET(PRISMUI_ADDTION_HDR
        ${SS_DIR}/include
        )
INCLUDE_DIRECTORIES(${PRISMUI_ADDTION_HDR})

FIND_PACKAGE(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED
        )

SET(PRISMUI_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_status.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_statistics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_hosts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ss_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/io_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/func.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/searchable_treeview.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/searchable_treeview_model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/searchable_treeWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/custom/http_server.cpp
        )

ADD_EXECUTABLE(prism ${PRISMUI_SRC})

TARGET_LINK_LIBRARIES(prism
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        shadowsocks-crypto
        mbedtls
        uv_a
        )

