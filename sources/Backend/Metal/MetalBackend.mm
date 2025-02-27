/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Metal Backend
 */

#include "../../Globals.h"
#include "../Backend.h"

#include <LLGL/LLGL.h>
#include <LLGL/Platform/NativeHandle.h>
#include <LLGL/Backend/Metal/NativeHandle.h>

#include "imgui.h"
#include "imgui_impl_metal.h"

#import <Metal/Metal.h>

class MetalBackend final : public Backend
{
    id<MTLDevice> mtlDevice = nil;
    id<MTLCommandBuffer> mtlCommandBuffer = nil;
    id<MTLRenderCommandEncoder> mtlRenderCmdEncoder = nil;
    MTLRenderPassDescriptor* mtlRenderPassDesc = nullptr;

public:

    MetalBackend()
    {
        CreateResources(
            "Metal",

            // Vertex shader
            "MetalSceneShader.metal",
            "VSMain",
            "1.1",

            // Pixel shader
            "MetalSceneShader.metal",
            "PSMain",
            "1.1"
        );
    }

    ~MetalBackend()
    {
        // Release Metal handles
        if (mtlDevice != nil)
            [mtlDevice release];
    }

    void InitContext(WindowContext& context) override
    {
        Backend::InitContext(context);

        // Setup renderer backend
        LLGL::Metal::RenderSystemNativeHandle nativeDeviceHandle;
        renderer->GetNativeHandle(&nativeDeviceHandle, sizeof(nativeDeviceHandle));
        mtlDevice = nativeDeviceHandle.device;

        ImGui_ImplMetal_Init(mtlDevice);
    }

    void ReleaseContext(WindowContext& context) override
    {
        ImGui::SetCurrentContext(context.imGuiContext);

        ImGui_ImplMetal_Shutdown();

        Backend::ReleaseContext(context);
    }

    void BeginFrame(WindowContext& context) override
    {
        Backend::BeginFrame(context);

        LLGL::Metal::CommandBufferNativeHandle nativeContextHandle;
        cmdBuffer->GetNativeHandle(&nativeContextHandle, sizeof(nativeContextHandle));

        mtlCommandBuffer = nativeContextHandle.commandBuffer;
        LLGL_VERIFY(mtlCommandBuffer != nil);

        mtlRenderCmdEncoder = (id<MTLRenderCommandEncoder>)nativeContextHandle.commandEncoder;
        LLGL_VERIFY(mtlRenderCmdEncoder != nil);

        mtlRenderPassDesc = nativeContextHandle.renderPassDesc;
        LLGL_VERIFY(mtlRenderPassDesc != nullptr);

        ImGui_ImplMetal_NewFrame(mtlRenderPassDesc);
    }

    void EndFrame(ImDrawData* data) override
    {
        // Encode render commands
        ImGui_ImplMetal_RenderDrawData(data, mtlCommandBuffer, mtlRenderCmdEncoder);

        [mtlRenderCmdEncoder release];
        [mtlRenderPassDesc release];
        [mtlCommandBuffer release];
    }
};

REGISTER_BACKEND(MetalBackend, "Metal");
