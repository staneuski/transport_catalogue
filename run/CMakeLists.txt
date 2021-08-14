cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(LIB ../lib)

set(GEOLIB "${LIB}/geo")
set(GEOLIB_FILES "${GEOLIB}/geo.h" "${GEOLIB}/geo.proto")

set(JSONLIB "${LIB}/json")
set(JSONLIB_FILES
    "${JSONLIB}/json.h" "${JSONLIB}/json.cpp"
    "${JSONLIB}/json_builder.h" "${JSONLIB}/json_builder.cpp")

set(SVGLIB "${LIB}/svg")
set(SVGLIB_FILES "${SVGLIB}/svg.h" "${SVGLIB}/svg.cpp")

set(GRAPHLIB "${LIB}/graph")
set(GRAPHLIB_FILES
    "${GRAPHLIB}/graph.h" "${GRAPHLIB}/ranges.h" "${GRAPHLIB}/router.h")

set(SRC ../src)
set(SRC_FILES
    "${SRC}/catalogue.h" "${SRC}/catalogue.cpp" "${SRC}/catalogue.proto"
    "${SRC}/database.proto"
    "${SRC}/domain.h" "${SRC}/domain.proto"
    "${SRC}/json_reader.h" "${SRC}/json_reader.cpp"
    "${SRC}/map_renderer.h" "${SRC}/map_renderer.cpp"
    "${SRC}/request_handler.h"
    "${SRC}/router.h" "${SRC}/router.cpp"
    "${SRC}/serialization.h" "${SRC}/serialization.cpp")

set(SRCS ${GEOLIB_FILES} ${JSONLIB_FILES} ${SVGLIB_FILES} ${GRAPHLIB_FILES} ${SRC_FILES})

include_directories(${LIB} ${SRC})

add_compile_options(-Wall -Wextra -Werror)

# Protobuf Library
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

set(${Protobuf_IMPORT_DIRS} ${GEOLIB} ${JSONLIB} ${SVGLIB} ${GRAPHLIB})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    "${GEOLIB}/geo.proto" "${SRC}/domain.proto" "${SRC}/catalogue.proto"
    "${SRC}/database.proto")

string(REPLACE
    "protobuf.lib" "protobufd.lib"
    "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE
    "protobuf.a" "protobufd.a"
    "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

message( ${Protobuf_INCLUDE_DIRS} )
# Executables
add_executable(make_base make_base.cpp ${PROTO_SRCS} ${PROTO_HDRS} ${SRCS})
target_include_directories(make_base PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(make_base PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(make_base
    "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>"
    Threads::Threads)

add_executable(input_generator input_generator.cpp ${JSONLIB_FILES})
add_executable(app-json app-json.cpp ${JSONLIB_FILES})
add_executable(app-svg app-svg.cpp ${SVGLIB_FILES})


# Google test
# include(FetchContent)
# FetchContent_Declare(
#     googletest
#    URL https://github.com/google/googletest/archive/0d9daa99365eb00a2b81392b6ec2c8a802769ef0.zip
# )
# FetchContent_MakeAvailable(googletest)
# enable_testing()

# #- json/json.h
# add_executable(gtest-json gtest-json.cpp ${JSONLIB_FILES})
# target_link_libraries(gtest-json gtest_main)
# add_test(NAME json COMMAND gtest-json)

# #- catalogue.h
# add_executable(gtest-transport gtest-transport.cpp ${SRCS})
# target_link_libraries(gtest-transport gtest_main)
# add_test(NAME transport COMMAND gtest-transport)
