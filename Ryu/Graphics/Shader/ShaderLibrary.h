#pragma once
#include "Graphics/Core/GfxShader.h"
#include <span>

namespace Ryu::Gfx
{
	struct PrecompiledShaderInfo;
	struct ShaderCompileInfo;

	class ShaderLibrary
	{
	public:
		using ShaderMap = std::unordered_map<std::string, Shader>;

	public:
		ShaderLibrary(const fs::path& precompiledPath);

		// Uses the shader compiler to compile at runtime
		bool Compile(const ShaderCompileInfo& info);

		Shader* GetShader(const std::string& name);

		u64 GetShaderCount() const { return m_shaders.size(); }
		auto begin() { return m_shaders.begin(); }
		auto end() { return m_shaders.end(); }

	private:
		void StorePrecompiledShaders(std::span<PrecompiledShaderInfo> infos);

	private:
		fs::path m_precompiledPath;
		ShaderMap m_shaders;
	};
}
