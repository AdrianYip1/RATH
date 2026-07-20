find_package(Vulkan REQUIRED)

add_library(rath_engine STATIC
    src/Rath/Platform/window.cpp
)

target_include_directories(rath_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(rath_engine
    PUBLIC  enginemath::enginemath
    PRIVATE glfw Vulkan::Vulkan
)