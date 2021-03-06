cmake_minimum_required(VERSION 2.8.9)
set(PROJECT_NAME Log)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")

# Run code to find log dependencies
include("${PROJECT_SOURCE_DIR}/src/CmakeLists.txt")

file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
add_executable(${PROJECT_TEST_NAME} ${TEST_SOURCES})
option(LOGGING "Enabled logging" ON)
if(LOG_DEPENDENCIES AND LOGGING)
    message(STATUS "LOGGING ENABLED")
    target_compile_definitions(${PROJECT_TEST_NAME} PUBLIC ${LOG_DEFINITIONS})
    target_link_libraries(${PROJECT_TEST_NAME} ${LOG_LIBRARIES})
else()
    message(STATUS "LOGGING DISABLED")
endif()
