find_package(Vulkan REQUIRED)

add_library(rath_engine STATIC
     "src/Rath/Platform/window.cpp" 
     "src/Rath/Core/application.cpp" 
     "src/Rath/Renderer/renderer.cpp" 
     "src/Rath/Renderer/device.cpp" 
     "src/Rath/Renderer/context.cpp" 
     "src/Rath/Renderer/swapchain.cpp" 
     "src/Rath/Renderer/pipeline.cpp" 
     "src/Rath/Renderer/renderpass.cpp" 
     "src/Rath/Renderer/buffers/vertex.cpp" 
     "src/Rath/Renderer/buffers/buffer.cpp" 
     "src/Rath/Renderer/buffers/uniform.cpp" 
     "src/Rath/Renderer/images/image.cpp" 
     "src/Rath/Renderer/descriptor.cpp"  
     "src/Rath/Renderer/images/texture.cpp" 
     "src/Rath/Renderer/images/depth.cpp" 
     "src/Rath/Renderer/models/model.cpp" 
     "src/Rath/Renderer/images/color.cpp"  "src/Rath/Renderer/buffers/storage.cpp" "src/Rath/Platform/input.cpp")

target_include_directories(rath_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(rath_engine
    PUBLIC  enginemath::enginemath 
    PRIVATE glfw Vulkan::Vulkan
)