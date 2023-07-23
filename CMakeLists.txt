cmake_minimum_required(VERSION 3.3)

project(DecoratorVerify)

if( NOT TARGET_BUILD_PLATFORM STREQUAL "windows")
    message(FATAL_ERROR "platform is not supported now! - ${TARGET_BUILD_PLATFORM}")
endif()

include(cmake_params/common_params.cmake)

set(DECORATOR_ROOT_DIR ${PROJECT_SOURCE_DIR}/decorator)
set(FIRST_APP_ROOT_DIR ${PROJECT_SOURCE_DIR}/first_app)

include(decorator/CMakeLists.txt)
include(first_app/CMakeLists.txt)

target_link_libraries(FirstApp Decorator)