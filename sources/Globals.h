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
#include <cstdint>
#include <cmath>


#ifndef M_PI
#define M_PI 3.141592654f
#endif


extern LLGL::Input              input;
extern LLGL::RenderSystemPtr    renderer;
extern LLGL::SwapChain*         swapChain;
extern LLGL::CommandBuffer*     cmdBuffer;
extern std::unique_ptr<Backend> backend;


struct Scene
{
    LLGL::PipelineState*    graphicsPSO     = nullptr;
    LLGL::Buffer*           viewCbuffer     = nullptr;
    LLGL::Buffer*           vertexBuffer    = nullptr;
    LLGL::Buffer*           indexBuffer     = nullptr;
    std::uint32_t           numIndices      = 0;

    struct Showcase
    {
        int                 rotateMode      = 0;
        float               rotation        = 0.0f;
        float               rotateSpeed     = 0.1f;
        bool                isVsync         = false;
    }
    showcase;

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
    }
    view;

    void ViewProjection(float aspectRatio = 1.0f, float nearPlane = 0.1f, float farPlane = 100.0f, float fov = 45.0f);
    void ModelRotation(float x, float y, float z, float angle);
};

extern Scene scene;

