/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Platform.h
 */

#pragma once

#include <LLGL/LLGL.h>

void PlatformInit(LLGL::Surface& surface);
void PlatformNewFrame(LLGL::Surface& surface);
void PlatformShutdown();

