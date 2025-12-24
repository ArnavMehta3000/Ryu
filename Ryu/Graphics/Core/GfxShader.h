#pragma once
#include "Graphics/Core/DX12.h"
#include "Graphics/Shader/ShaderType.h"
#include <memory>
#include <filesystem>
#include <dxcapi.h>


namespace Ryu::Gfx
{
	namespace fs = std::filesystem;
	struct ShaderCompileInfo;

	extern ShaderType GetShaderTypeFromName(std::string_view name);

	class Shader
	{
		friend class ShaderLibrary;
	public:
		using Blob = IDxcBlob;
		enum class CompilationSource { Precompiled, RuntimeCompiled };

		Shader() = default;
		~Shader() = default;
		Shader(Shader&& other) noexcept;
		Shader(const Shader& other);

		[[nodiscard]] inline bool IsValid() const { return m_blob != nullptr; }

		[[nodiscard]] inline ShaderType GetType() const { return m_type; }
		[[nodiscard]] inline std::string_view GetName() const { return m_name; }
		[[nodiscard]] inline Blob* GetBlob() const { return m_blob.Get(); }
		[[nodiscard]] inline Blob* GetReflection() const { return m_reflectionBlob.Get(); }
		[[nodiscard]] inline Blob* GetRootSignature() const { return m_rootSignatureBlob.Get(); }

		Shader& operator=(Shader&& other) noexcept;
		Shader& operator=(const Shader& other);

	private:
		std::string       m_name;
		ShaderType        m_type = ShaderType::VertexShader;
		CompilationSource m_source = CompilationSource::Precompiled;
		ComPtr<IDxcBlob>  m_blob;
		ComPtr<IDxcBlob>  m_reflectionBlob;
		ComPtr<IDxcBlob>  m_rootSignatureBlob;
	};
}
