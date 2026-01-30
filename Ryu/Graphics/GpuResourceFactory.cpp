#include "Graphics/GpuResourceFactory.h"
#include "Core/Profiling/Profiling.h"

namespace Ryu::Gfx
{
    GpuResourceFactory::GpuResourceFactory(Device* device)
        : m_device(device) {}

    std::unique_ptr<Gfx::Mesh> GpuResourceFactory::CreateMesh(const Asset::MeshData& data, std::string_view name)
    {
        RYU_PROFILE_SCOPE();
        //if (data.Vertices.empty())
        //{
        //    return nullptr;
        //}

        //// Vertex buffer
        //Buffer::Desc vbDesc
        //{
        //    .SizeInBytes   = data.VertexBufferSize(),
        //    .StrideInBytes = data.VertexStride(),
        //    .Usage         = Buffer::Usage::Default,
        //    .Type          = Buffer::Type::Vertex,
        //    .Name          = std::string(name) + " VB"
        //};

        //// Index buffer (optional)
        //std::unique_ptr<Buffer::Desc> ibDesc;
        //if (data.HasIndices())
        //{
        //    ibDesc = std::make_unique<Buffer::Desc>(Buffer::Desc
        //    {
        //        .SizeInBytes = data.IndexBufferSize(),
        //        .Usage       = Buffer::Usage::Default,
        //        .Type        = Buffer::Type::Index,
        //        .Format      = DXGI_FORMAT_R32_UINT,
        //        .Name        = std::string(name) + " IB"
        //    });
        //}

        //auto mesh = std::make_unique<Mesh>(m_device, vbDesc, ibDesc.get());

        //Mesh::DrawInfo drawInfo
        //{
        //    .Topology               = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        //    .VertexCountPerInstance = static_cast<u32>(data.Vertices.size()),
        //    .IndexCountPerInstance  = static_cast<u32>(data.Indices.size()),
        //    .InstanceCount          = 1
        //};
        //mesh->SetDrawInfo(drawInfo);

        //m_pendingMeshUploads.push(
        //{
        //    mesh.get(),
        //    data.Vertices,
        //    data.Indices
        //});

        //return mesh;

        return nullptr;
    }

    std::unique_ptr<Gfx::Texture> GpuResourceFactory::CreateTexture(const Asset::TextureData& data, std::string_view name)
    {
        RYU_PROFILE_SCOPE();
        /*if (data.Data.empty())
        {
            return nullptr;
        }

        DXGI_FORMAT format = ConvertFormat(data.PixelFormat);

        auto texture = std::make_unique<Texture>(m_device, data.Width, data.Height, format, name);

        m_pendingTextureUploads.push(
        {
            texture.get(),
            data.Data,
            static_cast<u32>(data.RowPitch())
        });

        return texture;*/

        return nullptr;
    }

    void GpuResourceFactory::ProcessPendingUploads(Gfx::CommandList& cmdList)
    {
        RYU_PROFILE_SCOPE();

        //while (!m_pendingMeshUploads.empty())
        //{
        //    PendingMeshUpload& upload = m_pendingMeshUploads.front();

        //    upload.Mesh->UploadBufferData(
        //        cmdList,
        //        const_cast<void*>(static_cast<const void*>(upload.Vertices.data())),
        //        upload.Indices.empty() ? nullptr : const_cast<void*>(static_cast<const void*>(upload.Indices.data()))
        //    );

        //    m_pendingMeshUploads.pop();
        //}

        //// Upload textures
        //while (!m_pendingTextureUploads.empty())
        //{
        //    PendingTextureUpload& upload = m_pendingTextureUploads.front();

        //    upload.Texture->Upload(cmdList, upload.Data.data(), upload.RowPitch);

        //    m_pendingTextureUploads.pop();
        //}
    }
}
