#include "Graphics/Core/GfxShader.h"
#include "Graphics/Compiler/ShaderCompiler.h"

namespace Ryu::Gfx
{
	Shader Shader::Compile(const ShaderCompileInfo& compileInfo)
	{
		static ShaderCompiler compiler;

		if (auto compileResult = compiler.Compile(compileInfo))
		{
			ShaderCompileResult& result = compileResult.value();

			Shader shader;
			
			shader.m_type         = compileInfo.Type;
			shader.m_name         = compileInfo.Name;
			shader.m_sourcePath   = compileInfo.FilePath.string();
			shader.m_compiledPath = result.CSOPath.string();
			shader.m_blob         = result.ShaderBlob;
			shader.m_reflection   = result.ReflectionBlob;

			return shader;
		}
		else
		{
			RYU_LOG_ERROR("Failed to compile shader: {}", compileInfo.FilePath.string());
		}

		return Shader();
	}
}
