/*
* LLGL Example ImGui
* Created on 02/22/2025 by L.Hermanns
* Published under the BSD-3 Clause License
* ----------------------------------------
* OpenGL Backend
*/

#include "../Backend.h"
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

    void InitContext(WindowContext& context) override
    {
        Backend::InitContext(context);

        ImGui_ImplOpenGL3_Init();
    }

    void ReleaseContext(WindowContext& context) override
    {
        ImGui::SetCurrentContext(context.imGuiContext);

        ImGui_ImplOpenGL3_Shutdown();

        Backend::ReleaseContext(context);
    }

    void BeginFrame(WindowContext& context) override
    {
        Backend::BeginFrame(context);

        ImGui_ImplOpenGL3_NewFrame();
    }

    void EndFrame(ImDrawData* data) override
    {
        ImGui_ImplOpenGL3_RenderDrawData(data);
    }
};

REGISTER_BACKEND(OpenGLBackend, "OpenGL");
