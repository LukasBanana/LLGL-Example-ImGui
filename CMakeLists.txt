# LLGL Example ImGui
# Created on 02/22/2025 by L.Hermanns
# Licensed under the BSD-3 Clause License

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(LLGL-Example-ImGui VERSION 1.0 LANGUAGES C CXX OBJCXX)

set(CMAKE_CXX_STANDARD 11)

# User options
option(EXAMPLE_WITH_VULKAN "Include Vulkan example. This requires the dependency to the Vulkan SDK." OFF)

# Gather source files
file(
    GLOB EXAMPLE_SOURCES_MAIN
    "sources/*.cpp"
    "sources/*.h"
)
file(
    GLOB IMGUI_SOURCES_MAIN
    "external/imgui/*.cpp"
    "external/imgui/*.h"
)

if(APPLE)
    file(
        GLOB EXAMPLE_SOURCES_PLATFORM
        "sources/Platform/MacOS/*.mm"
        "sources/Platform/MacOS/Info.plist"
    )
    file(
        GLOB EXAMPLE_SOURCES_BACKEND
        "sources/Backend/Metal/*.mm"
        "sources/Backend/Metal/*.cpp"
        "sources/Backend/Metal/*.h"
        "sources/Backend/OpenGL/*.cpp"
        "sources/Backend/OpenGL/*.h"
    )
    file(
        GLOB IMGUI_SOURCES_BACKEND
        "external/imgui/backends/imgui_impl_metal.*"
        "external/imgui/backends/imgui_impl_osx.*"
        "external/imgui/backends/imgui_impl_opengl3.*"
        "external/imgui/backends/imgui_impl_opengl3_loader.*"
    )
elseif(WIN32)
    file(
        GLOB EXAMPLE_SOURCES_PLATFORM
        "sources/Platform/Win32/*.*"
    )
    file(
        GLOB EXAMPLE_SOURCES_BACKEND
        "sources/Backend/Direct3D11/*.cpp"
        "sources/Backend/Direct3D11/*.h"
        "sources/Backend/Direct3D12/*.cpp"
        "sources/Backend/Direct3D12/*.h"
        "sources/Backend/OpenGL/*.cpp"
        "sources/Backend/OpenGL/*.h"
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
        GLOB EXAMPLE_SOURCES_PLATFORM
        "sources/Platform/Linux/*.*"
    )
    file(
        GLOB EXAMPLE_SOURCES_BACKEND
        "sources/Backend/OpenGL/*.*"
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

    file(
        GLOB EXAMPLE_SOURCES_BACKEND_VULKAN
        "sources/Backend/Vulkan/*.cpp"
        "sources/Backend/Vulkan/*.h"
    )
    list(APPEND EXAMPLE_SOURCES_BACKEND ${EXAMPLE_SOURCES_BACKEND_VULKAN})
endif()

file(GLOB EXAMPLE_SOURCES_BACKEND_MAIN "sources/Backend/*.*")
list(APPEND EXAMPLE_SOURCES_BACKEND ${EXAMPLE_SOURCES_BACKEND_MAIN})

file(GLOB EXAMPLE_SOURCES_PLATFORM_MAIN "sources/Platform/*.*")
list(APPEND EXAMPLE_SOURCES_PLATFORM ${EXAMPLE_SOURCES_PLATFORM_MAIN})

set(
    EXAMPLE_SOURCES
    ${EXAMPLE_SOURCES_MAIN}
    ${EXAMPLE_SOURCES_PLATFORM}
    ${EXAMPLE_SOURCES_BACKEND}
    ${IMGUI_SOURCES_MAIN}
    ${IMGUI_SOURCES_BACKEND}
)

source_group("Sources" FILES ${EXAMPLE_SOURCES_MAIN})
source_group("Sources\\Platform" FILES ${EXAMPLE_SOURCES_PLATFORM})
source_group("Sources\\Backend" FILES ${EXAMPLE_SOURCES_BACKEND})
source_group("Sources\\ImGui" FILES ${IMGUI_SOURCES_MAIN} ${IMGUI_SOURCES_BACKEND})

# Add LLGL dependency
include(FindLLGL.cmake)
if(NOT LLGL_FOUND)
    message(FATAL_ERROR "Missing LLGL library")
endif()

# Create executable project
if(APPLE)
    add_executable(LLGL-Example-ImGui MACOSX_BUNDLE ${EXAMPLE_SOURCES})
    set_target_properties(
        LLGL-Example-ImGui PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER        "LLGL.Examples.ImGui"
        MACOSX_BUNDLE_LONG_VERSION_STRING   "LLGL.Examples.ImGui-1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING  "1.0"
        MACOSX_BUNDLE_BUNDLE_VERSION        1.0
        MACOSX_BUNDLE                       ON
        MACOSX_BUNDLE_INFO_PLIST            "${PROJECT_SOURCE_DIR}/sources/Platform/MacOS/Info.plist"
    )
elseif(WIN32)
    add_executable(LLGL-Example-ImGui WIN32 ${EXAMPLE_SOURCES})
else()
    add_executable(LLGL-Example-ImGui ${EXAMPLE_SOURCES})
endif()

if(APPLE)
    target_link_libraries(LLGL-Example-ImGui "-framework Cocoa -framework Foundation -framework Metal -framework GameController")
endif()

# Sets the working directory for startup for the specified project.
function(set_project_working_dir PROJECT_NAME WORKING_DIR)
    if(WIN32)
        # VS_DEBUGGER_WORKING_DIRECTORY requires CMake 3.8.2
        if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.8.2")
            set_target_properties(
                ${PROJECT_NAME} PROPERTIES
                VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}"
            )
        endif()
    elseif(APPLE)
        if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17.5")
            # XCODE_SCHEME_WORKING_DIRECTORY requires CMake 3.17.5
            set_target_properties(
                ${PROJECT_NAME} PROPERTIES
                XCODE_GENERATE_SCHEME           TRUE
                XCODE_SCHEME_WORKING_DIRECTORY  "${WORKING_DIR}"
            )
        endif()
    endif()
endfunction()

set_project_working_dir(LLGL-Example-ImGui "${PROJECT_SOURCE_DIR}")

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
