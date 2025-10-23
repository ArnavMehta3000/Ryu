#pragma once
#include "Graphics/Core/DX12.h"
#include "Graphics/Shader/ShaderType.h"
#include <dxcapi.h>
#include <filesystem>

namespace Ryu::Gfx
{
	namespace fs = std::filesystem;

	struct ShaderCompileInfo
	{
		fs::path FilePath;
		ShaderType Type;
		std::string Name;
		std::vector<std::string> Defines;
	};

	struct ShaderCompileResult
	{
		std::string Name;
		std::string Hash;
		fs::path CSOPath;
		fs::path PDBPath;
		ComPtr<IDxcBlob> ShaderBlob;
		ComPtr<IDxcBlob> ReflectionBlob;
	};

	RYU_TODO("Implement shader hot reloading");
	class ShaderCompiler
	{
		RYU_DISABLE_COPY_AND_MOVE(ShaderCompiler)

	public:
		ShaderCompiler();
		~ShaderCompiler() = default;
		
		static std::wstring GetCSOOutputPath(const ShaderCompileInfo& info);
		static std::wstring GetPDBOutputPath(const ShaderCompileInfo& info);

		VoidResult CompileFromFile(const ShaderCompileInfo& info);
		Result<ShaderCompileResult> Compile(const ShaderCompileInfo& info);

	private:
		ComPtr<IDxcCompilerArgs> MakeCompilerArgs(const ShaderCompileInfo& info);
		VoidResult WriteBlobToFile(const ComPtr<IDxcBlob>& blob, const fs::path& path);
		Result<ComPtr<IDxcBlob>> GetCSOBlob(const ComPtr<IDxcResult>& result, fs::path outFile);
		Result<ComPtr<IDxcBlob>> GetPDBBlob(const ComPtr<IDxcResult>& result, fs::path outFile);
		Result<ComPtr<IDxcBlob>> GetReflectionBlob(const ComPtr<IDxcResult>& result);
		std::string GetHash(const ComPtr<IDxcResult>& result);

	private:
		ComPtr<IDxcUtils>          m_utils;
		ComPtr<IDxcCompiler3>      m_compiler;
		ComPtr<IDxcIncludeHandler> m_includeHandler;
	};
}
