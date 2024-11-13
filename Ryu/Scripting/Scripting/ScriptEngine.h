#pragma once
#include "Logger/LogCategory.h"

struct asSMessageInfo;
class asIScriptEngine;

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

		static void MessageCallback(const asSMessageInfo* info, void* data);

	private:
		void ConfigureEngine();

	private:
		asIScriptEngine* m_engine;
	};
}
