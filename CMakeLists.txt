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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(Palantir
        src/net.cpp
        src/scraper.cpp
        src/dispatcher.cpp
        src/main.cpp
        )

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
            GIT_TAG f347743b7ded3bc7768c36126257bf3ae8c4e045
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


    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/palantir.cfg
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    enable_testing ()
    add_subdirectory (tests)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/palantir.cfg
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tests/)

endif()