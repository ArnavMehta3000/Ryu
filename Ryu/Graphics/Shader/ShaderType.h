#pragma once
#include <ShaderType.Generated.h>

namespace Ryu::Gfx
{
	inline std::wstring_view GetTargetProfileFromType(ShaderType type)
	{
		using enum ShaderType;
		switch (type)
		{
		case VertexShader  : return L"vs_6_0";
		case PixelShader   : return L"ps_6_0";
		case GeometryShader: return L"gs_6_0";
		case HullShader    : return L"hs_6_0";
		case DomainShader  : return L"ds_6_0";
		case ComputeShader : return L"cs_6_0";
		default            : return L"";
		}
	}

	inline std::wstring_view GetEntryPointFromType(ShaderType type)
	{
		using enum ShaderType;
		switch (type)
		{
		case VertexShader  : return L"VSMain";
		case PixelShader   : return L"PSMain";
		case GeometryShader: return L"GSMain";
		case HullShader    : return L"HSMain";
		case DomainShader  : return L"DSMain";
		case ComputeShader : return L"CSMain";
		default            : return L"main";
		}
	}
}
