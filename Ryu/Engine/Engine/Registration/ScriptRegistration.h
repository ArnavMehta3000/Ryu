#pragma once
#include "Common/ObjectMacros.h"
class asIScriptEngine;

namespace Ryu::Engine
{
	class Engine;

	/**
	 * @brief Handles the registration of Ryu - angelscript functions
	 */
	class ScriptRegistration
	{
	public:
		static void Register(asIScriptEngine* engine);

	private:
#define DECLARE_API(X) static void RYU_CONCAT(Register, RYU_CONCAT(X, API))(asIScriptEngine* engine)

		DECLARE_API(Logger);
		DECLARE_API(Input);
		DECLARE_API(Engine);
		DECLARE_API(Debug);
		DECLARE_API(World);
		DECLARE_API(GameInstance);

#undef DECLARE_API
	};
}
