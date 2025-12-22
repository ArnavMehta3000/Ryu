#pragma once
#include "Graphics/Core/GfxShader.h"
#include <string>
#include <unordered_map>
#include <span>

namespace Ryu::Gfx
{
	struct PrecompiledShaderInfo;
	struct ShaderCompileInfo;

	class ShaderLibrary
	{
	public:
		// Maybe the key can only be the name since the type of the shader is embedded into the name?
		struct ShaderKey
		{
			std::string Name;
			ShaderType Type;

			struct Hasher
			{
				std::size_t operator()(const ShaderKey& key) const
				{
					const std::size_t h1 = std::hash<std::string>{}(key.Name);
					const std::size_t h2 = std::hash<std::underlying_type_t<Ryu::Gfx::ShaderType>>{}(
						static_cast<std::underlying_type_t<Ryu::Gfx::ShaderType>>(key.Type));

					return h1 ^ (h2 << 1);  // Combine the two hash values
				}
			};

			auto operator <=> (const ShaderKey& other) const = default;
		};

		using ShaderMap = std::unordered_map<ShaderKey, Shader, ShaderKey::Hasher>;

	public:
		ShaderLibrary(const fs::path& precompiledPath);

		// Uses the shader compiler to compile at runtime
		bool Compile(const ShaderCompileInfo& info);

		Shader* GetShader(const std::string& name, ShaderType type);

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
