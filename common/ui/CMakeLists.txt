# Minimum required version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
# Project name and language
PROJECT(CommonWidgets LANGUAGES CXX)

# Enable Qt's automatic features
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
#SET(CMAKE_AUTOUIC ON)      # Uncomment if using .ui files

# Find required Qt6 modules
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


# Source files
SET(COMMONWIDGETS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/searchable_treeview.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/searchable_treeview_model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/searchable_treeWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_main_frame.cpp
)
SET(COMMONWIDGETS_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/searchable_treeview.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/searchable_treeview_model.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/searchable_treeWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ui_main_frame.h
)

# Define the executable target
ADD_LIBRARY(CommonWidgets
        ${COMMONWIDGETS_SRC}
        ${COMMONWIDGETS_HEADERS}
)


# Set C++ standard
TARGET_COMPILE_FEATURES(CommonWidgets
        PRIVATE
        cxx_std_17
)


# Additional include directories
TARGET_INCLUDE_DIRECTORIES(CommonWidgets
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)



# Link Qt libraries to the target
TARGET_LINK_LIBRARIES(CommonWidgets
        PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
)
