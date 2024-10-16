#pragma once

namespace Ryu::Config
{
	// Treat Config as a singleton
	template <typename T>
	struct IConfig
	{
		static T& Get()
		{
			static T instance;
			return instance;
		}
	};
}
