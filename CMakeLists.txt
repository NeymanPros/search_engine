cmake_minimum_required(VERSION 3.22)
project(Search_engine)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# создание поискового сервера

add_executable(Search_engine main.cpp
				src/convert.cpp
				src/inv_index.cpp
				src/search_server.cpp)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(Search_engine PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(Search_engine PRIVATE include)


# тестрование

add_executable(run_tests
				test/search_server_test.cpp
				test/inv_index_test.cpp
				src/convert.cpp
				src/inv_index.cpp
				src/search_server.cpp)

FetchContent_Declare(googletest
		URL
		https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
target_link_libraries(run_tests PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(run_tests)

target_link_libraries(run_tests PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(run_tests PRIVATE include)

include(GoogleTest)
gtest_discover_tests(run_tests)

