cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Release)
list(APPEND CMAKE_PREFIX_PATH "../aws-sdk-cpp/build/out")
set(CMAKE_CXX_STANDARD 20)
project(sqs LANGUAGES CXX)

find_package(AWSSDK COMPONENTS sqs)

add_executable(${PROJECT_NAME} "./main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC ${AWSSDK_LINK_LIBRARIES})
