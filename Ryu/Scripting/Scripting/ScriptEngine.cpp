#include "Scripting/ScriptEngine.h"
#include "Common/StandardTypes.h"
#include "Logger/Logger.h"
#include "Logger/Assert.h"
#include "Profiling/Profiling.h"
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptfile/scriptfilesystem.h>
#include <datetime/datetime.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace Ryu::Scripting
{
	ScriptEngine::ScriptEngine(const std::string& scriptDir)
		: m_scriptDir(scriptDir)
	{
		RYU_PROFILE_SCOPE();
		m_engine = asCreateScriptEngine();
		m_engine->SetMessageCallback(asFUNCTION(ScriptEngine::EngineLogMessageCallback), nullptr, asCALL_CDECL);

		RegisterScriptDateTime(m_engine);
		RegisterStdString(m_engine);
		RegisterScriptArray(m_engine, true);
		RegisterScriptDictionary(m_engine);
		RegisterScriptFile(m_engine);
		RegisterScriptFileSystem(m_engine);
		
		RegisterLogging();

		LoadScriptFiles();

		asIScriptFunction* func = m_module->GetFunctionByDecl("void main()");
		asIScriptContext* ctx = m_contexts["Test.as"];
		ctx->Prepare(func);
		ctx->Execute();
	}
	
	ScriptEngine::~ScriptEngine()
	{
		RYU_PROFILE_SCOPE();

		for (auto& [script, ctx] : m_contexts)
		{
			if (ctx)
			{
				RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(ScriptEngine), "Releasing context for {}", script);
				ctx->Release();
			}
		}

		m_engine->ShutDownAndRelease();
	}
	
	void ScriptEngine::EngineLogMessageCallback(asSMessageInfo* msg, void* param)
	{
		if (!msg)
		{
			return;
		}

		const std::string formattedMsg = std::format("Angelscript: {} ({}, {}) : {} ", msg->section, msg->row, msg->col, msg->message);

		switch (msg->type)
		{
		case asMSGTYPE_INFORMATION:
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(ScriptEngine), "{}", formattedMsg);
			break;
			
		case asMSGTYPE_ERROR:
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(ScriptEngine), "{}", formattedMsg);
			break;

		case asMSGTYPE_WARNING:
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(ScriptEngine), "{}", formattedMsg);
			break;
		}
	}

	void ScriptEngine::ScriptLogMessageCallback(Logging::LogLevel level, const std::string& message)
	{
		std::string callInfo;
		if (asIScriptContext* ctx = asGetActiveContext())
		{
			if (asIScriptFunction* func = ctx->GetFunction())
			{
				const fs::path scriptName = func->GetScriptSectionName();
				callInfo = std::format("{}({})",
					scriptName.filename().string(), ctx->GetLineNumber());
			}
		}

		using enum Ryu::Logging::LogLevel;
		switch (level)
		{
		case Trace:
			RYU_LOG_TRACE(RYU_LOG_USE_CATEGORY(ScriptEngine), "AngelScript({}) - {}", callInfo, message);
			return;
		case Debug:
			RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(ScriptEngine), "AngelScript({}) - {}", callInfo, message);
			return;
		case Info:
			RYU_LOG_INFO(RYU_LOG_USE_CATEGORY(ScriptEngine), "AngelScript({}) - {}", callInfo, message);
			return;
		case Warn:
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(ScriptEngine), "AngelScript({}) - {}", callInfo, message);
			return;
		case Error:
			RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(ScriptEngine), "AngelScript({}) - {}", callInfo, message);
			return;
		case Fatal:
			RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(ScriptEngine), "AngelScript({}) - {}", callInfo, message);
			return;
		}
	}

	void ScriptEngine::RegisterLogging()
	{
		RYU_PROFILE_SCOPE();

		i32 r = 0;
		r = m_engine->SetDefaultNamespace("Ryu");                RYU_ASSERT(r >= 0);

		r = m_engine->RegisterEnum("LogLevel");                  RYU_ASSERT(r >= 0);
		r = m_engine->RegisterEnumValue("LogLevel", "Trace", 0); RYU_ASSERT(r >= 0);
		r = m_engine->RegisterEnumValue("LogLevel", "Debug", 1); RYU_ASSERT(r >= 0);
		r = m_engine->RegisterEnumValue("LogLevel", "Info", 2);  RYU_ASSERT(r >= 0);
		r = m_engine->RegisterEnumValue("LogLevel", "Warn", 3);  RYU_ASSERT(r >= 0);
		r = m_engine->RegisterEnumValue("LogLevel", "Error", 4); RYU_ASSERT(r >= 0);
		r = m_engine->RegisterEnumValue("LogLevel", "Fatal", 5); RYU_ASSERT(r >= 0);

		r = m_engine->RegisterGlobalFunction(
			"void Log(LogLevel level, string &in)",
			asFUNCTION(ScriptEngine::ScriptLogMessageCallback),
			asCALL_CDECL); RYU_ASSERT(r >= 0);

		r = m_engine->SetDefaultNamespace(""); RYU_ASSERT(r >= 0);
	}
	
	void ScriptEngine::LoadScriptFiles()
	{
		RYU_PROFILE_SCOPE();
		const auto GetFilesRecursively = [&](const fs::path& directory)
		{
			std::vector<fs::path> asFiles;

			if (!fs::exists(directory) || !fs::is_directory(directory))
			{
				return asFiles;
			}

			for (const auto& entry : fs::recursive_directory_iterator(directory))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".as")
				{
					asFiles.push_back(fs::absolute(entry.path()));
				}
			}

			return asFiles;
		};

		std::vector<fs::path> scriptFiles = GetFilesRecursively(m_scriptDir);
		if (scriptFiles.empty())
		{
			RYU_LOG_WARN(RYU_LOG_USE_CATEGORY(ScriptEngine), "No script files (*.as) found in {}", m_scriptDir);
		}
		
		i32 r = 0;
		ScriptBuilder builder;
		r = builder.StartNewModule(m_engine, ScriptBuilder::MODULE_NAME); RYU_ASSERT(r >= 0);
		for (const auto& file : scriptFiles)
		{
			// Get file name
			std::string fileName = file.filename().string();
			r = builder.AddSectionFromFile(file.string().c_str()); RYU_ASSERT(r >= 0);

			m_contexts[fileName] = m_engine->CreateContext();
		}
		
		r = builder.BuildModule(); RYU_ASSERT(r >= 0);
		m_module = builder.GetModule();
	}
}
