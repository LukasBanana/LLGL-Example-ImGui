/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Direct3D 11 Backend
 */

#include "../../Globals.h"

#include <LLGL/LLGL.h>
#include <LLGL/Platform/NativeHandle.h>
#include <LLGL/Backend/Direct3D11/NativeHandle.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

class BackendD3D11 final : public Backend
{
    // Global variables for the Direct3D 11 backend
    ID3D11Device*           d3dDevice           = nullptr;
    ID3D11DeviceContext*    d3dDeviceContext    = nullptr;

public:

    BackendD3D11()
    {
        renderer = LLGL::RenderSystem::Load("Direct3D11");

        LLGL::SwapChainDescriptor swapChainDesc;
        swapChainDesc.resolution = { 1280, 768 };
        swapChain = renderer->CreateSwapChain(swapChainDesc);

        cmdBuffer = renderer->CreateCommandBuffer(LLGL::CommandBufferFlags::ImmediateSubmit);
    }

    ~BackendD3D11()
    {
        input.Drop(swapChain->GetSurface());

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();

        // Release D3D handles
        if (d3dDevice != nullptr)
            d3dDevice->Release();
        if (d3dDeviceContext != nullptr)
            d3dDeviceContext->Release();
    }

    void InitImGui() override
    {
        // Setup platform backend
        LLGL::Window& wnd = LLGL::CastTo<LLGL::Window>(swapChain->GetSurface());
        LLGL::NativeHandle nativeHandle;
        wnd.GetNativeHandle(&nativeHandle, sizeof(nativeHandle));

        ImGui_ImplWin32_Init(nativeHandle.window);

        // Setup renderer backend
        LLGL::Direct3D11::RenderSystemNativeHandle nativeDeviceHandle;
        renderer->GetNativeHandle(&nativeDeviceHandle, sizeof(nativeDeviceHandle));
        d3dDevice = nativeDeviceHandle.device;

        LLGL::Direct3D11::CommandBufferNativeHandle nativeContextHandle;
        cmdBuffer->GetNativeHandle(&nativeContextHandle, sizeof(nativeContextHandle));
        d3dDeviceContext = nativeContextHandle.deviceContext;

        ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);
    }

    void NextFrame() override
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }

    void DrawFrame(ImDrawData* data) override
    {
        ImGui_ImplDX11_RenderDrawData(data);
    }
};

REGISTER_BACKEND(BackendD3D11, "Direct3D11");
