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
#include "imgui.h"
#include <functional>
#include <map>

class Backend;

using BackendPtr = std::unique_ptr<Backend>;

class Backend
{
public:
    using AllocateBackendFunc = std::function<BackendPtr()>;
    static void RegisterBackend(const char* name, AllocateBackendFunc onAllocateFunc);

public:
    virtual ~Backend();

    virtual void InitImGui();
    virtual void NextFrame();
    virtual void DrawFrame(ImDrawData* data) = 0;

    static BackendPtr NewBackend(const char* name);

protected:
    void CreateResources(const char* moduleName);
};

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


