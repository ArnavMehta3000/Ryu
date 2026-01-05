#include "Graphics/SceneRenderer.h"
#include "Asset/AssetRegistry.h"
#include "Asset/IGpuResourceFactory.h"
#include "Graphics/CommonStates.h"
#include "Graphics/Core/GfxCommandList.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Shader/ShaderLibrary.h"
#include <span>

namespace Ryu::Gfx
{
    namespace
    {
        // Per-object constant buffer layout
        struct /* alignas(256) */ ObjectConstants
        {
            Math::Matrix World;
            Math::Matrix WorldViewProjection;
        };

        // Per-frame constant buffer layout
        struct /* alignas(256) */ FrameConstants
        {
            Math::Matrix View;
            Math::Matrix Projection;
            Math::Matrix ViewProjection;
            Math::Vector4 CameraPosition;  // xyz = position, w = unused
            Math::Vector4 Time;            // x = delta, y = total, zw = unused
        };
    }

    SceneRenderer::SceneRenderer(Device* device, ShaderLibrary* shaderLib, Asset::AssetRegistry* registry, const Config& config)
        : m_device(device)
        , m_shaderLib(shaderLib)
        , m_assetRegistry(registry)
        , m_config(config)
        , m_cbPool(device, FRAME_BUFFER_COUNT)
    {
        CreateResources();

        m_defaultCamera.Position    = Math::Vector3(0.0f, 0.0f, -15.0f);
        m_defaultCamera.Forward     = Math::Vector3(0.0f, 0.0f, 1.0f);
        m_defaultCamera.NearPlane   = 0.1f;
        m_defaultCamera.FarPlane    = 1000.0f;
        m_defaultCamera.CullingMask = 0xFFFFFFFF;
        m_defaultCamera.Priority    = 0;

        // Will be set properly on first resize
        m_defaultCamera.ViewportX      = 0;
        m_defaultCamera.ViewportY      = 0;
        m_defaultCamera.ViewportWidth  = m_screenWidth;
        m_defaultCamera.ViewportHeight = m_screenHeight;
    }

    void SceneRenderer::RenderFrame(const Gfx::RenderFrame& frame, Asset::IGpuResourceFactory* gpuFactory)
    {
        BeginFrame();

        if (gpuFactory)
        {
            gpuFactory->ProcessPendingUploads(*m_device->GetGraphicsCommandList());
        }

        if (frame.Views.empty())
        {
            // No camera entities - use default camera
            Gfx::RenderView defaultView{ .CameraData = m_defaultCamera };
            RenderView(defaultView);  // Still need to collect renderables, but this should work for now
        }
        else
        {
            for (const auto& view : frame.Views)
            {
                RenderView(view);
            }
        }

        EndFrame();
    }

    void SceneRenderer::RenderView(const Gfx::RenderView& view)
    {
        CommandList* cmdList = m_device->GetGraphicsCommandList();

        // Set viewport and scissor based on camera
        const CD3DX12_VIEWPORT viewport(
            static_cast<f32>(view.CameraData.ViewportX),
            static_cast<f32>(view.CameraData.ViewportY),
            static_cast<f32>(view.CameraData.ViewportWidth),
            static_cast<f32>(view.CameraData.ViewportHeight),
            0.0f, 1.0f);

        const CD3DX12_RECT scissor(
            view.CameraData.ViewportX,
            view.CameraData.ViewportY,
            view.CameraData.ViewportX + view.CameraData.ViewportWidth,
            view.CameraData.ViewportY + view.CameraData.ViewportHeight);

        cmdList->SetViewports(
            std::span<const CD3DX12_VIEWPORT>(&viewport, 1),
            std::span<const CD3DX12_RECT>(&scissor, 1));

        RYU_TODO("Handle passing in timing info")
        BindPerFrameData(view.CameraData, 0.0f, 0.0f);
        RenderOpaquePass(view);
        RenderTransparentPass(view);
    }

    void SceneRenderer::SetDefaultCamera(const CameraData& camera)
    {
        m_defaultCamera = camera;
    }

    void SceneRenderer::SetConfig(const Config& config)
    {
        // Add more conditions for pipeline recreation
        bool needsPipelineRecreation = (config.EnableWireframe != m_config.EnableWireframe);
        m_config = config;

        if (needsPipelineRecreation)
        {
            CreatePipelineState();
        }
    }

    void SceneRenderer::CreateResources()
    {
        // Create CBV heap for per-object and per-frame constants
        u32 totalDescriptors = m_config.MaxConstantBuffers + 1; // +1 for frame CB
        m_cbvHeap = std::make_unique<DescriptorHeap>(
            m_device,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            totalDescriptors,
            true,
            "Scene CBV Heap");

        CreatePipelineState();
    }

