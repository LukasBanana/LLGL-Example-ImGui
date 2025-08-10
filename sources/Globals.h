/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Globals.h
 */

#pragma once

#include <LLGL/LLGL.h>
#include <memory>
#include <cstdint>
#include <cmath>
#include <vector>


#ifndef WITH_IMGUI
#define WITH_IMGUI 1
#endif

#ifndef M_PI
#define M_PI 3.141592654f
#endif


struct Scene
{
    LLGL::PipelineState*    graphicsPSO     = nullptr;
    LLGL::Buffer*           viewCbuffer     = nullptr;
    LLGL::Buffer*           vertexBuffer    = nullptr;
    LLGL::Buffer*           indexBuffer     = nullptr;
    std::uint32_t           numIndices      = 0;
};

struct alignas(16) View
{
    float vpMatrix[4][4]    = { { 1.0f, 0.0f, 0.0f, 0.0f },
                                { 0.0f, 1.0f, 0.0f, 0.0f },
                                { 0.0f, 0.0f, 1.0f, 0.0f },
                                { 0.0f, 0.0f, 0.0f, 1.0f } };
    float wMatrix[4][4]     = { { 1.0f, 0.0f, 0.0f, 0.0f },
                                { 0.0f, 1.0f, 0.0f, 0.0f },
                                { 0.0f, 0.0f, 1.0f, 0.0f },
                                { 0.0f, 0.0f, 7.0f, 1.0f } };
    float modelColor[4]     = { 1.0f, 1.0f, 1.0f, 1.0f };
    float lightVector[4]    = { 0.0f, 0.0f, 1.0f, 0.0f };
};


extern LLGL::RenderSystemPtr    renderer;
extern LLGL::CommandBuffer*     cmdBuffer;
extern Scene                    scene;
extern bool                     quitDemo;


void ViewProjection(View& view, float aspectRatio = 1.0f, float nearPlane = 0.1f, float farPlane = 100.0f, float fov = 45.0f);
void ModelRotation(View& view, float x, float y, float z, float angle);

