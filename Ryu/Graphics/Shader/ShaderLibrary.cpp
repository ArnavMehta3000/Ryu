#include "Graphics/Shader/ShaderLibrary.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Core/Logging/Logger.h"
#include "Core/Utils/ReadData.h"
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
						.Name           = basePath.stem().string()  // The name is the filename without the extension
					};

					// The root signature file does not have the shader type appended to it
					// Eg. CubeVS.cso will have Cube.rootsig

					std::string nameIsolated = info.Name.substr(0, info.Name.length() - 2);  // Name without shader extensions (VS/PS, etc,)
					info.RootSigPath = fs::path(basePath).replace_filename(nameIsolated).replace_extension(rootSignatureExtension);
					
					RYU_LOG_TRACE("Found precompiled shader ({})", basePath.filename().string());
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
		//ShaderCompiler& compiler = ShaderCompiler::Get();

		//if (auto compileResult = compiler.Compile(info))
		//{
		//	ShaderCompileResult& result = compileResult.value();

		//	Shader shader{};
		//	shader.m_name = result.Name;
		//	shader.m_type = info.Type;
		//	shader.m_source = Shader::CompilationSource::Precompiled;

		//	shader.m_blob.Attach(result.ShaderBlob.Detach());
		//	shader.m_reflectionBlob.Attach(result.ReflectionBlob.Detach());
		//	
		//	// Add to shader map
		//	if (m_shaders.contains(shader.m_name))
		//	{
		//		RYU_LOG_WARN("Shader with name {} already exists. Overwriting", info.Name);
		//	}
		//	m_shaders[shader.m_name] = std::move(shader);

		//	return true;
		//}
		//else
		//{
		//	RYU_LOG_ERROR("Failed to compile shader: {} | {}", info.FilePath.string(), compileResult.error());
		//}
		
		return false;
	}

	/*Shader* ShaderLibrary::GetShader(const std::string& name)
	{
		return m_shaders.contains(name) ? &m_shaders[name] : nullptr;		
	}*/
	
	void ShaderLibrary::StorePrecompiledShaders(std::span<PrecompiledShaderInfo> infos)
	{
		//static IDxcUtils* utils = ShaderCompiler::Get().GetUtils();

		//if (!utils)
		//{
		//	RYU_LOG_ERROR("Failed to get DXC Utils");
		//	return;
		//}
		//		
		//for (const auto& info : infos)
		//{
		//	// Source path has to be valid to even consider the rest of the data
		//	if (Utils::ReadDataResult srcReadResult = Utils::ReadData(info.SourcePath))
		//	{
		//		ComPtr<IDxcBlobEncoding> srcBlob;
		//		{
		//			const std::vector<byte>& srcData = srcReadResult.value();
		//			DXCall(utils->CreateBlob(srcData.data(), (u32)srcData.size(), DXC_CP_ACP, &srcBlob));
		//		}

		//		Shader shader{};
		//		shader.m_name    = info.Name;
		//		shader.m_type    = GetShaderTypeFromName(info.Name);
		//		shader.m_source  = Shader::CompilationSource::Precompiled;
		//		shader.m_blob.Attach(srcBlob.Detach());

		//		// Check for reflection data
		//		if (Utils::ReadDataResult reflReadResult = Utils::ReadData(info.ReflectionPath))
		//		{
		//			const std::vector<byte>& reflData = reflReadResult.value();

		//			ComPtr<IDxcBlobEncoding> reflBlob;
		//			{
		//				DXCall(utils->CreateBlob(reflData.data(), u32(reflData.size()), DXC_CP_ACP, &reflBlob));
		//				shader.m_reflectionBlob.Swap(reflBlob);
		//			}
		//		}
		//		else
		//		{
		//			RYU_LOG_DEBUG("No reflection data forund for shader ({})", info.Name);
		//		}

		//		// Check for root signature data
		//		if (Utils::ReadDataResult rootSigReadResult = Utils::ReadData(info.RootSigPath))
		//		{
		//			const std::vector<byte>& rootSigData = rootSigReadResult.value();

		//			ComPtr<IDxcBlobEncoding> rootSigBlob;
		//			{
		//				DXCall(utils->CreateBlob(rootSigData.data(), u32(rootSigData.size()), DXC_CP_ACP, &rootSigBlob));
		//				shader.m_rootSignatureBlob.Swap(rootSigBlob);
		//			}
		//		}
		//		
		//		// Add to shader map
		//		//if (m_shaders.contains(shader.m_name))
		//		//{
		//		//	RYU_LOG_WARN("Shader with name {} already exists. Overwriting", info.Name);
		//		//}
		//		//m_shaders[shader.m_name] = std::move(shader);
		//	}
		//}
	}
}
