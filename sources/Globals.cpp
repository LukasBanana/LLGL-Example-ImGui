/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Globals.cpp
 */

#include "Globals.h"

LLGL::Input                 input;
LLGL::RenderSystemPtr       renderer;
LLGL::SwapChain*            swapChain;
LLGL::CommandBuffer*        cmdBuffer;
std::unique_ptr<Backend>    backend;
