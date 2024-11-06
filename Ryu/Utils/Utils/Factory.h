#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Ryu::Utils
{
	/**
	 * @brief Class to auto register types during static initialization
	 * @tparam Base The class that inherits from Factory
	 * @tparam ...Args The arguments to pass to the constructor
	 */
	template <class Base, class... Args>
	class Factory
	{
		friend Base;
	public:
		/**
		 * @brief The function type of the factory
		 */
		using FuncType = std::unique_ptr<Base>(*)(Args...);

		/**
		 * @brief Alias for the underlying unordered_map
		 */
		using FactoryMap = std::unordered_map<std::string, FuncType>;

		/**
		 * @brief The base class of the factory registrar
		 * @tparam T The class to register
		 */
		template <class T>
		struct Registrar : Base
		{
			friend T;
		public:
			/**
			 * @brief Function to register the class with the factory
			 * @return True if registration was successful
			 */
			static bool Register()
			{
				std::string name(T::GetStaticName());
				
				Factory::GetStaticData()[name] = [](Args... args) -> std::unique_ptr<Base>
				{
					return std::make_unique<T>(std::forward<Args>(args)...);
				};
				return true;
			}
			static bool s_registered;

		private:
			Registrar() : Base(Key{}) { (void)s_registered; }
		};

	private:
		class Key
		{
			Key() {}
			template <class T> friend struct Registrar;
		};

	public:
		/**
		 * @brief Create an instance of the class
		 * @tparam ...Args The arguments to pass to the constructor
		 * @param name The name of the class to instantiate
		 * @param ...args The arguments to pass to the constructor
		 * @return A unique instance of the class
		 */
		template <class... Args>
		static std::unique_ptr<Base> Create(const std::string_view name, Args&&... args)
		{
			std::string nameStr(name);
			return GetStaticData().at(nameStr)(std::forward<Args>(args)...);
		}

		/**
		 * @brief Get the static map of registered classes
		 * @return The static map of registered classes
		 */
		const static FactoryMap& GetFactoryData()
		{
			return GetStaticData();
		}

	private:
		Factory() = default;

		static FactoryMap& GetStaticData()
		{
			static FactoryMap data;
			return data;
		}
	};

	template <class Base, class... Args>
	template <class T>
	bool Factory<Base, Args...>::Registrar<T>::s_registered =
		Factory<Base, Args...>::Registrar<T>::Register();
}
