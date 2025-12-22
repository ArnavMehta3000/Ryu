#include "Graphics/Core/GfxShader.h"

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

	Shader::Shader(Shader&& other) noexcept
		: m_type(other.m_type)
		, m_name(other.m_name)
	{
		m_blob.Swap(other.m_blob);
		m_reflectionBlob.Swap(other.m_reflectionBlob);
	}

	Shader::Shader(const Shader& other)
		: m_type(other.m_type)
		, m_name(other.m_name)
		, m_blob(other.m_blob)
		, m_reflectionBlob(other.m_reflectionBlob)
	{
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		m_type = other.m_type;
		m_name = other.m_name;
		m_blob = other.m_blob.Detach();
		m_reflectionBlob = other.m_reflectionBlob.Detach();
		return *this;
	}

	Shader& Shader::operator=(const Shader& other)
	{
		m_type = other.m_type;
		m_name = other.m_name;
		m_blob = other.m_blob;
		m_reflectionBlob = other.m_reflectionBlob;
		return *this;
	}
}
