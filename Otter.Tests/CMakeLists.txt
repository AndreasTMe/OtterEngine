project(Otter.Tests VERSION 0.1.0)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} gtest gtest_main)