/*
* LLGL Example ImGui
* Created on 02/22/2025 by L.Hermanns
* Published under the BSD-3 Clause License
* ----------------------------------------
* Direct3D 12 Backend
*/

#include "../Backend.h"
#include "../../Globals.h"

#include <LLGL/LLGL.h>
#include <LLGL/Platform/NativeHandle.h>
#include <LLGL/Backend/Direct3D12/NativeHandle.h>
#include <LLGL/Trap.h>

#include "imgui.h"
#include "imgui_impl_dx12.h"

static DXGI_FORMAT GetRTVFormat(LLGL::Format format)
{
    switch (format)
    {
        case LLGL::Format::RGBA8UNorm:  return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:                        return DXGI_FORMAT_UNKNOWN;
    }
}

static DXGI_FORMAT GetDSVFormat(LLGL::Format format)
{
    switch (format)
    {
        case LLGL::Format::D16UNorm:            return DXGI_FORMAT_D16_UNORM;
        case LLGL::Format::D24UNormS8UInt:      return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case LLGL::Format::D32Float:            return DXGI_FORMAT_D32_FLOAT;
        case LLGL::Format::D32FloatS8X24UInt:   return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        default:                                return DXGI_FORMAT_UNKNOWN;
    }
}

#define SAFE_RELEASE(OBJ)   \
    if ((OBJ) != nullptr)   \
    {                       \
        (OBJ)->Release();   \
        OBJ = nullptr;      \
    }

// Helper class to allocate D3D12 descriptors
class D3D12DescriptorHeapAllocator
{
    ID3D12DescriptorHeap*       d3dHeap         = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE d3dGPUHandle;
    UINT                        d3dHandleSize   = 0;
    std::vector<UINT>           freeIndices;

public:
    D3D12DescriptorHeapAllocator(ID3D12Device* d3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors)
    {
        // Create D3D12 descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC d3dSRVDescriptorHeapDesc = {};
        {
            d3dSRVDescriptorHeapDesc.Type           = type;
            d3dSRVDescriptorHeapDesc.NumDescriptors = numDescriptors;
            d3dSRVDescriptorHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            d3dSRVDescriptorHeapDesc.NodeMask       = 0;
        }
        HRESULT result = d3dDevice->CreateDescriptorHeap(&d3dSRVDescriptorHeapDesc, IID_PPV_ARGS(&d3dHeap));
        LLGL_VERIFY(SUCCEEDED(result));

        d3dCPUHandle = d3dHeap->GetCPUDescriptorHandleForHeapStart();
        d3dGPUHandle = d3dHeap->GetGPUDescriptorHandleForHeapStart();
        d3dHandleSize = d3dDevice->GetDescriptorHandleIncrementSize(type);

        // Initialize free indices
        freeIndices.reserve(numDescriptors);
        for (UINT n = numDescriptors; n > 0; --n)
            freeIndices.push_back(n - 1);
    }

    ~D3D12DescriptorHeapAllocator()
    {
        SAFE_RELEASE(d3dHeap);
    }

    void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE& outCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE& outGPUHandle)
    {
        LLGL_VERIFY(!freeIndices.empty());

        UINT index = freeIndices.back();
        freeIndices.pop_back();

        outCPUHandle.ptr = d3dCPUHandle.ptr + (index * d3dHandleSize);
        outGPUHandle.ptr = d3dGPUHandle.ptr + (index * d3dHandleSize);
    }

    void Free(D3D12_CPU_DESCRIPTOR_HANDLE inCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE inGPUHandle)
    {
        const UINT cpuIndex = static_cast<UINT>((inCPUHandle.ptr - d3dCPUHandle.ptr) / d3dHandleSize);
        const UINT gpuIndex = static_cast<UINT>((inGPUHandle.ptr - d3dGPUHandle.ptr) / d3dHandleSize);
        LLGL_VERIFY(cpuIndex == gpuIndex);
        freeIndices.push_back(cpuIndex);
    }

    ID3D12DescriptorHeap* GetNative() const
    {
        return d3dHeap;
    }

};

