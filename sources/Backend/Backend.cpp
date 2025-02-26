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
#include <cstring>


using BackendRegisterMap = std::map<std::string, Backend::AllocateBackendFunc>;

Backend::~Backend()
{
    input.Drop(swapChain->GetSurface());
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
    lastTick = LLGL::Timer::Tick();
}

void Backend::Release()
{
    PlatformShutdown();

    ImGui::DestroyContext();
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
    constexpr std::uint32_t col0 = 0xFFFFFFFF;
    constexpr std::uint32_t col1 = 0xFFFFFFFF;
    constexpr std::uint32_t col2 = 0xFFFFFFFF;
    constexpr std::uint32_t col3 = 0xFFFFFFFF;

    return
    {
        // front
        Vertex{ { -1, -1, -1 }, {  0,  0, -1 }, col0 },
        Vertex{ { -1,  1, -1 }, {  0,  0, -1 }, col1 },
        Vertex{ {  1,  1, -1 }, {  0,  0, -1 }, col2 },
        Vertex{ {  1, -1, -1 }, {  0,  0, -1 }, col3 },

        // right
        Vertex{ {  1, -1, -1 }, { +1,  0,  0 }, col0 },
        Vertex{ {  1,  1, -1 }, { +1,  0,  0 }, col1 },
        Vertex{ {  1,  1,  1 }, { +1,  0,  0 }, col2 },
        Vertex{ {  1, -1,  1 }, { +1,  0,  0 }, col3 },

        // left
        Vertex{ { -1, -1,  1 }, { -1,  0,  0 }, col0 },
        Vertex{ { -1,  1,  1 }, { -1,  0,  0 }, col1 },
        Vertex{ { -1,  1, -1 }, { -1,  0,  0 }, col2 },
        Vertex{ { -1, -1, -1 }, { -1,  0,  0 }, col3 },

        // top
        Vertex{ { -1,  1, -1 }, {  0, +1,  0 }, col0 },
        Vertex{ { -1,  1,  1 }, {  0, +1,  0 }, col1 },
        Vertex{ {  1,  1,  1 }, {  0, +1,  0 }, col2 },
        Vertex{ {  1,  1, -1 }, {  0, +1,  0 }, col3 },

        // bottom
        Vertex{ { -1, -1,  1 }, {  0, -1,  0 }, col0 },
        Vertex{ { -1, -1, -1 }, {  0, -1,  0 }, col1 },
        Vertex{ {  1, -1, -1 }, {  0, -1,  0 }, col2 },
        Vertex{ {  1, -1,  1 }, {  0, -1,  0 }, col3 },

        // back
        Vertex{ {  1, -1,  1 }, {  0,  0, +1 }, col0 },
        Vertex{ {  1,  1,  1 }, {  0,  0, +1 }, col1 },
        Vertex{ { -1,  1,  1 }, {  0,  0, +1 }, col2 },
        Vertex{ { -1, -1,  1 }, {  0,  0, +1 }, col3 },
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

class WindowEventListener final : public LLGL::Window::EventListener
{
    Backend* backend = nullptr;

public:
    WindowEventListener(Backend* inBackend) :
        backend { inBackend }
    {
    }

    void OnResize(LLGL::Window& sender, const LLGL::Extent2D& clientAreaSize) override
    {
        if (cmdBuffer != nullptr)
        {
            backend->OnResizeSurface(clientAreaSize);
            backend->RenderScene();
            swapChain->Present();
        }
    }

    void OnUpdate(LLGL::Window& /*sender*/) override
    {
        if (cmdBuffer != nullptr)
        {
            backend->RenderScene();
            swapChain->Present();
        }
    }
};

void Backend::OnResizeSurface(const LLGL::Extent2D& size)
{
    swapChain->ResizeBuffers(size);
    const float aspectRatio = static_cast<float>(size.width) / static_cast<float>(size.height);
    scene.ViewProjection(aspectRatio);
}

static LLGL::ShaderSourceType GetShaderSourceType(const char* filename)
{
    const std::size_t filenameLen = std::strlen(filename);
    if (filenameLen > 4 && std::strcmp(filename + filenameLen - 4, ".spv") == 0)
        return LLGL::ShaderSourceType::BinaryFile;
    else
        return LLGL::ShaderSourceType::CodeFile;
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

    const LLGL::RendererInfo& info = renderer->GetRendererInfo();

    #ifdef LLGL_OS_MACOS
    constexpr unsigned resX = 1280*2;
    constexpr unsigned resY = 768*2;
    #else
    constexpr unsigned resX = 1280;
    constexpr unsigned resY = 768;
    #endif

    // Create swap chain
    LLGL::SwapChainDescriptor swapChainDesc;
    {
        swapChainDesc.resolution    = { resX, resY };
        swapChainDesc.resizable     = true;
    }
    swapChain = renderer->CreateSwapChain(swapChainDesc);

    if (scene.showcase.isVsync)
        swapChain->SetVsyncInterval(1);

    // Register callback to update swap-chain on window resize
    LLGL::Window& window = LLGL::CastTo<LLGL::Window>(swapChain->GetSurface());

    window.AddEventListener(std::make_shared<WindowEventListener>(this));
    window.SetTitle(LLGL::UTF8String::Printf("LLGL/ImGui Example ( %s )", info.rendererName.c_str()));

    // Create command buffer with immediate context
    cmdBuffer = renderer->CreateCommandBuffer(LLGL::CommandBufferFlags::ImmediateSubmit);

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
        vertShaderDesc.debugName            = "Shader.Vert";
        vertShaderDesc.type                 = LLGL::ShaderType::Vertex;
        vertShaderDesc.source               = vertShaderPath.c_str();
        vertShaderDesc.sourceType           = GetShaderSourceType(vertShaderFilename);
        vertShaderDesc.entryPoint           = vertShaderEntry;
        vertShaderDesc.profile              = vertShaderProfile;
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
        fragShaderDesc.sourceType   = GetShaderSourceType(fragShaderFilename);
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
        psoDesc.debugName                       = "Graphics.PSO";
        psoDesc.pipelineLayout                  = psoLayout;
        psoDesc.vertexShader                    = vertShader;
        psoDesc.fragmentShader                  = fragShader;
        psoDesc.indexFormat                     = LLGL::Format::R16UInt;
        psoDesc.primitiveTopology               = LLGL::PrimitiveTopology::TriangleList;
        psoDesc.depth.testEnabled               = true;
        psoDesc.depth.writeEnabled              = true;
        psoDesc.rasterizer.cullMode             = LLGL::CullMode::Back;
        psoDesc.blend.targets[0].blendEnabled   = true;
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

void NormalizeVector3(float* v)
{
    const float vecLen = std::sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= vecLen;
    v[1] /= vecLen;
    v[2] /= vecLen;
}

static void ShowImGuiElements(float dt)
{
    // Show ImGui's demo window
    ImGui::Begin("LLGL/ImGui Example");
    {
        ImGui::SeparatorText("Video");
        {
            ImGui::Text("Frame Rate: %.3f ms (%.1f FPS)", dt * 1000.0f, 1.0f / dt);

            ImGui::Checkbox("Vsync Interval", &scene.showcase.isVsync);
        }
        ImGui::SeparatorText("Light");
        {
            if (ImGui::SliderFloat3("Light Vector", scene.view.lightVector, -1.0f, +1.0f))
                NormalizeVector3(scene.view.lightVector);
        }
        ImGui::SeparatorText("Scene");
        {
            ImGui::SliderFloat("Model Distance", &scene.view.wMatrix[3][2], 3.0f, 25.0f);

            ImGui::Combo("Rotation Mode", &scene.showcase.rotateMode, "Auto\0Manual\0\0");

            switch (scene.showcase.rotateMode)
            {
            case 0:
                if (ImGui::SliderFloat("Rotation Speed", &scene.showcase.rotateSpeed, -1.0f, +1.0f))
                {
                    if (std::abs(scene.showcase.rotateSpeed) < 0.05f)
                        scene.showcase.rotateSpeed = 0.0f;
                }
                break;

            case 1:
                ImGui::SliderFloat("Rotation Angle", &scene.showcase.rotation, 0.0f, M_PI*2.0f);
                break;
            }
        }
        ImGui::SeparatorText("Color");
        {
            ImGui::ColorPicker4("Model Color", scene.view.modelColor);
        }
    }
    ImGui::End();
}

void Backend::RenderScene()
{
    const float backgroundColor[4] = { 0.2f, 0.2f, 0.4f, 1.0f };

    // Measure elapsed time between frames for smooth animations
    std::uint64_t newTick = LLGL::Timer::Tick();
    const float deltaTime = static_cast<float>(static_cast<double>(newTick - lastTick) / static_cast<double>(LLGL::Timer::Frequency()));

    const bool wasVsyncEnabled = scene.showcase.isVsync;

    cmdBuffer->Begin();
    {
        // Upload new view data to GPU
        if (scene.viewCbuffer != nullptr)
        {
            if (scene.showcase.rotateMode == 0)
            {
                scene.showcase.rotation += scene.showcase.rotateSpeed * deltaTime * 10.0f;
                if (scene.showcase.rotation > M_PI*2.0f)
                    scene.showcase.rotation -= M_PI*2.0f;
                if (scene.showcase.rotation < 0.0f)
                    scene.showcase.rotation += M_PI*2.0f;
            }
            scene.ModelRotation(1.0f, 1.0f, 1.0f, scene.showcase.rotation);

            cmdBuffer->UpdateBuffer(*scene.viewCbuffer, 0, &scene.view, sizeof(scene.view));
        }

        cmdBuffer->BeginRenderPass(*swapChain);
        {
            cmdBuffer->Clear(LLGL::ClearFlags::ColorDepth, LLGL::ClearValue{ backgroundColor });

            cmdBuffer->SetViewport(swapChain->GetResolution());

            // Render 3D scene
            if (scene.graphicsPSO != nullptr)
            {
                cmdBuffer->PushDebugGroup("RenderScene");
                {
                    cmdBuffer->SetPipelineState(*scene.graphicsPSO);
                    cmdBuffer->SetVertexBuffer(*scene.vertexBuffer);
                    cmdBuffer->SetIndexBuffer(*scene.indexBuffer);
                    cmdBuffer->SetResource(0, *scene.viewCbuffer);
                    cmdBuffer->DrawIndexed(scene.numIndices, 0);
                }
                cmdBuffer->PopDebugGroup();
            }

#if WITH_IMGUI
            // GUI Rendering with ImGui library
            cmdBuffer->PushDebugGroup("RenderGUI");
            {
                backend->BeginFrame();
                {
                    ImGui::NewFrame();
                    {
                        ShowImGuiElements(deltaTime);
                    }
                    ImGui::Render();
                }
                backend->EndFrame(ImGui::GetDrawData());
            }
            cmdBuffer->PopDebugGroup();
#endif
        }
        cmdBuffer->EndRenderPass();
    }
    cmdBuffer->End();

    swapChain->Present();

    // If v-sync setting changed, update swap-chain now, but never during command encoding
    if (wasVsyncEnabled != scene.showcase.isVsync)
        swapChain->SetVsyncInterval(scene.showcase.isVsync ? 1 : 0);

    lastTick = newTick;
}
