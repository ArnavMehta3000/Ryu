#include "IGraphicsObject.h"
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include "GraphicsRHI/Utils/Logging.h"
#include "Utils/StringConv.h"

namespace Ryu::Graphics
{
    // DirectX 11 device specialization
    template<>
    void IGraphicsRHIObject<ID3D11Device, API::DirectX11>::SetNameImpl(MAYBE_UNUSED ID3D11Device* native, MAYBE_UNUSED std::string_view name)
    {
        DX11_NAME_OBJECT(native, name);
    }

    // DirectX 11 device child specialization
    template<>
    void IGraphicsRHIObject<ID3D11DeviceChild, API::DirectX11>::SetNameImpl(MAYBE_UNUSED ID3D11DeviceChild* native, MAYBE_UNUSED std::string_view name)
    {
        DX11_NAME_OBJECT(native, name);
    }

    // DirectX 12 device specialization
    template<>
    void IGraphicsRHIObject<ID3D12Device, API::DirectX12>::SetNameImpl(MAYBE_UNUSED ID3D12Device* native, MAYBE_UNUSED std::string_view name)
    {
        const std::string nameStr(name);
        DX12_NAME_OBJECT(native, Utils::ToWideStr(nameStr));
    }

    // DirectX 12 object specialization
    template<>
    void IGraphicsRHIObject<ID3D12Object, API::DirectX12>::SetNameImpl(MAYBE_UNUSED ID3D12Object* native, MAYBE_UNUSED std::string_view name)
    {
        const std::string nameStr(name);
        DX12_NAME_OBJECT(native, Utils::ToWideStr(nameStr));
    }

    // DXGI (uses DX11 naming)
    template<>
    void IGraphicsRHIObject<IDXGIObject, API::DirectX11>::SetNameImpl(MAYBE_UNUSED IDXGIObject* native, MAYBE_UNUSED std::string_view name)
    {
        DX11_NAME_OBJECT(native, name);
    }

    // Explicit instantiations
    template class IGraphicsRHIObject<ID3D11Device     , API::DirectX11>;
    template class IGraphicsRHIObject<ID3D11DeviceChild, API::DirectX11>;
    template class IGraphicsRHIObject<ID3D12Device     , API::DirectX12>;
    template class IGraphicsRHIObject<ID3D12Object     , API::DirectX12>;
    template class IGraphicsRHIObject<IDXGIObject      , API::DirectX11>;
}
