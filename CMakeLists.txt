cmake_minimum_required(VERSION 3.16)
project(Palantir)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(Palantir
        src/net.cpp
        src/scraper.cpp
        src/dispatcher.cpp
        src/main.cpp
        )

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    find_package(PkgConfig REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(hiredis 0.13.3 REQUIRED)

    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/zlib-1.2.11)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/pugixml-1.11)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/fmt-8.1.0)

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${PROJECT_SOURCE_DIR}/deps/zlib-1.2.11
            ${PROJECT_SOURCE_DIR}/deps/hiredis-1.0.2
            ${PROJECT_SOURCE_DIR}/deps/pugixml-1.11/src
            ${PROJECT_SOURCE_DIR}/deps/fmt-8.1.0/include
            )

    target_link_libraries(${PROJECT_NAME}
            OpenSSL::SSL OpenSSL::Crypto
            zlib
            HIREDIS::HIREDIS
            pugixml
            fmt
            )

endif()