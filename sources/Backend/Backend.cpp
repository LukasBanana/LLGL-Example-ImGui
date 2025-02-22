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

void Backend::InitImGui()
{
    PlatformInit(swapChain->GetSurface());
}

void Backend::NextFrame()
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

    constexpr unsigned resX = 1280;
    constexpr unsigned resY = 768;

    LLGL::SwapChainDescriptor swapChainDesc;
    swapChainDesc.resolution = { resX, resY };
    swapChain = renderer->CreateSwapChain(swapChainDesc);

    cmdBuffer = renderer->CreateCommandBuffer(LLGL::CommandBufferFlags::ImmediateSubmit);
}
