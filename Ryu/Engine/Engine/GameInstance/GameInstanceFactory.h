#pragma once
#include "Logger/LogCategory.h"
#include <memory>
#include <string>

class asIScriptEngine;
namespace Ryu::Game { class ScriptableGameInstance; };

namespace Ryu::Engine
{
	class GameInstanceFactory
	{
		RYU_LOG_CATEGORY(GameInstanceFactory);
	public:
		GameInstanceFactory(asIScriptEngine* engine);
		std::unique_ptr<Game::ScriptableGameInstance> CreateInstance(const std::string& className);

	private:
		asIScriptEngine* m_scriptEngine;
	};
}
