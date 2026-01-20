#pragma once

namespace Ryu::Gfx
{
	enum class ShaderType : u8
	{
		VertexShader,
		PixelShader,
		GeometryShader,
		HullShader,
		DomainShader,
		ComputeShader,
	};

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

namespace Ryu
{
	template<>
	inline constexpr std::string_view EnumToString<::Ryu::Gfx::ShaderType>(::Ryu::Gfx::ShaderType value)
	{
		switch (value)
		{
			using enum ::Ryu::Gfx::ShaderType;
		case VertexShader  : return "VertexShader";
		case PixelShader   : return "PixelShader";
		case GeometryShader: return "GeometryShader";
		case HullShader    : return "HullShader";
		case DomainShader  : return "DomainShader";
		case ComputeShader : return "ComputeShader";
		default            : return "<UNKNOWN>";
		}
	}
}
