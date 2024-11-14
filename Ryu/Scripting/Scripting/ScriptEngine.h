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

		inline constexpr RYU_API asIScriptEngine* GetEngine() const { return m_engine; }

		static void MessageCallback(const asSMessageInfo* info, void* data);

	private:
		void ConfigureEngine();

	private:
		asIScriptEngine* m_engine;
	};
}
