/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Globals.cpp
 */

#include "Globals.h"


LLGL::Input                     input;
LLGL::RenderSystemPtr           renderer;
std::vector<LLGL::SwapChain*>   g_swapChains;
LLGL::CommandBuffer*            cmdBuffer;
LLGL::PipelineState*            graphicsPSO;
Scene                           scene;


void ViewProjection(Scene::View& view, float aspectRatio, float nearPlane, float farPlane, float fov)
{
    auto& m = view.vpMatrix;

    const bool isUnitCube = (renderer->GetRenderingCaps().clippingRange == LLGL::ClippingRange::MinusOneToOne);

    const float fovRadian = fov*M_PI/180.0f;
    const float h = 1.0f / std::tanf(fovRadian / 2.0f);
    const float w = h / aspectRatio;

    const float m22 = (isUnitCube ? (farPlane + nearPlane)/(farPlane - nearPlane) : farPlane/(farPlane - nearPlane));
    const float m23 = (isUnitCube ? -(2.0f*farPlane*nearPlane)/(farPlane - nearPlane) : -(farPlane*nearPlane)/(farPlane - nearPlane));

    m[0][0] = w;    m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = h;    m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = m22;  m[2][3] = 1.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = m23;  m[3][3] = 0.0f;
}

void ModelRotation(Scene::View& view, float x, float y, float z, float angle)
{
    // Normalize axis vector
    const float axisLength = std::sqrtf(x*x + y*y + z*z);
    x /= axisLength;
    y /= axisLength;
    z /= axisLength;

    // Calculate matrix rotation
    const float c  = std::cosf(angle);
    const float s  = std::sinf(angle);
    const float cc = 1.0f - c;

    auto& m = view.wMatrix;
    m[0][0] = x*x*cc + c;
    m[1][0] = x*y*cc - z*s;
    m[2][0] = x*z*cc + y*s;

    m[0][1] = y*x*cc + z*s;
    m[1][1] = y*y*cc + c;
    m[2][1] = y*z*cc - x*s;

    m[0][2] = x*z*cc - y*s;
    m[1][2] = y*z*cc + x*s;
    m[2][2] = z*z*cc + c;
}
