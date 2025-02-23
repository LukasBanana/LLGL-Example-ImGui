/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Backend.cpp
 */

#include "Backend.h"
#include "../Globals.h"
#include "../Platform/Platform.h"
#include <LLGL/Utils/TypeNames.h>
#include <LLGL/Utils/Parse.h>
#include <LLGL/RenderSystem.h>
#include <vector>
#include <cstdint>


using BackendRegisterMap = std::map<std::string, Backend::AllocateBackendFunc>;

Backend::~Backend()
{
    input.Drop(swapChain->GetSurface());

    PlatformShutdown();
}

static BackendRegisterMap& GetBackendRegisterMap()
{
    static BackendRegisterMap registeredBackends;
    return registeredBackends;
}

void Backend::RegisterBackend(const char* name, AllocateBackendFunc onAllocateFunc)
{
    auto& registeredBackends = GetBackendRegisterMap();
    registeredBackends[name] = onAllocateFunc;
}

void Backend::Init()
{
    PlatformInit(swapChain->GetSurface());
}

void Backend::BeginFrame()
{
    PlatformNewFrame(swapChain->GetSurface());
}

std::unique_ptr<Backend> Backend::NewBackend(const char* name)
{
    auto& registeredBackends = GetBackendRegisterMap();
    auto it = registeredBackends.find(name);
    return (it != registeredBackends.end() ? it->second() : std::unique_ptr<Backend>{});
}

struct Vertex
{
    float           position[3];
    float           normal[3];
    std::uint32_t   color;
};

static std::vector<Vertex> GenerateMeshVertices()
{
    return
    {
        // front
        Vertex{ { -1, -1, -1 }, {  0,  0, -1 }, 0xFF0000FF },
        Vertex{ { -1,  1, -1 }, {  0,  0, -1 }, 0xFF00FF00 },
        Vertex{ {  1,  1, -1 }, {  0,  0, -1 }, 0xFFFF0000 },
        Vertex{ {  1, -1, -1 }, {  0,  0, -1 }, 0xFF00FFFF },

        // right
        Vertex{ {  1, -1, -1 }, { +1,  0,  0 }, 0xFF0000FF },
        Vertex{ {  1,  1, -1 }, { +1,  0,  0 }, 0xFF00FF00 },
        Vertex{ {  1,  1,  1 }, { +1,  0,  0 }, 0xFFFF0000 },
        Vertex{ {  1, -1,  1 }, { +1,  0,  0 }, 0xFF00FFFF },

        // left
        Vertex{ { -1, -1,  1 }, { -1,  0,  0 }, 0xFF0000FF },
        Vertex{ { -1,  1,  1 }, { -1,  0,  0 }, 0xFF00FF00 },
        Vertex{ { -1,  1, -1 }, { -1,  0,  0 }, 0xFFFF0000 },
        Vertex{ { -1, -1, -1 }, { -1,  0,  0 }, 0xFF00FFFF },

        // top
        Vertex{ { -1,  1, -1 }, {  0, +1,  0 }, 0xFF0000FF },
        Vertex{ { -1,  1,  1 }, {  0, +1,  0 }, 0xFF00FF00 },
        Vertex{ {  1,  1,  1 }, {  0, +1,  0 }, 0xFFFF0000 },
        Vertex{ {  1,  1, -1 }, {  0, +1,  0 }, 0xFF00FFFF },

        // bottom
        Vertex{ { -1, -1,  1 }, {  0, -1,  0 }, 0xFF0000FF },
        Vertex{ { -1, -1, -1 }, {  0, -1,  0 }, 0xFF00FF00 },
        Vertex{ {  1, -1, -1 }, {  0, -1,  0 }, 0xFFFF0000 },
        Vertex{ {  1, -1,  1 }, {  0, -1,  0 }, 0xFF00FFFF },

        // back
        Vertex{ {  1, -1,  1 }, {  0,  0, +1 }, 0xFF0000FF },
        Vertex{ {  1,  1,  1 }, {  0,  0, +1 }, 0xFF00FF00 },
        Vertex{ { -1,  1,  1 }, {  0,  0, +1 }, 0xFFFF0000 },
        Vertex{ { -1, -1,  1 }, {  0,  0, +1 }, 0xFF00FFFF },
    };
}

