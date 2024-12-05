#pragma once
#include "Graphics/API.h"
#include <d3d11_4.h>

namespace Ryu::Graphics
{
	namespace DX11
	{
		using IDX11Device            = ID3D11Device5;
		using IDX11DeviceContext     = ID3D11DeviceContext4;
		using IDX11RenderTarget      = ID3D11RenderTargetView1;
		using IDX11Texture2D         = ID3D11Texture2D1;
		using IDX11DepthStencil      = ID3D11DepthStencilView;
		using IDX11ShaderResource    = ID3D11ShaderResourceView1;
		using IDX11VertexShader      = ID3D11VertexShader;
		using IDX11PixelShader       = ID3D11PixelShader;
		using IDX11InputLayout       = ID3D11InputLayout;
		using IDX11SamplerState      = ID3D11SamplerState;
		using IDX11BlendState        = ID3D11BlendState;
		using IDX11DepthStencilState = ID3D11DepthStencilState;
		using IDX11RasterizerState   = ID3D11RasterizerState;
		using IDX11Buffer            = ID3D11Buffer;
	}
	
	template<>
	struct APITraits<API::DirectX11>
	{
		using DeviceType            = DX11::IDX11Device;
		using DeviceContextType     = DX11::IDX11DeviceContext;
		using RenderTargetType      = DX11::IDX11RenderTarget;
		using Texture2DType         = DX11::IDX11Texture2D;
		using DepthStencilType      = DX11::IDX11DepthStencil;
		using ShaderResourceType    = DX11::IDX11ShaderResource;
		using VertexShaderType      = DX11::IDX11VertexShader;
		using PixelShaderType       = DX11::IDX11PixelShader;
		using InputLayoutType       = DX11::IDX11InputLayout;
		using SamplerStateType      = DX11::IDX11SamplerState;
		using BlendStateType        = DX11::IDX11BlendState;
		using DepthStencilStateType = DX11::IDX11DepthStencilState;
		using RasterizerStateType   = DX11::IDX11RasterizerState;
		using BufferType            = DX11::IDX11Buffer;
	};
}
