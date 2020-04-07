cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)

project(UnitTest)

enable_testing()
find_package(GTest REQUIRED)

add_definitions(-DUNIT_TEST)

## Test for pressure utl functions

set(TEST_PRESSION_SRC test_pression.cpp
                      ../srcs/pression.cpp                        
)

add_executable(test_pression ${TEST_PRESSION_SRC})
target_link_libraries(test_pression GTest::GTest GTest::Main)

add_test(TestPression test_pression) # add the test to the registry to be run with ctest

## End Test for pressure utl functions