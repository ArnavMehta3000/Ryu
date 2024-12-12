#pragma once
#include "Graphics/Shared/SharedTypes.h"
#include <dxgi.h>

enum D3D_PRIMITIVE_TOPOLOGY : int;
enum D3D11_STENCIL_OP : int;
enum D3D11_COMPARISON_FUNC : int;
enum D3D11_FILL_MODE : int;
enum D3D11_CULL_MODE : int;
enum D3D11_BLEND_OP : int;
enum D3D12_PRIMITIVE_TOPOLOGY_TYPE : int;
enum D3D12_STENCIL_OP : int;
enum D3D12_COMPARISON_FUNC : int;
enum D3D12_FILL_MODE : int;
enum D3D12_CULL_MODE : int;
enum D3D12_BLEND_OP : int;

namespace Ryu::Graphics::Utils
{
	DXGI_FORMAT GetFormat(Format format);
	DXGI_FORMAT GetIndexFormat(IndexFormat format);
	bool IsFormatTypeless(Format format);

	D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(PrimitiveTopology topology);
	D3D11_STENCIL_OP GetStencilOp(StencilOp op);
	D3D11_COMPARISON_FUNC GetComparisonFunc(ComparisonFunc func);
	D3D11_FILL_MODE GetFillMode(FillMode mode);
	D3D11_CULL_MODE GetCullMode(CullMode mode);
	D3D11_BLEND_OP GetBlendOp(BlendOp op);
	
	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology);
	D3D12_STENCIL_OP GetStencilOp12(StencilOp op);
	D3D12_COMPARISON_FUNC GetComparisonFunc12(ComparisonFunc func);
	D3D12_FILL_MODE GetFillMode12(FillMode mode);
	D3D12_CULL_MODE GetCullMode12(CullMode mode);
	D3D12_BLEND_OP GetBlendOp12(BlendOp op);
}
