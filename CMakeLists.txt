# LLGL Example ImGui
# Created on 02/22/2025 by L.Hermanns
# Licensed under the BSD-3 Clause License

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(LLGL-Example-ImGui VERSION 1.0 LANGUAGES C CXX OBJCXX)

# User options
option(EXAMPLE_WITH_VULKAN "Include Vulkan example. This requires the dependency to the Vulkan SDK." OFF)

# Gather source files
file(
    GLOB EXAMPLE_SOURCES_MAIN
    "sources/*.*"
)
file(
    GLOB IMGUI_SOURCES_MAIN
    "external/imgui/*.cpp"
    "external/imgui/*.h"
)

if(APPLE)
    file(
        GLOB EXAMPLE_SOURCES_BACKEND
        "sources/Backend/Metal/*.*"
        "sources/Backend/Vulkan/*.*"
    )
    file(
        GLOB IMGUI_SOURCES_BACKEND
        "external/imgui/backends/imgui_impl_metal.*"
        "external/imgui/backends/imgui_impl_osx.*"
    )
elseif(WIN32)
    file(
        GLOB EXAMPLE_SOURCES_BACKEND
        "sources/Backend/Direct3D11/*.*"
        "sources/Backend/Direct3D12/*.*"
        "sources/Backend/OpenGL/*.*"
        "sources/Backend/Vulkan/*.*"
    )
    file(
        GLOB IMGUI_SOURCES_BACKEND
        "external/imgui/backends/imgui_impl_dx11.*"
        "external/imgui/backends/imgui_impl_dx12.*"
        "external/imgui/backends/imgui_impl_opengl3.*"
        "external/imgui/backends/imgui_impl_opengl3_loader.*"
        "external/imgui/backends/imgui_impl_win32.*"
    )
elseif(LINUX)
    file(
        GLOB EXAMPLE_SOURCES_BACKEND
        "sources/Backend/OpenGL/*.*"
        "sources/Backend/Vulkan/*.*"
    )
    file(
        GLOB IMGUI_SOURCES_BACKEND
        "external/imgui/backends/imgui_impl_opengl3.*"
        "external/imgui/backends/imgui_impl_opengl3_loader.*"
    )
endif()

if(EXAMPLE_WITH_VULKAN)
    file(
        GLOB IMGUI_SOURCES_BACKEND_VULKAN
        "external/imgui/backends/imgui_impl_vulkan.*"
    )
    list(APPEND IMGUI_SOURCES_BACKEND ${IMGUI_SOURCES_BACKEND_VULKAN})
endif()

file(GLOB EXAMPLE_SOURCES_BACKEND_MAIN "sources/Backend/*.*")
list(APPEND EXAMPLE_SOURCES_BACKEND ${EXAMPLE_SOURCES_BACKEND_MAIN})

set(
    EXAMPLE_SOURCES
    ${EXAMPLE_SOURCES_MAIN}
    ${EXAMPLE_SOURCES_BACKEND}
    ${IMGUI_SOURCES_MAIN}
    ${IMGUI_SOURCES_BACKEND}
)

source_group("Sources" FILES ${EXAMPLE_SOURCES_MAIN})
source_group("Sources\\Backend" FILES ${EXAMPLE_SOURCES_BACKEND})
source_group("Sources\\ImGui" FILES ${IMGUI_SOURCES_MAIN} ${IMGUI_SOURCES_BACKEND})

# Add LLGL dependency
include(FindLLGL.cmake)
if(NOT LLGL_FOUND)
    message(FATAL_ERROR "Missing LLGL library")
endif()

# Create executable project
add_executable(LLGL-Example-ImGui ${EXAMPLE_SOURCES})

# Add Vulkan dependency
if(EXAMPLE_WITH_VULKAN)
    find_package(Vulkan REQUIRED)
    if(Vulkan_FOUND)
        include_directories(${Vulkan_INCLUDE_DIR})
        target_link_libraries(LLGL-Example-ImGui ${Vulkan_LIBRARY})
        if(APPLE)
            target_link_libraries(LLGL-Example-ImGui "-framework QuartzCore")
        endif()
    endif()
endif()

include_directories("${LLGL_INCLUDE_DIR}" "external/imgui" "external/imgui/backends")
target_link_libraries(LLGL-Example-ImGui ${LLGL_LIBRARIES})
