#pragma once
#include "Graphics/Core/GfxResource.h"

namespace Ryu::Gfx
{
    struct DescriptorHandle;
    class CommandList;

    class Texture : public Resource
    {
    public:
        Texture(Device* parent);
        Texture(Device* parent, DX12::Resource* resource, std::string_view name);  // Swapchain textures
        Texture(Device* parent, u32 width, u32 height, DXGI_FORMAT format, std::string_view name);  // New
        virtual ~Texture() = default;

        void UpdateTextureResource(DX12::Resource* resource, std::string_view name);
        void CreateRenderTarget(D3D12_RENDER_TARGET_VIEW_DESC* desc, DescriptorHandle rtvHeapHandle);
        void Upload(const CommandList& cmdList, const void* data, u32 rowPitch);

        [[nodiscard]] u32 GetWidth() const { return m_width; }
        [[nodiscard]] u32 GetHeight() const { return m_height; }
        [[nodiscard]] DXGI_FORMAT GetFormat() const { return m_format; }

    private:
        ComPtr<DX12::Resource> m_uploadBuffer;
        u32 m_width = 0;
        u32 m_height = 0;
        DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
        bool m_isBackBuffer : 1 = false;
        bool m_isRenderTarget : 1 = false;
        bool m_needsUpload : 1 = false;
    };
}
