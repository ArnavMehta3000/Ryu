#include "Graphics/Core/GfxShader.h"
#include <d3dcompiler.h>

namespace Ryu::Gfx
{
	ShaderType GetShaderTypeFromName(std::string_view name)
	{
		//The last 2 letters of the name are the shader type
		const std::string_view text = name.substr(name.size() - 2);

		if (text == "VS") return ShaderType::VertexShader;
		if (text == "HS") return ShaderType::HullShader;
		if (text == "DS") return ShaderType::DomainShader;
		if (text == "GS") return ShaderType::GeometryShader;
		if (text == "PS") return ShaderType::PixelShader;
		if (text == "CS") return ShaderType::ComputeShader;

		RYU_LOG_ERROR("Trying to load precompiled shaders, unknown shader type ({}) extracted from name", text);
		return ShaderType::VertexShader;
	}

	ComPtr<IDxcBlobEncoding> CreateDXCBlobFromRyuBlob(const Utils::Blob& blob, u32 codePage)
	{
		static ComPtr<IDxcUtils> dxcUtils;
		if (!dxcUtils)
		{
			DXCall(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)));
		}

				
		ComPtr<IDxcBlobEncoding> dxcBlob = nullptr;
		DXCall(dxcUtils->CreateBlob(blob.Data, static_cast<u32>(blob.Size), codePage, &dxcBlob));

		return dxcBlob;
	}

	ComPtr<ID3DBlob> CreateD3DBlobFromRyuBlob(const Utils::Blob& blob)
	{
		ComPtr<ID3DBlob> outBlob;
		::D3DCreateBlob(blob.Size, &outBlob);

		std::memcpy(outBlob->GetBufferPointer(), blob.Data, blob.Size);
		return outBlob;
	}

	Shader::Shader(Shader&& other) noexcept
		: m_type(other.m_type)
		, m_name(other.m_name)
	{
		m_blob.Swap(other.m_blob);
		m_reflectionBlob.Swap(other.m_reflectionBlob);
		m_rootSignatureBlob.Swap(other.m_rootSignatureBlob);
	}

	Shader::Shader(const Shader& other)
		: m_type(other.m_type)
		, m_name(other.m_name)
		, m_blob(other.m_blob)
		, m_reflectionBlob(other.m_reflectionBlob)
		, m_rootSignatureBlob(other.m_rootSignatureBlob)
	{
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		m_type = other.m_type;
		m_name = other.m_name;
		m_blob.Swap(other.m_blob);
		m_reflectionBlob.Swap(other.m_reflectionBlob);
		m_rootSignatureBlob.Swap(other.m_rootSignatureBlob);
		return *this;
	}

	Shader& Shader::operator=(const Shader& other)
	{
		m_type = other.m_type;
		m_name = other.m_name;
		m_blob = other.m_blob;
		m_reflectionBlob = other.m_reflectionBlob;
		m_rootSignatureBlob = other.m_rootSignatureBlob;
		return *this;
	}
}
