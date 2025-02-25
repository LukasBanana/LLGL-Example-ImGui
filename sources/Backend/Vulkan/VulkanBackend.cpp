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

#include <vulkan/vulkan.h>

#include "imgui.h"
#include "imgui_impl_vulkan.h"

static VkFormat GetVulkanColorFormat(LLGL::Format format)
{
    return (format == LLGL::Format::BGRA8UNorm ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM);
}

static VkFormat GetVulkanDepthStencilFormat(LLGL::Format format)
{
    return (format == LLGL::Format::D32Float ? VK_FORMAT_D32_SFLOAT : VK_FORMAT_D24_UNORM_S8_UINT);
}

static VkRenderPass CreateVulkanRenderPass(VkDevice vulkanDevice)
{
    VkAttachmentDescription vulkanAttachmentDescs[2] = {};
    {
        vulkanAttachmentDescs[0].flags          = 0;
        vulkanAttachmentDescs[0].format         = GetVulkanColorFormat(swapChain->GetColorFormat());
        vulkanAttachmentDescs[0].samples        = VK_SAMPLE_COUNT_1_BIT;
        vulkanAttachmentDescs[0].loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
        vulkanAttachmentDescs[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        vulkanAttachmentDescs[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
        vulkanAttachmentDescs[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        vulkanAttachmentDescs[0].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        vulkanAttachmentDescs[0].finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    {
        vulkanAttachmentDescs[1].flags          = 0;
        vulkanAttachmentDescs[1].format         = GetVulkanDepthStencilFormat(swapChain->GetDepthStencilFormat());
        vulkanAttachmentDescs[1].samples        = VK_SAMPLE_COUNT_1_BIT;
        vulkanAttachmentDescs[1].loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
        vulkanAttachmentDescs[1].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        vulkanAttachmentDescs[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
        vulkanAttachmentDescs[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        vulkanAttachmentDescs[1].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR;
        vulkanAttachmentDescs[1].finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    VkAttachmentReference vulkanAttachmentRefs[2] = {};
    {
        vulkanAttachmentRefs[0].attachment  = 0;
        vulkanAttachmentRefs[0].layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    {
        vulkanAttachmentRefs[1].attachment  = 1;
        vulkanAttachmentRefs[1].layout      = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR;
    }

    VkSubpassDescription vulkanSubpassDescs[1] = {};
    {
        vulkanSubpassDescs[0].flags                     = 0;
        vulkanSubpassDescs[0].pipelineBindPoint         = VK_PIPELINE_BIND_POINT_GRAPHICS;
        vulkanSubpassDescs[0].inputAttachmentCount      = 0;
        vulkanSubpassDescs[0].pInputAttachments         = nullptr;
        vulkanSubpassDescs[0].colorAttachmentCount      = 1;
        vulkanSubpassDescs[0].pColorAttachments         = &vulkanAttachmentRefs[0];
        vulkanSubpassDescs[0].pResolveAttachments       = nullptr;
        vulkanSubpassDescs[0].pDepthStencilAttachment   = &vulkanAttachmentRefs[1];
        vulkanSubpassDescs[0].preserveAttachmentCount   = 0;
        vulkanSubpassDescs[0].pPreserveAttachments      = nullptr;
    }

    VkRenderPassCreateInfo vulkanRenderPassInfo = {};
    {
        vulkanRenderPassInfo.sType              = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        vulkanRenderPassInfo.pNext              = nullptr;
        vulkanRenderPassInfo.flags              = 0;
        vulkanRenderPassInfo.attachmentCount    = sizeof(vulkanAttachmentDescs)/sizeof(vulkanAttachmentDescs[0]);
        vulkanRenderPassInfo.pAttachments       = vulkanAttachmentDescs;
        vulkanRenderPassInfo.subpassCount       = sizeof(vulkanSubpassDescs)/sizeof(vulkanSubpassDescs[0]);
        vulkanRenderPassInfo.pSubpasses         = vulkanSubpassDescs;
        vulkanRenderPassInfo.dependencyCount    = 0;
        vulkanRenderPassInfo.pDependencies      = nullptr;
    }
    VkRenderPass vulkanRenderPass = VK_NULL_HANDLE;
    VkResult result = vkCreateRenderPass(vulkanDevice, &vulkanRenderPassInfo, nullptr, &vulkanRenderPass);
    LLGL_VERIFY(result == VK_SUCCESS);
    return vulkanRenderPass;
}

class VulkanBackend final : public Backend
{
    // Global variables for the Vulkan backend
    VkDevice        vulkanDevice        = VK_NULL_HANDLE;
    VkRenderPass    vulkanRenderPass    = VK_NULL_HANDLE;

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
        if (vulkanRenderPass != VK_NULL_HANDLE)
            vkDestroyRenderPass(vulkanDevice, vulkanRenderPass, nullptr);
    }

    void Init() override
    {
        Backend::Init();

        // Setup renderer backend
        LLGL::Vulkan::RenderSystemNativeHandle nativeDeviceHandle;
        renderer->GetNativeHandle(&nativeDeviceHandle, sizeof(nativeDeviceHandle));
        vulkanDevice = nativeDeviceHandle.device;

        // Create Vulkan render pass
        vulkanRenderPass = CreateVulkanRenderPass(vulkanDevice);

        ImGui_ImplVulkan_InitInfo initInfo = {};
        {
            initInfo.Instance           = nativeDeviceHandle.instance;
            initInfo.PhysicalDevice     = nativeDeviceHandle.physicalDevice;
            initInfo.Device             = nativeDeviceHandle.device;
            initInfo.QueueFamily        = nativeDeviceHandle.queueFamily;
            initInfo.Queue              = nativeDeviceHandle.queue;
            initInfo.DescriptorPool     = VK_NULL_HANDLE;
            initInfo.DescriptorPoolSize = 64;
            initInfo.RenderPass         = vulkanRenderPass;
            initInfo.MinImageCount      = 2;
            initInfo.ImageCount         = 2;
            initInfo.MSAASamples        = VK_SAMPLE_COUNT_1_BIT;
        }
        ImGui_ImplVulkan_Init(&initInfo);
    }

    void Release() override
    {
        ImGui_ImplVulkan_Shutdown();

        Backend::Release();
    }

    void BeginFrame() override
    {
        Backend::BeginFrame();

        ImGui_ImplVulkan_NewFrame();
    }

    void EndFrame(ImDrawData* data) override
    {
        LLGL::Vulkan::CommandBufferNativeHandle nativeContextHandle = {};
        cmdBuffer->GetNativeHandle(&nativeContextHandle, sizeof(nativeContextHandle));

        ImGui_ImplVulkan_RenderDrawData(data, nativeContextHandle.commandBuffer);
    }
};

REGISTER_BACKEND(VulkanBackend, "Vulkan");
