#pragma once
#include "Asset/AssetData.h"
#include "Graphics/Core/DX12.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Mesh.h"
#include "Asset/IGpuResourceFactory.h"
#include <queue>

namespace Ryu::Gfx
{
	class GpuResourceFactory : public Asset::IGpuResourceFactory
	{
	public:
        explicit GpuResourceFactory(Device* device);

        std::unique_ptr<Gfx::Mesh> CreateMesh(const Asset::MeshData& data, std::string_view name) override;
		std::unique_ptr<Gfx::Texture> CreateTexture(const Asset::TextureData& data, std::string_view name) override;
		void ProcessPendingUploads(Gfx::CommandList& cmdList) override;

    private:
        static DXGI_FORMAT ConvertFormat(Asset::TextureData::Format format)
        {
            switch (format)
            {
            case Asset::TextureData::Format::R8:      return DXGI_FORMAT_R8_UNORM;
            case Asset::TextureData::Format::RG8:     return DXGI_FORMAT_R8G8_UNORM;
            case Asset::TextureData::Format::RGBA8:   return DXGI_FORMAT_R8G8B8A8_UNORM;
            case Asset::TextureData::Format::R16F:    return DXGI_FORMAT_R16_FLOAT;
            case Asset::TextureData::Format::RG16F:   return DXGI_FORMAT_R16G16_FLOAT;
            case Asset::TextureData::Format::RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case Asset::TextureData::Format::R32F:    return DXGI_FORMAT_R32_FLOAT;
            case Asset::TextureData::Format::RG32F:   return DXGI_FORMAT_R32G32_FLOAT;
            case Asset::TextureData::Format::RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            default:                                  return DXGI_FORMAT_R8G8B8A8_UNORM;
            }
        }

    private:
        struct PendingMeshUpload
        {
            Mesh* Mesh;
            std::vector<Asset::MeshData::Vertex> Vertices;
            std::vector<u32> Indices;
        };

        struct PendingTextureUpload
        {
            Texture* Texture;
            std::vector<byte> Data;
            u32 RowPitch;
        };

        Device*                          m_device;
        std::queue<PendingMeshUpload>    m_pendingMeshUploads;
        std::queue<PendingTextureUpload> m_pendingTextureUploads;
	};
}
