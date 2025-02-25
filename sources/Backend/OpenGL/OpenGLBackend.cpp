/*
* LLGL Example ImGui
* Created on 02/22/2025 by L.Hermanns
* Published under the BSD-3 Clause License
* ----------------------------------------
* OpenGL Backend
*/

#include "../../Globals.h"

#include <LLGL/LLGL.h>
#include <LLGL/Platform/NativeHandle.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"

class OpenGLBackend final : public Backend
{
public:

    OpenGLBackend()
    {
        CreateResources(
            "OpenGL",

            // Vertex shader
            "OpenGLSceneShader.vert",
            nullptr,
            nullptr,

            // Pixel shader
            "OpenGLSceneShader.frag",
            nullptr,
            nullptr
        );
    }

    void Init() override
    {
        Backend::Init();

        ImGui_ImplOpenGL3_Init();
    }

    void Release() override
    {
        ImGui_ImplOpenGL3_Shutdown();

        Backend::Release();
    }

    void BeginFrame() override
    {
        Backend::BeginFrame();

        ImGui_ImplOpenGL3_NewFrame();
    }

    void EndFrame(ImDrawData* data) override
    {
        ImGui_ImplOpenGL3_RenderDrawData(data);
    }
};

REGISTER_BACKEND(OpenGLBackend, "OpenGL");
