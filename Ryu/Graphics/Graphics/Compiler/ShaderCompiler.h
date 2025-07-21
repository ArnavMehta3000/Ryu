#pragma once
#include "Graphics/Core/DX12.h"
#include <dxcapi.h>
#include <filesystem>

namespace Ryu::Gfx
{
	namespace fs = std::filesystem;
	RYU_LOG_DECLARE_CATEGORY(ShaderCompiler);

	struct ShaderCompileInfo
	{
		bool Debug = false;
		fs::path FilePath;
		std::wstring EntryPoint;
		std::wstring TargetProfile;
		std::vector<std::wstring> Defines;
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler();

		VoidResult CompileFromFile(const ShaderCompileInfo& info);

	private:
		ComPtr<IDxcCompilerArgs> MakeCompilerArgs(const ShaderCompileInfo& info);

	private:
		ComPtr<IDxcUtils> m_utils;
		ComPtr<IDxcCompiler3> m_compiler;
	};
}
