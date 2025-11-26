cmake_minimum_required(VERSION 3.22)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
# создание поискового сервера
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(search_engine main.cpp)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

# тестрование
FetchContent_Declare(googletest
		URL
		https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
target_link_libraries(search_engine PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(search_engine)

add_executable(run_tests
    test/search_server_test.cpp
)

target_link_libraries(run_tests PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)

