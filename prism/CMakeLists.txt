CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
PROJECT(PRISMUI)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
#SET(CMAKE_AUTOUIC ON)


SET(PRISMUI_ADDTION_HDR
        ${SS_DIR}/include
        )
INCLUDE_DIRECTORIES(${PRISMUI_ADDTION_HDR})

FIND_PACKAGE(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        QUIET
        )
IF(Qt6Widgets_FOUND)
    MESSAGE(STATUS "Found Qt6Widgets: ${Qt6Widgets_VERSION}")
    SET(QTLIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
ELSE()
    FIND_PACKAGE(Qt5 COMPONENTS
            Core
            Gui
            Widgets
            Network
            REQUIRED
            )
    IF(Qt5Widgets_FOUND)
        MESSAGE(STATUS "Found Qt5Widgets: ${Qt5Widgets_VERSION}")
        SET(QTLIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
    ENDIF()
ENDIF()

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
        ${QTLIBS}
        shadowsocks-crypto
        mbedtls
        uv_a
        )

