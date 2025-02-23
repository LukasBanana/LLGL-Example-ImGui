/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Backend.cpp
 */

#include "Backend.h"
#include "../Globals.h"
#include "../Platform/Platform.h"
#include <LLGL/Utils/TypeNames.h>


using BackendRegisterMap = std::map<std::string, Backend::AllocateBackendFunc>;

Backend::~Backend()
{
    input.Drop(swapChain->GetSurface());

    PlatformShutdown();
}

static BackendRegisterMap& GetBackendRegisterMap()
{
    static BackendRegisterMap registeredBackends;
    return registeredBackends;
}

void Backend::RegisterBackend(const char* name, AllocateBackendFunc onAllocateFunc)
{
    auto& registeredBackends = GetBackendRegisterMap();
    registeredBackends[name] = onAllocateFunc;
}

void Backend::Init()
{
    PlatformInit(swapChain->GetSurface());
}

void Backend::BeginFrame()
{
    PlatformNewFrame(swapChain->GetSurface());
}

std::unique_ptr<Backend> Backend::NewBackend(const char* name)
{
    auto& registeredBackends = GetBackendRegisterMap();
    auto it = registeredBackends.find(name);
    return (it != registeredBackends.end() ? it->second() : std::unique_ptr<Backend>{});
}

void Backend::CreateResources(const char* moduleName)
{
    // Load render system model
    LLGL::Report report;
    renderer = LLGL::RenderSystem::Load(moduleName, &report);
    if (!renderer)
    {
        LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "%s", report.GetText());
        return;
    }

    #ifdef LLGL_OS_MACOS
    constexpr unsigned resX = 1280*2;
    constexpr unsigned resY = 768*2;
    #else
    constexpr unsigned resX = 1280;
    constexpr unsigned resY = 768;
    #endif

    LLGL::SwapChainDescriptor swapChainDesc;
    swapChainDesc.resolution = { resX, resY };
    swapChain = renderer->CreateSwapChain(swapChainDesc);

    cmdBuffer = renderer->CreateCommandBuffer(LLGL::CommandBufferFlags::ImmediateSubmit);

    // Print renderer information
    const LLGL::RendererInfo& info = renderer->GetRendererInfo();
    const LLGL::Extent2D swapChainRes = swapChain->GetResolution();

    LLGL::Log::Printf(
        "Render System:\n"
        "  Renderer:           %s\n"
        "  Device:             %s\n"
        "  Vendor:             %s\n"
        "  Shading Language:   %s\n"
        "\n"
        "Swap-Chain:\n"
        "  Resolution:         %u x %u\n"
        "  Samples:            %u\n"
        "  ColorFormat:        %s\n"
        "  DepthStencilFormat: %s\n"
        "\n",
        info.rendererName.c_str(),
        info.deviceName.c_str(),
        info.vendorName.c_str(),
        info.shadingLanguageName.c_str(),
        swapChainRes.width,
        swapChainRes.height,
        swapChain->GetSamples(),
        LLGL::ToString(swapChain->GetColorFormat()),
        LLGL::ToString(swapChain->GetDepthStencilFormat())
    );
}
