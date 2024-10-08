#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Ryu::Utils
{
	template <class Base, class... Args>
	class Factory
	{
		friend Base;
	public:
		template <class T>
		struct Registrar : Base
		{
			friend T;
		public:
			static bool Register()
			{
				const std::string name(T::GetStaticName());
				Factory::GetData()[name] = [](Args... args) -> std::unique_ptr<Base>
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
		template <class... T>
		static std::unique_ptr<Base> Create(const std::string_view name, T&&... args)
		{
			std::string nameStr(name);
			return GetData().at(nameStr)(std::forward<T>(args)...);
		}

	private:
		using FuncType = std::unique_ptr<Base>(*)(Args...);
		Factory() = default;

		static std::unordered_map<std::string, FuncType>& GetData()
		{
			static std::unordered_map<std::string, FuncType> data;
			return data;
		}
	};

	template <class Base, class... Args>
	template <class T>
	bool Factory<Base, Args...>::Registrar<T>::s_registered =
		Factory<Base, Args...>::Registrar<T>::Register();
}
