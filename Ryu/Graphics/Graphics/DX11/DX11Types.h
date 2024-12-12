#pragma once
#include <d3d11_4.h>

namespace Ryu::Graphics::DX11
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
