cmake_minimum_required(VERSION 3.15)
project(rath VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/enginemath)

# Forcing depth to be from 0 to 1 for Vulkan in enginemath
target_compile_definitions(enginemath INTERFACE ENGINEMATH_DEPTH_ZERO_TO_ONE)

# glfw: skip docs/tests/examples, only want the library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

find_package(Vulkan REQUIRED)
message(STATUS "GLSLC: ${Vulkan_GLSLC_EXECUTABLE}")

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders)
file(GLOB SHADER_SOURCES
    ${SHADER_DIR}/*.vert
    ${SHADER_DIR}/*.frag
    ${SHADER_DIR}/*.comp)

set(SPV_FILES "")
foreach(SHADER ${SHADER_SOURCES})
    add_custom_command(
        OUTPUT ${SHADER}.spv
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER}.spv
        DEPENDS ${SHADER}
        COMMENT "compiling ${SHADER}")
    list(APPEND SPV_FILES ${SHADER}.spv)
endforeach()

add_custom_target(shaders DEPENDS ${SPV_FILES})

foreach(SPV ${SPV_FILES})
    get_filename_component(SPV_NAME ${SPV} NAME)
    add_custom_command(TARGET shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPV} ${CMAKE_BINARY_DIR}/shaders/${SPV_NAME})
endforeach()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE external/stb)

add_library(rath_engine STATIC
    engine/platform/window.cpp
    engine/app/application.cpp
    engine/rhi_vulkan/vkDevice.cpp
    engine/rhi_vulkan/vkSwapchain.cpp 
    engine/rhi_vulkan/vkRender.cpp
    engine/rhi_vulkan/vkPipeline.cpp
    engine/rhi_vulkan/vkBuffer.cpp)

target_include_directories(rath_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rath_engine
    PUBLIC enginemath::enginemath
    PRIVATE glfw Vulkan::Vulkan)

add_executable(rath sandbox/main.cpp 
"sandbox/visualizer.cpp")

target_link_libraries(rath PRIVATE rath_engine glfw Vulkan::Vulkan stb)

add_dependencies(rath shaders)