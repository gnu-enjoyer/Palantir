cmake_minimum_required(VERSION 3.16)

project(Palantir)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(Palantir
        src/palantir.hpp
        src/cache.cpp
        src/dispatcher.cpp
        src/scraper.cpp
        src/net.cpp
        src/main.cpp)

target_include_directories(Palantir
        PRIVATE
        include)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    include(FetchContent)

    find_package(PkgConfig REQUIRED)

    find_package(OpenSSL REQUIRED)

    FetchContent_Declare(
            zlib
            GIT_REPOSITORY https://github.com/madler/zlib.git
            GIT_TAG cacf7f1d4e3d44d871b605da3b647f07d718623f
    )

    FetchContent_Declare(
            hiredis
            GIT_REPOSITORY  https://github.com/redis/hiredis.git
            GIT_TAG 8a15f4d6578560f2a375c32fc567c4c88335c2a8
    )

    FetchContent_Declare(
            fmt
            GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
            GIT_TAG b6f4ceaed0a0a24ccf575fab6c56dd50ccf6f1a9
    )

    FetchContent_Declare(
            pugixml
            GIT_REPOSITORY  https://github.com/zeux/pugixml.git
            GIT_TAG dd50fa5b45ab8d58d6c27566c2eaf04a8b7e5841
    )

    FetchContent_MakeAvailable(
            zlib
            hiredis
            fmt
            pugixml
    )

    add_library(PalantirLib
            INTERFACE)

    target_link_libraries(PalantirLib
            INTERFACE
            OpenSSL::SSL OpenSSL::Crypto
            libz.a
            hiredis_static
            pugixml
            fmt
            )

    target_link_libraries(
            Palantir
            PalantirLib
    )
endif()

enable_testing ()

add_subdirectory (tests)
