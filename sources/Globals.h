/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Globals.h
 */

#pragma once

#include <LLGL/LLGL.h>
#include "Backend/Backend.h"
#include <memory>

extern LLGL::Input              input;
extern LLGL::RenderSystemPtr    renderer;
extern LLGL::SwapChain*         swapChain;
extern LLGL::CommandBuffer*     cmdBuffer;
extern std::unique_ptr<Backend> backend;