    void SceneRenderer::CreatePipelineState()
    {
        RYU_TODO("Stop hard loading shaders and architect an alternative")

        // Compile shaders & create root signature
        Shader* vs = m_shaderLib->GetShader("MeshVS");
        Shader* ps = m_shaderLib->GetShader("MeshPS");
        
        RYU_ASSERT(vs->IsValid(), "Failed to compile vertex shader!");
        RYU_ASSERT(ps->IsValid(), "Failed to compile pixel shader!");

        m_rootSignature = std::make_unique<RootSignature>(m_device, vs->GetRootSignature(), "Root Signature");

        const D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        Shader::Blob* const vsBlob = vs->GetBlob();
        Shader::Blob* const psBlob = ps->GetBlob();

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
        psoStream.RootSignature = m_rootSignature->GetNative();
        psoStream.InputLayout = { inputElementDescs, static_cast<u32>(std::size(inputElementDescs)) };
        psoStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        psoStream.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());
        psoStream.Rasterizer = CommonStates::RSCullCounterClockwise();
        psoStream.BlendDesc = CommonStates::BSOpaque();
        psoStream.RTVFormats =
        {
            .RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
            .NumRenderTargets = 1
        };

        const D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc
        {
            .SizeInBytes = sizeof(PipelineStateStream),
            .pPipelineStateSubobjectStream = &psoStream
        };

        m_opaquePipeline = std::make_unique<PipelineState>(m_device, psoStreamDesc, "Opaque Pipeline State");
    }

    void SceneRenderer::BeginFrame()
    {
        m_cbPool.ResetFrame(m_currentFrame);

        CommandList* cmdList = m_device->GetGraphicsCommandList();
        const Texture* renderTarget = m_device->GetCurrentBackBuffer();

        m_device->BeginFrame(m_opaquePipeline.get());

        cmdList->SetGraphicsRootSignature(*m_rootSignature);
        cmdList->SetDescriptorHeap(*m_cbvHeap);

        cmdList->TransitionResource(*renderTarget,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);

        m_device->SetBackBufferRenderTarget(true);
    }

    void SceneRenderer::EndFrame()
    {
        m_currentFrame++;
        m_cbPool.Reclaim(m_currentFrame);

        CommandList* cmdList = m_device->GetGraphicsCommandList();
        const Texture* renderTarget = m_device->GetCurrentBackBuffer();

        cmdList->TransitionResource(*renderTarget,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);

        m_device->EndFrame();
        m_device->Present();
    }

    void SceneRenderer::RenderOpaquePass(const Gfx::RenderView& view)
    {
        CommandList* cmdList = m_device->GetGraphicsCommandList();
        // Currently GfxDevice::Begin takes in a pipeline and passes it to the commandlist (to be set during reset)
        // Either There has to be a better way to pass the pipeline state
        // Or we chose to not set any initial state when starting the commandlist
        RYU_TODO("Fix the pipeline state setting stuff")
        cmdList->GetNative()->SetPipelineState(*m_opaquePipeline);

        for (const auto& item : view.OpaqueItems)
        {
            DrawRenderItem(item, view.CameraData);
        }
    }

    void SceneRenderer::RenderTransparentPass(const Gfx::RenderView& view)
    {
        if (view.TransparentItems.empty())
        {
            return;
        }

        CommandList* cmdList = m_device->GetGraphicsCommandList();
        cmdList->GetNative()->SetPipelineState(*m_transparentPipeline);

        for (const auto& item : view.TransparentItems)
        {
            DrawRenderItem(item, view.CameraData);
        }
    }

    void SceneRenderer::BindPerFrameData(const CameraData& camera, f32 deltaTime, f32 totalTime)
    {
        Buffer* frameCB = m_cbPool.Acquire(sizeof(FrameConstants), m_currentFrame, "FrameCB");

        FrameConstants data
        {
            .View           = camera.ViewMatrix,
            .Projection     = camera.ProjectionMatrix,
            .ViewProjection = camera.ViewProjectionMatrix,
            .CameraPosition = { camera.Position.x, camera.Position.y, camera.Position.z, 1.0f },
            .Time           = { deltaTime, totalTime, 0, 0 }
        };

        m_cbPool.UpdateBuffer(frameCB, &data, sizeof(FrameConstants));

        CommandList* cmdList = m_device->GetGraphicsCommandList();
        cmdList->SetGraphicsConstantBuffer(0, *frameCB);
    }

    void SceneRenderer::DrawRenderItem(const RenderItem& item, const CameraData& camera)
    {
        Mesh* mesh = m_assetRegistry->Meshes().GetGpu(item.MeshHandle);
        if (!mesh)
        {
            return;
        }

        ObjectConstants objData
        {
            .World               = item.WorldTransform,
            .WorldViewProjection = item.WorldTransform * camera.ViewProjectionMatrix
        };

        ScopedConstantBuffer objCB(&m_cbPool, m_cbPool.Acquire(sizeof(ObjectConstants), m_currentFrame, "ObjectCB"));
        m_cbPool.UpdateBuffer(objCB.Get(), &objData, sizeof(ObjectConstants));

        CommandList* cmdList = m_device->GetGraphicsCommandList();

        cmdList->SetGraphicsConstantBuffer(1, *objCB);

        // Bind mesh and draw
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
    
    void SceneRenderer::OnResize(u32 width, u32 height)
    {
        m_screenWidth = width;
        m_screenHeight = height;
        m_device->ResizeBuffers(width, height);

        // Update default camera viewport
        m_defaultCamera.ViewportWidth = width;
        m_defaultCamera.ViewportHeight = height;
    }
}
