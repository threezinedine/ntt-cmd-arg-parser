cmake_minimum_required(VERSION 3.10)
project(NTTArgParser)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(NTTArgParser_USE_TESTS "Build tests" OFF)
option(NTTArgParser_USE_EXAMPLES "Build examples" OFF)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vendors)
    set(CMAKE_FOLDER "vendors")
    add_subdirectory(vendors)
    unset(CMAKE_FOLDER)
endif()

file(
    GLOB
    CPP_SOURCE_FILES
    src/*.cpp
)

file(
    GLOB
    HEADER_SOURCE_FILES
    src/*.hpp
)

set(
    MAIN_HEADER
    include/NTTArgParser.hpp
)

set(
    COMMON_DEFINITIONS
)

if (UNIX)
    set(
        COMMON_DEFINITIONS  
        ${COMMON_DEFINITIONS}
        -DNTT_PLATFORM_UNIX
    )
else()
    set(
        COMMON_DEFINITIONS
        ${COMMON_DEFINITIONS}
        -DNTT_PLATFORM_WINDOWS
    )
endif()

if (MSVC)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build configurations" FORCE)
    set_property(CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS "Debug" "Release")

    set(
        COMMON_DEFINITIONS
        ${COMMON_DEFINITIONS}
        -DNTT_MSVC
        $<$<CONFIG:Debug>:NTT_DEBUG>
        $<$<CONFIG:Release>:NTT_NDEBUG>
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
        -Wswitch-enum
        -Wcast-align
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(
            COMMON_DEFINITIONS
            ${COMMON_DEFINITIONS}
            -DNTT_DEBUG
        )
    else()
        set(
            COMMON_DEFINITIONS
            ${COMMON_DEFINITIONS}
            -DNTT_NDEBUG
        )
    endif()
endif()

add_library(
    ${PROJECT_NAME}
    STATIC
    ${CPP_SOURCE_FILES}
    ${HEADER_SOURCE_FILES}
    ${MAIN_HEADER}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ./include
    ./src
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    NTTLib
)

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    ${COMMON_DEFINITIONS}
)

if (NTTArgParser_USE_TESTS)
    file(
        GLOB 
        TEST_SOURCE_FILES
        tests/*.cpp
    )

    set(TEST_PROJECT_NAME ${PROJECT_NAME}_test)

    add_executable(
        ${TEST_PROJECT_NAME}
        ${TEST_SOURCE_FILES}
    )

    target_link_libraries(
        ${TEST_PROJECT_NAME}
        PRIVATE 
        ${PROJECT_NAME}
        gtest
        gmock
    )

    target_compile_definitions(
        ${TEST_PROJECT_NAME}
        PUBLIC
        ${COMMON_DEFINITIONS}
    )
endif()

if (NTTArgParser_USE_EXAMPLES)
    set(CMAKE_FOLDER "examples")
        add_subdirectory(examples)
    unset(CMAKE_FOLDER)
endif()

