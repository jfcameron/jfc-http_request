# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(null)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DEMO "Build the demo" ON)
option(JFC_BUILD_DOCS "Build documentation" OFF)
option(JFC_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(thirdparty)

jfc_project(library
    NAME "jfchttp"
    VERSION 0.75
    DESCRIPTION "concurrent, object oriented http lib"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE STATIC

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CURL_INCLUDE_DIR}

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/curl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/curl/src/curl_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/curl/src/curl_get.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/curl/src/curl_post.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/curl/src/curl_request.cpp

    LIBRARIES
        ${CURL_LIBRARIES}

    DEPENDENCIES
	${CURL_LIBRARY_NAME}
)

if (JFC_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

