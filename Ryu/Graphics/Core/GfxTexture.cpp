#include "Graphics/Core/GfxTexture.h"
#include "Graphics/Core/GfxDevice.h"
#include "Graphics/Core/GfxCommandList.h"

namespace Ryu::Gfx
{
    Texture::Texture(Device* parent)
        : Resource(parent)
    {}

    Texture::Texture(Device* parent, DX12::Resource* resource, std::string_view name)
        : Resource(parent)
        , m_isBackBuffer(true)
    {
        RYU_ASSERT(resource, "Resource is null!");
        m_resource.Attach(resource);
        DX12::SetObjectName(m_resource.Get(), name.data());
    }

    Texture::Texture(Device* parent, u32 width, u32 height, DXGI_FORMAT format, std::string_view name)
        : Resource(parent)
        , m_width(width)
        , m_height(height)
        , m_format(format)
        , m_needsUpload(true)
    {
        DX12::Device* device = GetDevice()->GetNativeDevice();

        // Create texture resource (default heap - GPU only)
        D3D12_RESOURCE_DESC texDesc{};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = format;
        texDesc.SampleDesc.Count = 1;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        const CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);

        HRESULT hr = device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_resource));

        RYU_ASSERT(SUCCEEDED(hr), "Failed to create texture resource!");
        DX12::SetObjectName(m_resource.Get(), name.data());

        // Create upload buffer
        const u64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, 1);
        const CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

        hr = device->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer));

        RYU_ASSERT(SUCCEEDED(hr), "Failed to create texture upload buffer!");
        DX12::SetObjectName(m_uploadBuffer.Get(), (std::string(name) + " Upload").data());
    }

    void Texture::UpdateTextureResource(DX12::Resource* resource, std::string_view name)
    {
        RYU_ASSERT(resource, "Resource is null!");
        ComRelease(m_resource);
        m_resource.Attach(resource);
        DX12::SetObjectName(m_resource.Get(), name.data());
    }

    void Texture::CreateRenderTarget(D3D12_RENDER_TARGET_VIEW_DESC* desc, DescriptorHandle rtvHeapHandle)
    {
        DX12::Device* device = GetDevice()->GetNativeDevice();
        device->CreateRenderTargetView(m_resource.Get(), desc, rtvHeapHandle.CPU);
        m_isRenderTarget = true;
    }

    void Texture::Upload(const CommandList& cmdList, const void* data, u32 rowPitch)
    {
        if (!m_needsUpload || !data)
        {
            return;
        }

        D3D12_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pData = data;
        subresourceData.RowPitch = rowPitch;
        subresourceData.SlicePitch = rowPitch * m_height;

        UpdateSubresources(
            cmdList.GetNative(),
            m_resource.Get(),
            m_uploadBuffer.Get(),
            0, 0, 1,
            &subresourceData);

        // Transition to shader resource
        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        cmdList.GetNative()->ResourceBarrier(1, &barrier);

        m_needsUpload = false;
    }
}
