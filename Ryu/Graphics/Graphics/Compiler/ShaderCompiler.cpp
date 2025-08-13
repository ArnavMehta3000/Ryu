#include "Graphics/Compiler/ShaderCompiler.h"
#include "Common/Assert.h"
#include "Logging/Logger.h"
#include "Utils/StringConv.h"
#include <fstream>

namespace Ryu::Gfx
{
	static constexpr std::wstring_view g_shaderCSOExt = L".cso";
	static constexpr std::wstring_view g_shaderPDBExt = L".pdb";
	static constexpr std::wstring_view g_CompiledShaderBasePath = L"Shaders\\Compiled";

	ShaderCompiler::ShaderCompiler()
	{
		DXCall(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)));
		DXCall(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)));
		DXCall(m_utils->CreateDefaultIncludeHandler(&m_includeHandler));
	}

	std::wstring ShaderCompiler::GetCSOOutputPath(const ShaderCompileInfo& info)
	{
		// Returns <current_path>/<g_CompiledShaderBasePath>/<filename>.<g_shaderCSOExt>
		return fs::current_path()
			/ g_CompiledShaderBasePath
			/ (info.FilePath.stem().wstring() + g_shaderCSOExt.data());
	}

	std::wstring ShaderCompiler::GetPDBOutputPath(const ShaderCompileInfo& info)
	{
		// Returns <current_path>/<g_CompiledShaderBasePath>/<filename>.<g_shaderPDBExt>
		return fs::current_path()
			/ g_CompiledShaderBasePath
			/ (info.FilePath.stem().wstring() + g_shaderPDBExt.data());
	}


	VoidResult ShaderCompiler::CompileFromFile(const ShaderCompileInfo& info)
	{
		// Get filename from path without extension
		fs::path path = info.FilePath;
		std::string filename = path.stem().string();
		std::replace(filename.begin(), filename.end(), '\\', '/');

		ComPtr<IDxcBlobEncoding> sourceBlob;
		DXCall(m_utils->LoadFile(
			info.FilePath.c_str(),
			nullptr,
			&sourceBlob
		));

		DxcBuffer sourceBuffer
		{
			.Ptr = sourceBlob->GetBufferPointer(),
			.Size = sourceBlob->GetBufferSize(),
			.Encoding = DXC_CP_ACP
		};

		RYU_LOG_INFO("Compiling shader: {}", info.FilePath.string());

		// Compile the shader
		ComPtr<IDxcCompilerArgs> args = MakeCompilerArgs(info);
		ComPtr<IDxcResult> compiledShaderBuffer;

		DXCall(m_compiler->Compile(
			&sourceBuffer,
			args->GetArguments(),
			args->GetCount(),
			nullptr,
			IID_PPV_ARGS(&compiledShaderBuffer)
		));

		ComPtr<IDxcBlobUtf8> errors;
		ComPtr<IDxcBlobWide> shaderName;
		DXCall(compiledShaderBuffer->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), &shaderName));
		if (errors && errors->GetStringLength() > 0)
		{
			const LPCSTR errorString = errors->GetStringPointer();
			return MakeResultError{ errorString };
		}

		// Make output directory
		auto outPath = fs::current_path() / "Compiled";
		std::error_code ec;
		fs::create_directories(outPath, ec);
		if (ec)
		{
			return MakeResultError{ fmt::format("Failed to create output directory: {}", outPath.parent_path().string()) };
		}

		// Write CSO file
		{
			const std::string outFile = (outPath / (filename + ".cso")).string();

			RYU_LOG_DEBUG("Writing compiled shader: {}", outFile);

			std::ofstream shaderOut(
				outFile,
				std::ios::binary | std::ios::trunc | std::ios::out
			);

			if (shaderOut.is_open())
			{
				ComPtr<IDxcBlob> shaderBlob;
				DXCall(compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &shaderName));
				std::string writeData((const char*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
				shaderOut.write(writeData.data(), writeData.size());
			}
			else
			{
				return MakeResultError{ fmt::format("Failed to write CSO file: {}" , outFile) };
			}
		}

		// Write PDB file
		{
			const std::string outFile = (outPath / (filename + ".pdb")).string();

			RYU_LOG_DEBUG("Writing compiled shader PDB: {}", outFile);

			std::ofstream pdbOut(
				outFile,
				std::ios::binary | std::ios::trunc | std::ios::out
			);

			if (pdbOut.is_open())
			{
				ComPtr<IDxcBlob> pdbBlob;
				DXCall(compiledShaderBuffer->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdbBlob), &shaderName));
				std::string writeData((const char*)pdbBlob->GetBufferPointer(), pdbBlob->GetBufferSize());
				pdbOut.write(writeData.data(), writeData.size());
			}
			else
			{
				return MakeResultError{ fmt::format("Failed to write PDB file: {}" , outFile) };
			}
		}

		return {};
	}

	Result<ShaderCompileResult> ShaderCompiler::Compile(const ShaderCompileInfo& info)
	{
		RYU_ASSERT(m_compiler, "Shader compiler is not initialized!");
		RYU_ASSERT(m_utils, "Shader utils is not initialized!");

		RYU_LOG_TRACE("Compiling shader: {}", info.FilePath.string());

		ComPtr<IDxcBlobEncoding> source;
		HRESULT hr = m_utils->LoadFile(
			info.FilePath.c_str(),
			nullptr,
			&source
		);

		if (FAILED(hr))
		{
			const auto message = fmt::format("Failed to load shader file: {}", info.FilePath.string());
			RYU_LOG_ERROR("{}", message);
			return MakeResultError{ message };
		}

		DxcBuffer sourceBuffer
		{
			.Ptr = source->GetBufferPointer(),
			.Size = source->GetBufferSize(),
			.Encoding = DXC_CP_ACP
		};

		ComPtr<IDxcCompilerArgs> args = MakeCompilerArgs(info);
		RYU_ASSERT(args, "Failed to create compiler arguments!");

		ComPtr<IDxcResult> result;
		hr = m_compiler->Compile(
			&sourceBuffer,
			args->GetArguments(),
			args->GetCount(),
			m_includeHandler.Get(),
			IID_PPV_ARGS(&result)
		);

		if (FAILED(hr))
		{
			RYU_LOG_ERROR("Failed to compile shader file: {}", info.FilePath.string());
		}

		ComPtr<IDxcBlobEncoding> errors;
		DXCall(result->GetErrorBuffer(&errors));
		if (errors && errors->GetBufferSize() != 0)
		{
			RYU_LOG_ERROR("{}", (const char*)errors->GetBufferPointer());
			return MakeResultError{"Shader compiled with errors! "};
		}

		hr = result->GetStatus(&hr);
		if (FAILED(hr))
		{
			return MakeResultError{ fmt::format("Failed to compile shader ({})", info.FilePath.string()) };
		}

		const std::wstring csoOutputPath = GetCSOOutputPath(info);
		const std::wstring pdbOutputPath = GetPDBOutputPath(info);

		// Save shader CSO
		ComPtr<IDxcBlob> shaderBlob;
		if (auto getResult = GetCSOBlob(result, csoOutputPath); getResult)
		{
			shaderBlob = std::move(getResult.value());
		}
		else
		{
			return std::unexpected(getResult.error());
		}

#if defined (RYU_BUILD_DEBUG)
		// Save shader PDB
		ComPtr<IDxcBlob> pdbBlob;
		if (auto getResult = GetPDBBlob(result, pdbOutputPath); getResult)
		{
			pdbBlob = std::move(getResult.value());
		}
		else
		{
			return std::unexpected(getResult.error());
		}
#endif

		// Print hash
		const std::string name = info.Name.empty() ? info.FilePath.stem().string() : info.Name;
		std::string hash = GetHash(result);
		RYU_LOG_DEBUG("Shader ({}) hash: {}", name, hash);

		// Get reflection blob
		ComPtr<IDxcBlob> reflectionBlob;
		if (auto getResult = GetReflectionBlob(result); getResult)
		{
			reflectionBlob = std::move(getResult.value());
		}
		else
		{
			return std::unexpected(getResult.error());
		}

		RYU_LOG_INFO("Shader [{}] compiled successfully", name);

		return ShaderCompileResult
		{
			.Name           = name,
			.Hash           = hash,
			.CSOPath        = csoOutputPath,
			.PDBPath        = pdbOutputPath,
			.ShaderBlob     = shaderBlob,
			.ReflectionBlob = reflectionBlob
		};
	}

	ComPtr<IDxcCompilerArgs> ShaderCompiler::MakeCompilerArgs(const ShaderCompileInfo& info)
	{
		//const std::wstring csoOutputPath = GetCSOOutputPath(info);
		//const std::wstring pdbOutputPath = GetPDBOutputPath(info);
		const std::wstring name = info.Name.empty() ? info.FilePath.stem().wstring() : Utils::ToWideStr(info.Name);

		std::vector<LPCWSTR> args;
		args.push_back(name.c_str());

		//args.push_back(L"-Fo");
		//args.push_back(csoOutputPath.c_str());

		//args.push_back(L"-Fd");
		//args.push_back(pdbOutputPath.c_str());

		args.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);

