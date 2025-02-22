/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Backend.cpp
 */

#include "Backend.h"

using BackendRegisterMap = std::map<std::string, Backend::AllocateBackendFunc>;

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

std::unique_ptr<Backend> Backend::NewBackend(const char* name)
{
    auto& registeredBackends = GetBackendRegisterMap();
    auto it = registeredBackends.find(name);
    return (it != registeredBackends.end() ? it->second() : std::unique_ptr<Backend>{});
}
