CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
PROJECT(PRISM LANGUAGES C CXX)


# Disable test-related variables
SET(LIBUV_BUILD_TESTS OFF CACHE BOOL "Disable libuv tests" FORCE)
SET(LIBUV_BUILD_BENCH OFF CACHE BOOL "Disable libuv benchmarks" FORCE)
SET(BUILD_TESTING OFF CACHE BOOL "Disable libuv tests" FORCE)

SET(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
SET(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
SET(BUILD_UNITTEST OFF CACHE BOOL "" FORCE)


ADD_DEFINITIONS(-D MBEDTLS_ALLOW_PRIVATE_ACCESS)

SET(MBEDTLS_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/ext/mbedtls-flat)
SET(LIBUV_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/ext/libuv)
SET(SS_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/ext/shadowsocks-crypto)
SET(PRISMUI_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/prism)

ADD_SUBDIRECTORY(${LIBUV_DIR})
ADD_SUBDIRECTORY(${MBEDTLS_DIR})
ADD_SUBDIRECTORY(${SS_DIR})
ADD_SUBDIRECTORY(${PRISMUI_DIR})
