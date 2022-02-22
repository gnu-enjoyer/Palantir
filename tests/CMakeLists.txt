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

target_include_directories(PalanTest PRIVATE
        ${PROJECT_SOURCE_DIR}/deps/zlib-1.2.11
        ${PROJECT_SOURCE_DIR}/deps/hiredis-1.0.2
        ${PROJECT_SOURCE_DIR}/deps/pugixml-1.11/src
        ${PROJECT_SOURCE_DIR}/deps/fmt-8.1.0/include
        ../src/
        )

target_link_libraries(PalanTest
        OpenSSL::SSL OpenSSL::Crypto
        zlib
        HIREDIS::HIREDIS
        pugixml
        fmt
        Catch2::Catch2WithMain
        )