#include "Graphics/Shader/ShaderLibrary.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Logging/Logger.h"
#include "Utils/ReadData.h"
#include <dxcapi.h>

namespace Ryu::Gfx
{
	struct PrecompiledShaderInfo
	{
		fs::path SourcePath;
		fs::path ReflectionPath;
		fs::path RootSigPath;
		std::string Name;
	};

	ShaderLibrary::ShaderLibrary(const fs::path& precompiledPath)
		: m_precompiledPath(precompiledPath)
	{
		std::vector<PrecompiledShaderInfo> precompiledShaders;

		try
		{
			// Recursively loop through the precompiled shaders path and save their (possible) paths/filenames
			const auto it = fs::directory_iterator(m_precompiledPath, fs::directory_options::skip_permission_denied);
			for (const auto& entry : it)
			{
				if (entry.is_directory())
				{
					continue;
				}

				// Maybe add PDB's as well
				static constexpr std::string_view blobExtension(".cso");
				static constexpr std::string_view reflectionExtension(".reflect");
				static constexpr std::string_view rootSignatureExtension(".rootsig");

				// Save complete path
				fs::path basePath = std::filesystem::absolute(entry).make_preferred();
				fs::path extension = basePath.extension();

				// We at least have a shader blob (or only look for CSO files)
				if (extension == blobExtension)
				{
					PrecompiledShaderInfo info
					{
						.SourcePath     = basePath,
						.ReflectionPath = fs::path(basePath).replace_extension(reflectionExtension),
						.RootSigPath    = fs::path(basePath).replace_extension(rootSignatureExtension),
						.Name           = basePath.stem().string()  // The name is the filename without the extension
					};

					RYU_LOG_TRACE("Found precompiled shader ({})", info.SourcePath.string());
					precompiledShaders.emplace_back(info);
				}
			}

			// All shaders (CSO's) in this vector should be valid
			StorePrecompiledShaders(precompiledShaders);
		}
		catch (const std::exception& e)
		{
			RYU_LOG_ERROR("Failed to load precompiled shaders: {}", e.what());
		}
	}

	bool ShaderLibrary::Compile(const ShaderCompileInfo& info)
	{
		ShaderCompiler& compiler = ShaderCompiler::Get();

		if (auto compileResult = compiler.Compile(info))
		{
			ShaderCompileResult& result = compileResult.value();

			Shader shader{};
			shader.m_name = result.Name;
			shader.m_type = info.Type;
			shader.m_source = Shader::CompilationSource::Precompiled;

			shader.m_blob.Attach(result.ShaderBlob.Detach());
			shader.m_reflectionBlob.Attach(result.ReflectionBlob.Detach());
			
			// Add to shader map
			ShaderKey key{.Name = result.Name, .Type = shader.GetType() };
			m_shaders[key] = std::move(shader);

			return true;
		}
		else
		{
			RYU_LOG_ERROR("Failed to compile shader: {} | {}", info.FilePath.string(), compileResult.error());
		}
		
		return false;
	}

	Shader* ShaderLibrary::GetShader(const std::string& name, ShaderType type)
	{
		ShaderKey key{.Name = name, .Type = type };
		return m_shaders.contains(key) ? &m_shaders[key] : nullptr;		
	}
	
	void ShaderLibrary::StorePrecompiledShaders(std::span<PrecompiledShaderInfo> infos)
	{
		static IDxcUtils* utils = ShaderCompiler::Get().GetUtils();

		if (!utils)
		{
			RYU_LOG_ERROR("Failed to get DXC Utils");
			return;
		}
				
		for (const auto& info : infos)
		{
			// Source path has to be valid to even consider the rest of the data
			if (Utils::ReadDataResult srcReadResult = Utils::ReadData(info.SourcePath))
			{
				ComPtr<IDxcBlobEncoding> srcBlob;
				{
					const std::vector<byte>& srcData = srcReadResult.value();
					DXCall(utils->CreateBlob(srcData.data(), (u32)srcData.size(), DXC_CP_ACP, &srcBlob));
				}

				Shader shader{};
				shader.m_name    = info.Name;
				shader.m_type    = GetShaderTypeFromName(info.Name);
				shader.m_source  = Shader::CompilationSource::Precompiled;
				shader.m_blob.Attach(srcBlob.Detach());

				// Check for reflection data
				if (Utils::ReadDataResult reflReadResult = Utils::ReadData(info.ReflectionPath))
				{
					const std::vector<byte>& reflData = reflReadResult.value();

					ComPtr<IDxcBlobEncoding> reflBlob;
					{
						DXCall(utils->CreateBlob(reflData.data(), u32(reflData.size()), DXC_CP_ACP, &reflBlob));
						shader.m_reflectionBlob.Swap(reflBlob);
					}
				}
				else
				{
					RYU_LOG_DEBUG("No reflection data forund for shader ({})", info.Name);
				}
				
				// Add to shader map
				ShaderKey key{.Name = info.Name, .Type = shader.GetType() };
				m_shaders[key] = std::move(shader);
			}
		}
	}
}
