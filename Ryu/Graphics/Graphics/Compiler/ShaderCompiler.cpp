#include "Graphics/Compiler/ShaderCompiler.h"
#include "Logger/Logger.h"
#include <fstream>

namespace Ryu::Gfx
{
	ShaderCompiler::ShaderCompiler()
	{
		DXCall(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)));
		DXCall(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)));
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

		RYU_LOG_INFO(LogShaderCompiler, "Compiling shader: {}", info.FilePath.string());

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
			return MakeResultError{ std::format("Failed to create output directory: {}", outPath.parent_path().string()) };
		}

		// Write CSO file
		{
			const std::string outFile = (outPath / (filename + ".cso")).string();

			RYU_LOG_DEBUG(LogShaderCompiler, "Writing compiled shader: {}", outFile);

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
				return MakeResultError{ std::format("Failed to write CSO file: {}" , outFile) };
			}
		}

		// Write PDB file
		{
			const std::string outFile = (outPath / (filename + ".pdb")).string();

			RYU_LOG_DEBUG(LogShaderCompiler, "Writing compiled shader PDB: {}", outFile);

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
				return MakeResultError{ std::format("Failed to write PDB file: {}" , outFile) };
			}
		}

		return {};
	}

	ComPtr<IDxcCompilerArgs> ShaderCompiler::MakeCompilerArgs(const ShaderCompileInfo& info)
	{
		std::vector<LPCWSTR> args;

		args.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);

		if (info.Debug)
		{
			args.push_back(DXC_ARG_DEBUG);
		}
		else
		{
			args.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
		}

		for (auto& define : info.Defines)
		{
			args.push_back(L"-D");
			args.push_back(define.c_str());
		}

		ComPtr<IDxcCompilerArgs> compilerArgs;
		DXCall(m_utils->BuildArguments(
			info.FilePath.c_str(),
			info.EntryPoint.c_str(),
			info.TargetProfile.c_str(),
			args.data(),
			static_cast<uint32_t>(args.size()),
			nullptr,
			0,
			&compilerArgs
		));

		return compilerArgs;
	}
}
