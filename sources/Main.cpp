/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Demonstrates how to use Dear ImGui library with LLGL
 */

#include <LLGL/LLGL.h>
#include <LLGL/Platform/Platform.h>

#include "imgui.h"
#include "Globals.h"
#include <string.h>
#include <cmath>


static std::unique_ptr<Backend> CreateLLGLBackend(int argc, char* argv[])
{
    if (argc >= 2)
    {
        // Create backend by its module name
        return Backend::NewBackend(argv[1]);
    }

    // Create default backend for the respective platform
    #if defined LLGL_OS_WIN32
    return Backend::NewBackend("Direct3D11");
    #elif defined LLGL_OS_LINUX
    return Backend::NewBackend("OpenGL");
    #elif defined LLGL_OS_MACOS
    return Backend::NewBackend("Metal");
    #else
    #error Unsupported platform for this example!
    #endif
}

static int InitExample(int argc, char* argv[])
{
    // Initialize logging
    #ifdef __APPLE__
    LLGL::Log::RegisterCallbackStd();
    #else
    LLGL::Log::RegisterCallbackStd(LLGL::Log::StdOutFlags::Colored);
    #endif

    // Create LLGL backend
    backend = CreateLLGLBackend(argc, argv);
    if (!backend)
    {
        LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "Failed to initialize backend!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Initialize LLGL/ImGui backend
    backend->Init();

    // Attach input listener to main window
    input.Listen(swapChain->GetSurface());

    return 0;
}

static void ShutdownExample()
{
    // Release backend
    backend.reset();

    // Shutdown ImGui
    ImGui::DestroyContext();

    // Unload LLGL
    LLGL::RenderSystem::Unload(std::move(renderer));
}

static void ForwardInputToImGui()
{
    // Forward user input to ImGui
    ImGuiIO& io = ImGui::GetIO();

    if (input.KeyDown(LLGL::Key::LButton))
    {
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
    }
    if (input.KeyUp(LLGL::Key::LButton))
    {
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
    }
}

int main(int argc, char* argv[])
{
    // Initialize example backend and ImGui
    int init = InitExample(argc, argv);
    if (init != 0)
        return init;

    while (LLGL::Surface::ProcessEvents() && !input.KeyPressed(LLGL::Key::Escape))
    {
        ForwardInputToImGui();

        // Render frame and present result on screen
        backend->RenderScene();
        swapChain->Present();

        // Reset input state
        input.Reset();
    }

    ShutdownExample();

    return 0;
}


