cmake_minimum_required(VERSION 3.0.0)
project(transport_catalogue VERSION 0.2.0)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS_DEBUG -fsanitize=address)

add_compile_options(-Wall -Wextra -Werror)


include_directories(src tests)

set(GEO)
set(HELPERS)
set(JSON src/json/json.cpp src/json/json_builder.cpp)
set(SVG src/svg/svg.cpp)
set(TRANSPORT
    src/transport/map_renderer.cpp
    src/transport/json_reader.cpp
    src/transport/catalogue.cpp)

set(SRC ${GEO} ${JSON} ${HELPERS} ${SVG} ${TRANSPORT})


#######################################
# APPS
#######################################
add_executable(main src/main.cpp ${SRC})
add_executable(app-svg tests/app-svg.cpp ${SVG})
add_executable(app-json tests/app-json.cpp ${JSON})
add_executable(input_generator tests/input_generator.cpp ${JSON})


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


#- transport/catalogue.h
add_executable(gtest-transport tests/gtest-transport.cpp ${SRC})
target_link_libraries(gtest-transport gtest_main)
add_test(NAME transport COMMAND gtest-transport)

#- json/json.h
add_executable(gtest-json tests/gtest-json.cpp ${JSON})
target_link_libraries(gtest-json gtest_main)
add_test(NAME json COMMAND gtest-json)