static std::vector<std::uint16_t> GenerateMeshIndices()
{
    return
    {
         0,  1,  2,  0,  2,  3, // front
         4,  5,  6,  4,  6,  7, // right
         8,  9, 10,  8, 10, 11, // left
        12, 13, 14, 12, 14, 15, // top
        16, 17, 18, 16, 18, 19, // bottom
        20, 21, 22, 20, 22, 23, // back
    };
}

bool Backend::CreateResources(
    const char* moduleName,
    const char* vertShaderFilename,
    const char* vertShaderEntry,
    const char* vertShaderProfile,
    const char* fragShaderFilename,
    const char* fragShaderEntry,
    const char* fragShaderProfile)
{
    // Load render system model
    LLGL::Report report;
    LLGL::RenderSystemDescriptor rendererDesc;
    {
        rendererDesc.moduleName = moduleName;
        #if !NDEBUG
        rendererDesc.debugger   = &debugger;
        #endif
    }
    renderer = LLGL::RenderSystem::Load(rendererDesc, &report);
    if (!renderer)
    {
        LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "%s", report.GetText());
        return false;
    }

    #ifdef LLGL_OS_MACOS
    constexpr unsigned resX = 1280*2;
    constexpr unsigned resY = 768*2;
    #else
    constexpr unsigned resX = 1280;
    constexpr unsigned resY = 768;
    #endif

    LLGL::SwapChainDescriptor swapChainDesc;
    swapChainDesc.resolution = { resX, resY };
    swapChain = renderer->CreateSwapChain(swapChainDesc);

    // Enable v-sync
    swapChain->SetVsyncInterval(1);

    cmdBuffer = renderer->CreateCommandBuffer(LLGL::CommandBufferFlags::ImmediateSubmit);

    // Print renderer information
    const LLGL::RendererInfo& info = renderer->GetRendererInfo();
    const LLGL::Extent2D swapChainRes = swapChain->GetResolution();

    LLGL::Log::Printf(
        "Render System:\n"
        "  Renderer:           %s\n"
        "  Device:             %s\n"
        "  Vendor:             %s\n"
        "  Shading Language:   %s\n"
        "\n"
        "Swap-Chain:\n"
        "  Resolution:         %u x %u\n"
        "  Samples:            %u\n"
        "  ColorFormat:        %s\n"
        "  DepthStencilFormat: %s\n"
        "\n",
        info.rendererName.c_str(),
        info.deviceName.c_str(),
        info.vendorName.c_str(),
        info.shadingLanguageName.c_str(),
        swapChainRes.width,
        swapChainRes.height,
        swapChain->GetSamples(),
        LLGL::ToString(swapChain->GetColorFormat()),
        LLGL::ToString(swapChain->GetDepthStencilFormat())
    );

    // Initialize view settings
    scene.ViewProjection(static_cast<float>(resX) / static_cast<float>(resY));

    // Create scene resources
    LLGL::BufferDescriptor viewCbufferDesc;
    {
        viewCbufferDesc.debugName   = "View.Cbuffer";
        viewCbufferDesc.size        = sizeof(scene.view);
        viewCbufferDesc.bindFlags   = LLGL::BindFlags::ConstantBuffer;
    }
    scene.viewCbuffer = renderer->CreateBuffer(viewCbufferDesc, &scene.view);

    const LLGL::VertexAttribute vertexAttribs[3] =
    {
        LLGL::VertexAttribute{ "position", LLGL::Format::RGB32Float, 0, offsetof(Vertex, position), sizeof(Vertex) },
        LLGL::VertexAttribute{ "normal",   LLGL::Format::RGB32Float, 1, offsetof(Vertex, normal  ), sizeof(Vertex) },
        LLGL::VertexAttribute{ "color",    LLGL::Format::RGBA8UNorm, 2, offsetof(Vertex, color   ), sizeof(Vertex) },
    };

    auto vertices = GenerateMeshVertices();
    LLGL::BufferDescriptor vertexBufferDesc;
    {
        vertexBufferDesc.debugName      = "Scene.Vbuffer";
        vertexBufferDesc.size           = vertices.size() * sizeof(Vertex);
        vertexBufferDesc.bindFlags      = LLGL::BindFlags::VertexBuffer;
        vertexBufferDesc.vertexAttribs  = vertexAttribs;
    }
    scene.vertexBuffer = renderer->CreateBuffer(vertexBufferDesc, vertices.data());

    auto indices = GenerateMeshIndices();
    LLGL::BufferDescriptor indexBufferDesc;
    {
        indexBufferDesc.debugName       = "Scene.Ibuffer";
        indexBufferDesc.size            = indices.size() * sizeof(std::uint16_t);
        indexBufferDesc.bindFlags       = LLGL::BindFlags::IndexBuffer;
        indexBufferDesc.format          = LLGL::Format::R16UInt;
        indexBufferDesc.vertexAttribs   = vertexAttribs;
    }
    scene.indexBuffer = renderer->CreateBuffer(indexBufferDesc, indices.data());
    scene.numIndices = static_cast<std::uint32_t>(indices.size());

    // Create graphics PSO and layout
    LLGL::PipelineLayout* psoLayout = renderer->CreatePipelineLayout(
        LLGL::Parse("cbuffer(View@1):vert:frag")
    );

    const std::string shaderDir = "sources/Backend/" + std::string(moduleName) + '/';

    const std::string vertShaderPath = shaderDir + vertShaderFilename;
    LLGL::ShaderDescriptor vertShaderDesc;
    {
        vertShaderDesc.debugName    = "Shader.Vert";
        vertShaderDesc.type         = LLGL::ShaderType::Vertex;
        vertShaderDesc.source       = vertShaderPath.c_str();
        vertShaderDesc.sourceType   = LLGL::ShaderSourceType::CodeFile;
        vertShaderDesc.entryPoint   = vertShaderEntry;
        vertShaderDesc.profile      = vertShaderProfile;
        vertShaderDesc.vertex.inputAttribs  = std::vector<LLGL::VertexAttribute>{ std::begin(vertexAttribs), std::end(vertexAttribs) };
    }
    LLGL::Shader* vertShader = renderer->CreateShader(vertShaderDesc);

    if (const LLGL::Report* vertShaderReport = vertShader->GetReport())
    {
        if (vertShaderReport->HasErrors())
        {
            LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "Loading vertex shader failed:\n%s", vertShaderReport->GetText());
            return false;
        }
    }

    const std::string fragShaderPath = shaderDir + fragShaderFilename;
    LLGL::ShaderDescriptor fragShaderDesc;
    {
        fragShaderDesc.debugName    = "Shader.Frag";
        fragShaderDesc.type         = LLGL::ShaderType::Fragment;
        fragShaderDesc.source       = fragShaderPath.c_str();
        fragShaderDesc.sourceType   = LLGL::ShaderSourceType::CodeFile;
        fragShaderDesc.entryPoint   = fragShaderEntry;
        fragShaderDesc.profile      = fragShaderProfile;
    }
    LLGL::Shader* fragShader = renderer->CreateShader(fragShaderDesc);

    if (const LLGL::Report* fragShaderReport = fragShader->GetReport())
    {
        if (fragShaderReport->HasErrors())
        {
            LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "Loading fragment shader failed:\n%s", fragShaderReport->GetText());
            return false;
        }
    }

    LLGL::GraphicsPipelineDescriptor psoDesc;
    {
        psoDesc.debugName           = "Graphics.PSO";
        psoDesc.pipelineLayout      = psoLayout;
        psoDesc.vertexShader        = vertShader;
        psoDesc.fragmentShader      = fragShader;
        psoDesc.indexFormat         = LLGL::Format::R16UInt;
        psoDesc.primitiveTopology   = LLGL::PrimitiveTopology::TriangleList;
        psoDesc.depth.testEnabled   = true;
        psoDesc.depth.writeEnabled  = true;
        psoDesc.rasterizer.cullMode = LLGL::CullMode::Back;
    }
    scene.graphicsPSO = renderer->CreatePipelineState(psoDesc);

    if (const LLGL::Report* graphicsPSOReport = scene.graphicsPSO->GetReport())
    {
        if (graphicsPSOReport->HasErrors())
        {
            LLGL::Log::Errorf(LLGL::Log::ColorFlags::StdError, "%s", graphicsPSOReport->GetText());
            return false;
        }
    }

    return true;
}
