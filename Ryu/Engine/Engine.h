#pragma once
#include <Core/Includes.h>

namespace Ryu
{
	class Engine;

	RYU_API_FUNC(bool, InitializeEngineInstance, Engine* engine);


	class Engine
	{
	public:

		void Run()
		{
			int x = 0;
		}

	private:
	};

	using EnginePtr = Engine**;
}