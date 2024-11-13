#pragma once
#include "Scripting/Common.h"
#include "Logger/LogCategory.h"

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

		static void MessageCallback(const AS::asSMessageInfo* info, void* data);

	private:
		void ConfigureEngine();

	private:
		AS::asIScriptEngine* m_engine;
	};
}
