#pragma once
#include "Common/Common.h"

namespace Ryu::Game
{
	class ScriptableGameInstance
	{
	public:
		virtual ~ScriptableGameInstance() = default;

		virtual void OnInit() = 0;
		virtual void OnStart() = 0;
		virtual void OnStop() = 0;
		virtual void OnUpdate(f64 deltaTime) = 0;
		virtual void OnShutdown() = 0;

		constexpr inline void AddRef() { ++m_refCount; }
		constexpr inline void Release() { if (--m_refCount == 0) delete this; }

	private:
		i32 m_refCount{ 1 };
	};
}
