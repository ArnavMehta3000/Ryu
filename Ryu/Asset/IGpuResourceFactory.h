#pragma once
#include "Asset/AssetData.h"

namespace Ryu::Asset
{
    class IGpuResourceFactory
    {
    public:
        virtual ~IGpuResourceFactory() = default;

        //virtual std::unique_ptr<Gfx::Mesh> CreateMesh(const MeshData& data, std::string_view name) = 0;
        //virtual std::unique_ptr<Gfx::Texture> CreateTexture(const TextureData& data, std::string_view name) = 0;

        //virtual void ProcessPendingUploads(Gfx::CommandList& cmdList) = 0;
    };
}