using D3D12DescriptorHeapAllocatorPtr = std::unique_ptr<D3D12DescriptorHeapAllocator>;

static D3D12DescriptorHeapAllocatorPtr g_heapAllocator;

class Direct3D12Backend final : public Backend
{
    // Global variables for the Direct3D 12 backend
    ID3D12Device*               d3dDevice               = nullptr;
    ID3D12CommandQueue*         d3dCommandQueue         = nullptr;
    ID3D12GraphicsCommandList*  d3dCommandList          = nullptr;

public:

    Direct3D12Backend()
    {
        CreateResources(
            "Direct3D12",

            // Vertex shader
            "Direct3D12SceneShader.hlsl",
            "VSMain",
            "vs_6_0",

            // Pixel shader
            "Direct3D12SceneShader.hlsl",
            "PSMain",
            "ps_6_0"
        );

        // Create SRV descriptor heap for ImGui's internal resources
        LLGL::Direct3D12::RenderSystemNativeHandle nativeDeviceHandle;
        renderer->GetNativeHandle(&nativeDeviceHandle, sizeof(nativeDeviceHandle));
        d3dDevice = nativeDeviceHandle.device;
        d3dCommandQueue = nativeDeviceHandle.commandQueue;

        g_heapAllocator = D3D12DescriptorHeapAllocatorPtr(new D3D12DescriptorHeapAllocator{ d3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 64 });
    }

    ~Direct3D12Backend()
    {
        // Release D3D handles
        g_heapAllocator.reset();
        SAFE_RELEASE(d3dCommandList);
        SAFE_RELEASE(d3dCommandQueue);
        SAFE_RELEASE(d3dDevice);
    }

    void InitContext(WindowContext& context) override
    {
        Backend::InitContext(context);

        // Setup renderer backend
        LLGL::Direct3D12::CommandBufferNativeHandle nativeContextHandle;
        cmdBuffer->GetNativeHandle(&nativeContextHandle, sizeof(nativeContextHandle));
        d3dCommandList = nativeContextHandle.commandList;

        // Initialize ImGui D3D12 backend
        ImGui_ImplDX12_InitInfo imGuiInfo = {};
        {
            imGuiInfo.Device                = d3dDevice;
            imGuiInfo.CommandQueue          = d3dCommandQueue;
            imGuiInfo.NumFramesInFlight     = 2;
            imGuiInfo.RTVFormat             = GetRTVFormat(g_swapChains.front()->GetColorFormat());
            imGuiInfo.DSVFormat             = GetDSVFormat(g_swapChains.front()->GetDepthStencilFormat());
            imGuiInfo.SrvDescriptorAllocFn  = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* outCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE* outGPUDescHandle)
                {
                    g_heapAllocator->Alloc(*outCPUDescHandle, *outGPUDescHandle);
                };
            imGuiInfo.SrvDescriptorFreeFn   = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE inCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE inGPUDescHandle)
                {
                    g_heapAllocator->Free(inCPUDescHandle, inGPUDescHandle);
                };
        }
        ImGui_ImplDX12_Init(&imGuiInfo);
    }

    void ReleaseContext(WindowContext& context) override
    {
        ImGui::SetCurrentContext(context.imGuiContext);

        ImGui_ImplDX12_Shutdown();

        Backend::ReleaseContext(context);
    }

    void BeginFrame(WindowContext& context) override
    {
        Backend::BeginFrame(context);

        ImGui_ImplDX12_NewFrame();
    }

    void EndFrame(ImDrawData* data) override
    {
        ID3D12DescriptorHeap* d3dHeap = g_heapAllocator->GetNative();
        d3dCommandList->SetDescriptorHeaps(1, &d3dHeap);

        ImGui_ImplDX12_RenderDrawData(data, d3dCommandList);
    }
};

REGISTER_BACKEND(Direct3D12Backend, "Direct3D12");
