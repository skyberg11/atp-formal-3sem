find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(TESTDIR "\"${CMAKE_SOURCE_DIR}/tests/testcases\"")
add_definitions( -DTESTDIR=${TESTDIR} )

add_executable(test test.cpp)
set(CMAKE_CXX_FLAGS --coverage)
target_link_libraries(test ${GTEST_LIBRARIES} pthread)
set_target_properties(test
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)