/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * ImGuiWin32.cpp
 */

#include "../Platform.h"
#include <LLGL/Platform/NativeHandle.h>

#include "imgui_impl_win32.h"


void PlatformInit(LLGL::Surface& surface)
{
    LLGL::NativeHandle nativeHandle;
    surface.GetNativeHandle(&nativeHandle, sizeof(nativeHandle));

    ImGui_ImplWin32_Init(nativeHandle.window);
}

void PlatformNewFrame(LLGL::Surface& /*surface*/)
{
    ImGui_ImplWin32_NewFrame();
}

void PlatformShutdown()
{
    ImGui_ImplWin32_Shutdown();
}

