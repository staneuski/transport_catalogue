cmake_minimum_required(VERSION 3.0.0)
project(transport_catalogue VERSION 0.2.0)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

include_directories(src tests)

set(GEO)
set(SVG src/svg/svg.cpp)
set(JSON src/json/json.cpp)
set(TRANSPORT
    src/transport/domain.cpp
    src/transport/json_reader.cpp
    src/transport/transport_catalogue.cpp)

set(SRC ${GEO} ${JSON} ${SVG} ${TRANSPORT})

#######################################
# APPS
#######################################
add_executable(main src/main.cpp ${SRC})
add_executable(app-svg tests/app-svg.cpp ${SVG})
add_executable(app-json tests/app-json.cpp ${JSON})


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


#- transport/transport_catalogue.h
add_executable(gtest-transport tests/gtest-transport.cpp ${SRC})
target_link_libraries(gtest-transport gtest_main)
add_test(NAME transport COMMAND gtest-transport)

#- json/json.h
add_executable(gtest-json tests/gtest-json.cpp ${JSON})
target_link_libraries(gtest-json gtest_main)
add_test(NAME json COMMAND gtest-json)
