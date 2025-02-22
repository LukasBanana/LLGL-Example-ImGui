/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * ImGuiMacOS.mm
 */

#include "../Platform.h"
#include <LLGL/Platform/NativeHandle.h>

#include "imgui_impl_osx.h"


static NSView* GetContentViewFromNativeHandle(const LLGL::NativeHandle& nativeHandle)
{
    if ([nativeHandle.responder isKindOfClass:[NSWindow class]])
    {
        /* Interpret responder as NSWindow */
        return [(NSWindow*)nativeHandle.responder contentView];
    }
    if ([nativeHandle.responder isKindOfClass:[NSView class]])
    {
        /* Interpret responder as NSView */
        return (NSView*)nativeHandle.responder;
    }
    LLGL::Trap(LLGL::Exception::InvalidArgument, __FUNCTION__, "NativeHandle::responder is neither of type NSWindow nor NSView");
}

static NSView* GetNSViewFromSurface(LLGL::Surface& surface)
{
    LLGL::Window& wnd = LLGL::CastTo<LLGL::Window>(surface);
    LLGL::NativeHandle nativeHandle;
    wnd.GetNativeHandle(&nativeHandle, sizeof(nativeHandle));
    return GetContentViewFromNativeHandle(nativeHandle);
}

void PlatformInit(LLGL::Surface& surface)
{
    ImGui_ImplOSX_Init(GetNSViewFromSurface(surface));
}

void PlatformNewFrame(LLGL::Surface& surface)
{
    ImGui_ImplOSX_NewFrame(GetNSViewFromSurface(surface));
}

void PlatformShutdown()
{
    ImGui_ImplOSX_Shutdown();
}

