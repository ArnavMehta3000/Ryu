#pragma once

namespace Ryu::Config
{
	/**
	 * @brief Base class for all config classes
	 * @details Config classes must inherit from this class for global access
	 * @tparam T Config type
	 */
	template <typename T>
	struct IConfig
	{
		/**
		 * @brief Static method to get the Config object
		 * @return T& Config object
		 */
		static T& Get()
		{
			static T instance;
			return instance;
		}
	};
}
