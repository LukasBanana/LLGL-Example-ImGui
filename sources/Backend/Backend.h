/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Backend.h
 */

#pragma once

#include <LLGL/LLGL.h>
#include <LLGL/Platform/Platform.h>
#include "../Globals.h"
#include "imgui.h"
#include <functional>
#include <map>
#include <vector>
#include <memory>

class Backend;

using BackendPtr = std::unique_ptr<Backend>;

class Backend
{
public:
    using AllocateBackendFunc = std::function<BackendPtr()>;
    static void RegisterBackend(const char* name, AllocateBackendFunc onAllocateFunc);

    struct WindowContext
    {
        LLGL::SwapChain*    swapChain       = nullptr;
        ImGuiContext*       imGuiContext    = nullptr;
        Scene::View         view;
        LLGL::Offset2D      mousePosInWindow;
    };

public:
    virtual ~Backend();

    void Init();
    void Release();

    virtual void BeginFrame(WindowContext& context);
    virtual void EndFrame(ImDrawData* data) = 0;

    void RenderSceneForAllContexts();
    void RenderSceneForContext(WindowContext& context, float dt);

    void OnResizeSurface(WindowContext& context, const LLGL::Extent2D& size);

    static BackendPtr NewBackend(const char* name);

protected:
    virtual void InitContext(WindowContext& context);
    virtual void ReleaseContext(WindowContext& context);

    bool CreateResources(
        const char* moduleName,
        const char* vertShaderFilename,
        const char* vertShaderEntry,
        const char* vertShaderProfile,
        const char* fragShaderFilename,
        const char* fragShaderEntry,
        const char* fragShaderProfile
    );

private:
    LLGL::RenderingDebugger     debugger;
    std::uint64_t               lastTick = 0;
    std::vector<WindowContext>  windowContexts;
};

extern std::unique_ptr<Backend> g_backend;

#define REGISTER_BACKEND(CLASS, NAME)               \
    static struct Register ## CLASS                 \
    {                                               \
        Register ## CLASS()                         \
        {                                           \
            Backend::RegisterBackend(               \
                NAME,                               \
                []() -> BackendPtr                  \
                {                                   \
                    return BackendPtr(new CLASS{}); \
                }                                   \
            );                                      \
        }                                           \
    }                                               \
    g_registryEntry ## CLASS


