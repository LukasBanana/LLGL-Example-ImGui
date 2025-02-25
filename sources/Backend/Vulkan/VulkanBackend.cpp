/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Vulkan Backend
 */

#include "../../Globals.h"

#include <LLGL/LLGL.h>
#include <LLGL/Platform/NativeHandle.h>
#include <LLGL/Backend/Vulkan/NativeHandle.h>

#include "imgui.h"
#include "imgui_impl_vulkan.h"

class VulkanBackend final : public Backend
{
    // Global variables for the Vulkan backend
    VkCommandBuffer vulkanCommandBuffer = VK_NULL_HANDLE;

public:

    VulkanBackend()
    {
        CreateResources(
            "Vulkan",

            // Vertex shader
            "VulkanSceneShader.vert.spv",
            nullptr,
            nullptr,

            // Pixel shader
            "VulkanSceneShader.frag.spv",
            nullptr,
            nullptr
        );
    }

    ~VulkanBackend()
    {
        ImGui_ImplVulkan_Shutdown();
    }

    void Init() override
    {
        Backend::Init();

        // Setup renderer backend
        LLGL::Vulkan::RenderSystemNativeHandle nativeDeviceHandle;
        renderer->GetNativeHandle(&nativeDeviceHandle, sizeof(nativeDeviceHandle));
        //d3dDevice = nativeDeviceHandle.device;

        LLGL::Vulkan::CommandBufferNativeHandle nativeContextHandle;
        cmdBuffer->GetNativeHandle(&nativeContextHandle, sizeof(nativeContextHandle));
        //d3dDeviceContext = nativeContextHandle.deviceContext;

        ImGui_ImplVulkan_InitInfo initInfo;
        {

        }
        ImGui_ImplVulkan_Init(&initInfo);
    }

    void BeginFrame() override
    {
        Backend::BeginFrame();

        ImGui_ImplVulkan_NewFrame();
    }

    void EndFrame(ImDrawData* data) override
    {
        ImGui_ImplVulkan_RenderDrawData(data, vulkanCommandBuffer);
    }
};

REGISTER_BACKEND(VulkanBackend, "Vulkan");
