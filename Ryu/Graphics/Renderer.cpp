#include "Graphics/Renderer.h"

#include "Core/Profiling/Profiling.h"
#include "Core/Utils/Timing/FrameTimer.h"
#include "Game/World/World.h"
#include "Graphics/CommonStates.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/IRendererHook.h"

namespace Ryu::Gfx
{
    Renderer::Renderer(HWND window, IRendererHook* hook)
        : m_device(std::make_unique<Device>(window))
        , m_gpuFactory(std::make_unique<GpuResourceFactory>(m_device.get()))
        , m_assets(std::make_unique<Asset::AssetRegistry>(m_gpuFactory.get()))
        , m_renderWorld(m_assets.get())
        , m_shaderLibrary("./Shaders/Compiled"sv)
        , m_sceneRenderer(m_device.get(), &m_shaderLibrary, m_assets.get())
        , m_hook(hook)
    {
        RYU_PROFILE_SCOPE();

        m_device->Initialize();

        const auto [w, h] = m_device->GetClientSize();
        m_sceneRenderer.OnResize(w, h);  // Force a test resize
        m_camera.SetViewportSize((f32)w, (f32)h);
        m_camera.SetPosition(Math::Vector3(0.0f, 0.0f, -15.0f));

        CreateResources();

#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiSetup(m_device.get());
        }
#endif
    }

    Renderer::~Renderer()
    {
#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiShutdown();
        }
#endif
    }

    void Renderer::CreateResources()
    {
        RYU_PROFILE_SCOPE();

        //CompileShaders();
        //CreateConstantBuffer();
        //CreatePipelineState();
    }

    void Renderer::CompileShaders()
    {
        RYU_PROFILE_SCOPE();

        if (Shader* vs = m_shaderLibrary.GetShader("MeshVS"))
        {
            m_vs = vs;
        }

        if (Shader* ps = m_shaderLibrary.GetShader("MeshPS"))
        {
            m_ps = ps;
        }

        RYU_ASSERT(m_vs->IsValid(), "Failed to compile vertex shader!");
        RYU_ASSERT(m_ps->IsValid(), "Failed to compile pixel shader!");

        m_rootSignature = std::make_unique<RootSignature>(m_device.get(), m_vs->GetRootSignature(), "Root Signature");
    }

    void Renderer::CreatePipelineState()
    {
        RYU_PROFILE_SCOPE();

        const D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        Shader::Blob* const vsBlob = m_vs->GetBlob();
        Shader::Blob* const psBlob = m_ps->GetBlob();

        struct PipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
            CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
        };

        PipelineStateStream psoStream{};
        psoStream.RootSignature     = m_rootSignature->GetNative();
        psoStream.InputLayout       = { inputElementDescs, static_cast<u32>(std::size(inputElementDescs)) };
        psoStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoStream.VS                = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        psoStream.PS                = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());
        psoStream.Rasterizer        = CommonStates::RSCullCounterClockwise();
        psoStream.BlendDesc         = CommonStates::BSOpaque();
        psoStream.RTVFormats        =
        {
            .RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
            .NumRenderTargets = 1
        };

        const D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc
        {
            .SizeInBytes = sizeof(PipelineStateStream),
            .pPipelineStateSubobjectStream = &psoStream
        };

        m_pipelineState = std::make_unique<PipelineState>(m_device.get(), psoStreamDesc, "Graphics Pipeline State");
    }

    void Renderer::CreateConstantBuffer()
    {
        RYU_PROFILE_SCOPE();

        m_cbvHeap = std::make_unique<DescriptorHeap>(
            m_device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 
            u32(Asset::PrimitiveType::MAX_COUNT), true, "Frame CBV Heap");

        for (size_t i = 0; i < m_perObjectCBs.size(); ++i)
        {
            DescriptorHandle handle = m_cbvHeap->Allocate();

            const Buffer::Desc cbDesc
            {
                .SizeInBytes = sizeof(ConstantBuffer),
                .Usage       = Buffer::Usage::Upload,
                .Type        = Buffer::Type::Constant,
                .Name        = "Object CB " + std::to_string(i)
            };

            m_perObjectCBs[i] = std::make_unique<Buffer>(m_device.get(), cbDesc, handle);
        }
    }

    static f32 t = 0;

    void Renderer::Render()
    {
        RYU_PROFILE_SCOPE();

        m_camera.Update(0.1f);        

        CommandList* cmdList = m_device->GetGraphicsCommandList();
        const Texture* renderTarget = m_device->GetCurrentBackBuffer();

        m_device->BeginFrame(m_pipelineState.get());

        // Process pending GPU uploads (meshes, textures)
        m_gpuFactory->ProcessPendingUploads(*cmdList);

        cmdList->SetGraphicsRootSignature(*m_rootSignature);
        cmdList->SetDescriptorHeap(*m_cbvHeap);

        cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_device->SetBackBufferRenderTarget(true);

        t += 0.1f;
        const f32 rad = DirectX::XMConvertToRadians(t);
        Math::Matrix rotation = Math::Matrix::CreateRotationX(rad)
            * Math::Matrix::CreateRotationY(rad)
            * Math::Matrix::CreateRotationZ(rad);

        constexpr f32 spacing = 4.0f;
        const f32 startX = -spacing * (static_cast<f32>(Asset::PrimitiveType::MAX_COUNT) - 1) * 0.5f;

        for (u8 i = 0; i < static_cast<u8>(Asset::PrimitiveType::MAX_COUNT); ++i)
        {
            auto type = static_cast<Asset::PrimitiveType>(i);

            if (Mesh* mesh = m_assets->GetPrimitiveGpu(type))
            {
                Math::Matrix translation = Math::Matrix::CreateTranslation(startX + i * spacing, 0.0f, 0.0f);
                m_cbData.WVP = rotation * translation * m_camera.GetViewProjectionMatrix();

                if (ConstantBuffer* mapped = m_perObjectCBs[i]->Map<ConstantBuffer>())
                {
                    std::memcpy(mapped, &m_cbData, sizeof(ConstantBuffer));
                }

                cmdList->SetGraphicsConstantBuffer(0, *m_perObjectCBs[i]);
                mesh->SetPipelineBuffers(*cmdList, 0);

                if (mesh->HasIndexBuffer())
                {
                    cmdList->DrawMeshIndexedInstanced(*mesh);
                }
                else
                {
                    cmdList->DrawMeshInstanced(*mesh);
                }
            }
        }

#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiFrameBegin();
            m_hook->OnImGuiRender();
            m_hook->OnImGuiFrameEnd(cmdList);
        }
#endif

        cmdList->TransitionResource(*renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        m_device->EndFrame();
        m_device->Present();
    }

    void Renderer::RenderWorld(Game::World& world, const Utils::FrameTimer& frameTimer)
    {
        //m_gpuFactory->ProcessPendingUploads(*m_device->GetGraphicsCommandList());

        const Gfx::RenderFrame frameData = m_renderWorld.ExtractRenderData(
            world,
            frameTimer.DeltaTimeF(),
            static_cast<f32>(frameTimer.TimeSinceStart<std::chrono::seconds>()),
            frameTimer.FrameCount());

        m_sceneRenderer.RenderFrame(frameData, m_gpuFactory.get());
    }

    void Renderer::OnResize(u32 w, u32 h)
    {
        RYU_PROFILE_SCOPE();
        //m_device->ResizeBuffers(w, h);
        m_camera.SetViewportSize(static_cast<f32>(w), static_cast<f32>(h));
        m_sceneRenderer.OnResize(w, h);  // This will resize the device;
    }
}
