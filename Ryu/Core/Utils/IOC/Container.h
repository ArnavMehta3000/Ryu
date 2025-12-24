#pragma once
#include "Core/Common/Result.h"
#include "Core/Utils/Singleton.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <any>
#include <format>

namespace Ryu::IOC
{
	template <typename T>
	concept IsParamterized = requires() { typename T::IOCParams{} };

	template <typename T>
	concept IsNotParamterized = !IsParamterized<T>;


	class Container : Utils::Singleton<Container>
	{
	public:
		template <typename T> 
		using Generator = std::function<std::shared_ptr<T>()>;

		template <typename T>
		using ParameterizedGenerator = std::function<std::shared_ptr<T>(typename T::IOCParams params)>;

		template <typename T> 
		using ResolveResult = Result<std::shared_ptr<T>>;

	public:
		template <IsNotParamterized T>
		void RegisterFactory(Generator<T> generator)
		{
			m_factoryMap[typeid(T)] = std::move(generator);
		}

		template <IsParamterized T>
		void RegisterFactory(ParameterizedGenerator<T> generator)
		{
			m_factoryMap[typeid(T)] = std::move(generator);
		}
		
		template <IsNotParamterized T>
		ResolveResult<T> Resolve()
		{
			return ResolveInternal<T, Container::Generator<T>>(); 
		}
		
		template <IsParamterized T>
		ResolveResult<T> Resolve(typename T::IOCParams&& params = {}) 
		{
			return ResolveInternal<T, Container::ParameterizedGenerator<T>>(std::forward<typename T::IOCParams>(params));
		}

	private:
		template <typename T, typename G, typename... Args>
		ResolveResult<T> ResolveInternal(Args&&... args);

	private:
		std::unordered_map<std::type_index, std::any> m_factoryMap;
	};
	

	template<typename T, typename G, typename ...Args>
	inline Container::ResolveResult<T> Container::ResolveInternal(Args&&... args)
	{
		if (const auto it = m_factoryMap.find(typeid(T)); it != m_factoryMap.end())
		{
			try
			{
				return std::any_cast<G>(it->second)(std::forward<Args>(args)...);
			}
			catch (const std::bad_any_cast&)
			{
				return std::format("Failed to resolve mapped type from [{}] to [{}]",
					it->second.type().name(), typeid(G).name());
			}
		}
		else
		{
			return std::format("Failed to find generator for type [{}] in IOC container!", typeid(T).name());
		}
	}
}

#define RYU_IOC_PARAMS(...) struct IOCParams { __VA_ARGS__ }
