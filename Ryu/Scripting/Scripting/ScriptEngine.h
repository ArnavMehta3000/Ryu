#pragma once
#include "Common/ObjectMacros.h"
#include "Logging/LogLevel.h"
#include "Scripting/ScriptBuilder.h"
#include <string>
#include <unordered_map>

namespace Ryu::Scripting
{
	class ScriptEngine
	{
	public:
		ScriptEngine(const std::string& scriptDir);
		~ScriptEngine();

		inline NODISCARD asIScriptEngine* GetEngine() const noexcept { return m_engine; }
		inline NODISCARD asIScriptModule* GetModule() const noexcept { return m_module; }

	private:
		static void EngineLogMessageCallback(asSMessageInfo* msg, void* param);
		static void ScriptLogMessageCallback(Logging::LogLevel level, const std::string& message);

		void RegisterLogging();
		void LoadScriptFiles();

	private:
		asIScriptEngine* m_engine = nullptr;
		asIScriptModule* m_module = nullptr;
		std::string m_scriptDir;
		std::unordered_map<std::string, asIScriptContext*> m_contexts;
	};
}
