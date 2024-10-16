#pragma once

namespace Ryu::Config
{
	// Treat Config as a singleton
	template <typename T>
	struct IConfig
	{
		static T& Get()
		{
			return GetInstance();
		}

		static const T& Get()
		{
			return GetInstance();
		}

	private:
		static T& GetInstance()
		{
			static T instance;
			return instance;
		}
	};
}
