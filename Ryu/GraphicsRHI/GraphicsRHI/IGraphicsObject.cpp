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
    void IGraphicsNamedObject<ID3D11Device, API::DirectX11>::SetNameImpl(ID3D11Device* native, std::string_view name)
    {
        DX11_NAME_OBJECT(native, name);
    }

    // DirectX 11 device child specialization
    template<>
    void IGraphicsNamedObject<ID3D11DeviceChild, API::DirectX11>::SetNameImpl(ID3D11DeviceChild* native, std::string_view name)
    {
        DX11_NAME_OBJECT(native, name);
    }

    // DirectX 12 device specialization
    template<>
    void IGraphicsNamedObject<ID3D12Device, API::DirectX12>::SetNameImpl(ID3D12Device* native, std::string_view name)
    {
        const std::string nameStr(name);
        DX12_NAME_OBJECT(native, Utils::ToWideStr(nameStr));
    }

    // DirectX 12 object specialization
    template<>
    void IGraphicsNamedObject<ID3D12Object, API::DirectX12>::SetNameImpl(ID3D12Object* native, std::string_view name)
    {
        const std::string nameStr(name);
        DX12_NAME_OBJECT(native, Utils::ToWideStr(nameStr));
    }

    // DXGI (uses DX11 naming)
    template<>
    void IGraphicsNamedObject<IDXGIObject, API::DirectX11>::SetNameImpl(IDXGIObject* native, std::string_view name)
    {
        DX11_NAME_OBJECT(native, name);
    }

    // Explicit instantiations
    template class IGraphicsNamedObject<ID3D11Device     , API::DirectX11>;
    template class IGraphicsNamedObject<ID3D11DeviceChild, API::DirectX11>;
    template class IGraphicsNamedObject<ID3D12Device     , API::DirectX12>;
    template class IGraphicsNamedObject<ID3D12Object     , API::DirectX12>;
    template class IGraphicsNamedObject<IDXGIObject      , API::DirectX11>;
}
