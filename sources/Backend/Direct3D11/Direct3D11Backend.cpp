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
#include "imgui_impl_dx11.h"

class Direct3D11Backend final : public Backend
{
    // Global variables for the Direct3D 11 backend
    ID3D11Device*           d3dDevice           = nullptr;
    ID3D11DeviceContext*    d3dDeviceContext    = nullptr;

public:

    Direct3D11Backend()
    {
        CreateResources(
            "Direct3D11",

            // Vertex shader
            "Direct3D11SceneShader.hlsl",
            "VSMain",
            "vs_5_0",

            // Pixel shader
            "Direct3D11SceneShader.hlsl",
            "PSMain",
            "ps_5_0"
        );
    }

    ~Direct3D11Backend()
    {
        ImGui_ImplDX11_Shutdown();

        // Release D3D handles
        if (d3dDevice != nullptr)
            d3dDevice->Release();
        if (d3dDeviceContext != nullptr)
            d3dDeviceContext->Release();
    }

    void Init() override
    {
        Backend::Init();

        // Setup renderer backend
        LLGL::Direct3D11::RenderSystemNativeHandle nativeDeviceHandle;
        renderer->GetNativeHandle(&nativeDeviceHandle, sizeof(nativeDeviceHandle));
        d3dDevice = nativeDeviceHandle.device;

        LLGL::Direct3D11::CommandBufferNativeHandle nativeContextHandle;
        cmdBuffer->GetNativeHandle(&nativeContextHandle, sizeof(nativeContextHandle));
        d3dDeviceContext = nativeContextHandle.deviceContext;

        ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);
    }

    void BeginFrame() override
    {
        Backend::BeginFrame();

        ImGui_ImplDX11_NewFrame();
    }

    void EndFrame(ImDrawData* data) override
    {
        ImGui_ImplDX11_RenderDrawData(data);
    }
};

REGISTER_BACKEND(Direct3D11Backend, "Direct3D11");
