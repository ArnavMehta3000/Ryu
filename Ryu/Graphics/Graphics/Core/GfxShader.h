#pragma once
#include "Graphics/Core/DX12.h"
#include "Graphics/ShaderType.h"
#include <memory>
#include <dxcapi.h>

namespace Ryu::Gfx
{
	struct ShaderCompileInfo;

	class Shader
	{
	public:
		using Blob = IDxcBlob;

		static Shader Compile(const ShaderCompileInfo& compileInfo);

		[[nodiscard]] inline bool IsValid() const { return m_blob != nullptr; }

		[[nodiscard]] inline ShaderType GetType() const { return m_type; }
		[[nodiscard]] inline std::string_view GetName() const { return m_name; }
		[[nodiscard]] inline std::string_view GetSourcePath() const { return m_sourcePath; }
		[[nodiscard]] inline std::string_view GetCompiledPath() const { return m_compiledPath; }
		[[nodiscard]] inline Blob* GetBlob() const { return m_blob.Get(); }
		[[nodiscard]] inline Blob* GetReflection() const { return m_reflection.Get(); }

	private:
		ShaderType m_type;
		std::string m_name;
		std::string m_sourcePath;
		std::string m_compiledPath;
		ComPtr<IDxcBlob> m_blob = nullptr;
		ComPtr<IDxcBlob> m_reflection = nullptr;
	};
}
