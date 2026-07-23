find_package(Vulkan REQUIRED)

add_library(rath_engine STATIC
    src/Rath/Platform/window.cpp
 "src/Rath/Platform/window.cpp" "src/Rath/Core/application.cpp" "src/Rath/Renderer/renderer.cpp" "src/Rath/Renderer/device.cpp" "src/Rath/Renderer/context.cpp" "src/Rath/Renderer/swapchain.cpp")

target_include_directories(rath_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(rath_engine
    PUBLIC  enginemath::enginemath
    PRIVATE glfw Vulkan::Vulkan
)