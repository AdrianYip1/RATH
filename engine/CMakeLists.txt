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
     "src/Rath/Core/application.cpp" 
     "src/Rath/Renderer/renderer.cpp" 
     "src/Rath/Renderer/device.cpp" 
     "src/Rath/Renderer/context.cpp" 
     "src/Rath/Renderer/swapchain.cpp" 
     "src/Rath/Renderer/pipeline.cpp" 
     "src/Rath/Renderer/renderpass.cpp" 
     "src/Rath/Renderer/buffers/buffer.cpp" 
     "src/Rath/Renderer/buffers/uniform.cpp" 
     "src/Rath/Renderer/images/image.cpp" 
     "src/Rath/Renderer/descriptor.cpp"  
     "src/Rath/Renderer/images/texture.cpp" 
     "src/Rath/Renderer/images/depth.cpp" 
     "src/Rath/Renderer/models/model.cpp" 
     "src/Rath/Renderer/images/color.cpp"  
     "src/Rath/Renderer/buffers/storage.cpp" 
     "src/Rath/Platform/input.cpp" 
     "src/Rath/Core/camera/cameraController.cpp" 
     "src/Rath/Core/camera/camera.cpp"
     "src/Rath/Renderer/models/material.cpp" 
     "src/Rath/Renderer/ui/ui.cpp")


target_include_directories(rath_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(rath_engine
    PUBLIC  enginemath::enginemath 
    PRIVATE glfw Vulkan::Vulkan
    PRIVATE imgui
)

add_dependencies(rath_engine shaders)