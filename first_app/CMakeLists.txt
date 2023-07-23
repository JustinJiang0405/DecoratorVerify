#
# first_app
#

file(GLOB VERIFY_APP_HEADER_FILES
    ${FIRST_APP_ROOT_DIR}/include/*.h
)

file(GLOB VERIFY_APP_SOURCE_FILES
    ${FIRST_APP_ROOT_DIR}/src/*.cpp
)

set(VERIFY_APP_FILES
    ${VERIFY_APP_HEADER_FILES}
    ${VERIFY_APP_SOURCE_FILES}
)

add_executable(FirstApp ${VERIFY_APP_FILES})

target_include_directories(FirstApp PRIVATE ${DECORATOR_ROOT_DIR}/include ${FIRST_APP_ROOT_DIR}/include)

set(VERIFY_APP_COMMON_COMPILE_DEF "")
set(VERIFY_APP_DEBUG_COMPILE_DEF "")
set(VERIFY_APP_RELEASE_COMPILE_DEF "")

set(VERIFY_APP_COMPILE_DEF
    $<$<CONFIG:debug>:${VERIFY_APP_DEBUG_COMPILE_DEF}>
    $<$<CONFIG:release>:${VERIFY_APP_RELEASE_COMPILE_DEF}>
)
target_compile_definitions(FirstApp PRIVATE ${VERIFY_APP_COMPILE_DEF})

set(VERIFY_APP_LINK_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/bin
    ${VULKAN_LIB_DIR}
    ${GLFW_LIB_DIR}
)
target_link_directories(FirstApp PRIVATE ${VERIFY_APP_LINK_DIR})

set(VERIFY_APP_LINK_LIBS
    glfw3.lib
    vulkan-1.lib
)
target_link_libraries(FirstApp ${VERIFY_APP_LINK_LIBS})