#if defined (RYU_BUILD_DEBUG)
		args.push_back(DXC_ARG_DEBUG);
#else
		args.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

		for (auto& define : info.Defines)
		{
			const std::wstring value = Utils::ToWideStr(define);

			args.push_back(L"-D");
			args.push_back(value.c_str());
		}

		ComPtr<IDxcCompilerArgs> compilerArgs;
		DXCall(m_utils->BuildArguments(
			info.FilePath.c_str(),
			GetEntryPointFromType(info.Type).data(),
			GetTargetProfileFromType(info.Type).data(),
			args.data(), static_cast<uint32_t>(args.size()),  // Arguments
			nullptr, 0,                                       // Defines
			&compilerArgs
		));

		return compilerArgs;
	}

	VoidResult ShaderCompiler::WriteBlobToFile(const ComPtr<IDxcBlob>& blob, const fs::path& path)
	{
		if (!blob)
		{
			return MakeResultError{ "Blob is null!" };
		}

		// Create the output directory if it doesn't exist yet
		if (!fs::exists(path.parent_path()))
		{
			fs::create_directories(path.parent_path());
		}

		std::ofstream file(path, std::ios::binary | std::ios::trunc | std::ios::out);
		if (file.is_open())
		{
			file.write((const char*)blob->GetBufferPointer(), blob->GetBufferSize());
			return {};
		}
		else
		{
			return MakeResultError{ fmt::format("Failed to write file: {}", path.string()) };
		}
	}

	Result<ComPtr<IDxcBlob>> ShaderCompiler::GetCSOBlob(const ComPtr<IDxcResult>& result, fs::path outFile)
	{
		ComPtr<IDxcBlob> shaderBlob;
		DXCall(result->GetResult(&shaderBlob));
		if (auto writeResult = WriteBlobToFile(shaderBlob, outFile); !writeResult)
		{
			return std::unexpected(writeResult.error());
		}

		return shaderBlob;
	}

	Result<ComPtr<IDxcBlob>> ShaderCompiler::GetPDBBlob(const ComPtr<IDxcResult>& result, fs::path outFile)
	{
		ComPtr<IDxcBlob> pdbBlob;
		DXCall(result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdbBlob), nullptr));
		if (auto writeResult = WriteBlobToFile(pdbBlob, outFile); !writeResult)
		{
			return std::unexpected(writeResult.error());
		}

		return pdbBlob;
	}

	Result<ComPtr<IDxcBlob>> ShaderCompiler::GetReflectionBlob(const ComPtr<IDxcResult>& result)
	{
		ComPtr<IDxcBlob> reflectionBlob;
		DXCall(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr));
		return reflectionBlob;
	}

	std::string ShaderCompiler::GetHash(const ComPtr<IDxcResult>& result)
	{
		std::string hash;
		ComPtr<IDxcBlob> hashBlob;
		DXCall(result->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&hashBlob), nullptr));
		if (hashBlob)
		{
			DxcShaderHash* hashBuffer = (DxcShaderHash*)hashBlob->GetBufferPointer();
			for (u32 i = 0; i < _countof(hashBuffer->HashDigest); i++)
			{
				hash += fmt::format("{:02x}", hashBuffer->HashDigest[i]);
			}
		}

		return hash;
	}
}
