# auto compile shaders
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
file(GLOB SHADER_SOURCES
    ${SHADER_DIR}/*.vert
    ${SHADER_DIR}/*.frag
    ${SHADER_DIR}/*.comp)

set (SPV_FILES "")
foreach(SHADER ${SHADER_SOURCES})
    add_custom_command(
        OUTPUT ${SHADER}.spv
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER}.spv
        DEPENDS ${SHADER}
        COMMENT "compiling ${SHADER}")
    list(APPEND SPV_FILES ${SHADER}.spv)
endforeach()

add_custom_target(shaders DEPENDS ${SPV_FILES})

add_library(rath_engine STATIC
     "src/Rath/Platform/window.cpp"
     "src/Rath/Core/application.cpp" "src/Rath/VK/device.cpp" "src/Rath/VK/context.cpp")


target_include_directories(rath_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_precompile_headers(rath_engine PRIVATE src/Rath/Core/stdIncludes.hpp)

target_link_libraries(rath_engine
    PUBLIC  enginemath::enginemath
    PRIVATE glfw Vulkan::Vulkan)

add_dependencies(rath_engine shaders)
