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
#include "Backend/Backend.h"
#include "Globals.h"
#include <string.h>
#include <cmath>

#if _WIN32
#   include <Windows.h>
#endif


static std::unique_ptr<Backend> CreateLLGLBackend(const char* moduleName)
{
    if (moduleName != nullptr && *moduleName != '\0')
    {
        // Create backend by its module name
        return Backend::NewBackend(moduleName);
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

static int InitExample(const char* moduleName)
{
    // Initialize logging
    #ifdef __APPLE__
    LLGL::Log::RegisterCallbackStd();
    #else
    LLGL::Log::RegisterCallbackStd(LLGL::Log::StdOutFlags::Colored);
    #endif

    // Create LLGL backend
    g_backend = CreateLLGLBackend(moduleName);
    if (!g_backend)
    {
        LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "Failed to initialize backend!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

#if WITH_IMGUI
    // Initialize LLGL/ImGui backend
    g_backend->Init();
#endif

    // Attach input listener to main window
    for (LLGL::SwapChain* swapChain : g_swapChains)
        input.Listen(swapChain->GetSurface());

    return 0;
}

static void ShutdownExample()
{
#if WITH_IMGUI
    // Shutdown ImGui
    g_backend->Release();
#endif

    // Release backend
    g_backend.reset();

    // Unload LLGL
    LLGL::RenderSystem::Unload(std::move(renderer));
}

static bool IsAnyWindowOpen()
{
    for (LLGL::SwapChain* swapChain : g_swapChains)
    {
        auto& window = LLGL::CastTo<LLGL::Window>(swapChain->GetSurface());
        if (window.IsShown())
            return true;
    }
    return false;
}

#if _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nShowCmd)
#else
int main(int argc, char* argv[])
#endif
{
    // Initialize example backend and ImGui
#if _WIN32
    int init = InitExample(cmdLine);
#else
    int init = InitExample(argc >= 2 ? argv[1] : nullptr);
#endif
    if (init != 0)
        return init;

    while (LLGL::Surface::ProcessEvents() && !input.KeyPressed(LLGL::Key::Escape) && IsAnyWindowOpen())
    {
        // Render frame and present result on screen
        g_backend->RenderSceneForAllContexts();

        // Reset input state
        input.Reset();
    }

    ShutdownExample();

    return 0;
}


