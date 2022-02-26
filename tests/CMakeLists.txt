Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.0-preview4
        )

FetchContent_MakeAvailable(Catch2)

add_executable(PalanTest
        ../src/net.cpp
        ../src/scraper.cpp
        ../src/dispatcher.cpp
        tests.cpp
        )

target_include_directories(PalanTest
        PRIVATE
        ../src/
        )

target_link_libraries(PalanTest
        PalantirLib
        Catch2::Catch2
        )

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include (CTest)
include (Catch)
catch_discover_tests(PalanTest)