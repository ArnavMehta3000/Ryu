#pragma once
#include "Logger/LogCategory.h"

struct asSMessageInfo;
class asIScriptEngine;
class asIScriptContext;

namespace Ryu::Scripting
{
	class ScriptEngine
	{
		RYU_LOG_CATEGORY(ScriptEngine);

	public:
		ScriptEngine();
		~ScriptEngine();

		bool Init();
		void Shutdown();

		asIScriptContext* CreateContext() const;
		inline constexpr RYU_API asIScriptEngine* GetEngine() const { return m_engine; }

	private:
		void PrintExceptionInfo(asIScriptContext* context) const;
		void MessageCallback(const asSMessageInfo* info, void* data);
		void ConfigureEngine();

	private:
		asIScriptEngine* m_engine;
	};
}
