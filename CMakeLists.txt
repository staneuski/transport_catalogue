cmake_minimum_required(VERSION 3.0.0)
project(search_engine VERSION 0.6.0)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

include_directories(src tests)

set(GEO)
set(TRANSPORT
    src/transport/input_reader.cpp
    src/transport/transport_catalogue.cpp)

set(SRC ${GEO} ${IO} ${SVG} ${TRANSPORT})

#######################################
# SRC
#######################################
add_executable(main src/main.cpp ${SRC})


#######################################
# TESTS
#######################################
include(FetchContent)
FetchContent_Declare(
    googletest
   URL https://github.com/google/googletest/archive/0d9daa99365eb00a2b81392b6ec2c8a802769ef0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(gtest-transport_catalogue tests/gtest-transport_catalogue.cpp ${SRC})
target_link_libraries(gtest-transport_catalogue gtest_main -lpthread)
add_test(NAME transport_catalogue COMMAND gtest-transport_catalogue)