#pragma once
#include "Common/ObjectMacros.h"
#include <any>
#include <map>
#include <string>
#include <typeinfo>
#include <libassert/assert.hpp>

namespace Ryu::Utils
{
	/**
	 * @brief Registry for singletons
	 */
	class SingletonRegistry
	{

	public:
		
		/**
		 * @brief Singleton entry
		 */
		struct SingletonEntry
		{
			/**
			 * @brief Name of the singleton class
			 */
			std::string Name;

			/**
			 * @brief Singleton instance
			 */
			std::unique_ptr<std::any> Instance{ nullptr };
		};

		using SingletonMap = std::map<size_t, SingletonEntry>;

		/**
		 * @brief Get the singleton instance without checking
		 * @tparam T The type of the singleton instance
		 * @return The singleton instance. nullptr if not found
		 */
		template <typename T>
		inline static T* Get()
		{
			try
			{
				auto val = std::any_cast<T*>(*s_singletons[GetHash<T>()].Instance);
				return val;
			}
			catch (const std::bad_any_cast&)
			{
				return nullptr;
			}
		}
				
		/**
		 * @brief Get the singleton instance with checking
		 * @tparam T The type of the singleton instance
		 * @return The singleton instance
		 */
		template <typename T>
		inline static T* GetChecked()
		{
			auto ptr = Get<T>();
			ASSERT(ptr, "Singleton type was not registered!", typeid(T).name());
			return ptr;
		}

		/**
		 * @brief Register a singleton instance using a unique pointer
		 * @tparam T The type of the singleton
		 * @param instance The unique pointer
		 */
		template <typename T>
		inline static void Register(std::unique_ptr<std::any> instance)
		{
			DEBUG_ASSERT(instance, "Invalid instance pointer");
			DEBUG_ASSERT(!s_singletons[GetHash<T>()].Instance,
				"Singleton for type has already been registered!", s_singletons);

			s_singletons[GetHash<T>()] = { typeid(T).name(), std::move(instance) };
		}

		/**
		 * @brief Register a singleton instance using a raw pointer
		 * @tparam T The type of the singleton
		 * @param instance The raw pointer
		 */
		template <typename T>
		inline static void Register(T* instance)
		{
			DEBUG_ASSERT(instance, "Invalid instance pointer");
			DEBUG_ASSERT(!s_singletons[GetHash<T>()].Instance,
				"Singleton for type has already been registered!", s_singletons);

			s_singletons[GetHash<T>()] = { typeid(T).name(), std::make_unique<std::any>(instance) };
		}

		/**
		 * @brief Register a singleton by creating a instance in place
		 * @tparam T The type of the singleton
		 */
		template <typename T>
		inline static void Register()
		{
			DEBUG_ASSERT(!s_singletons[GetHash<T>()].Instance,
				"Singleton for type has already been registered!", s_singletons);

			s_singletons[GetHash<T>()] = { typeid(T).name(), std::make_unique<std::any>(new T()) };
		}

		/**
		 * @brief Unregister a singleton
		 * @tparam T The type of the singleton
		 */
		template <typename T>
		inline static void Unregister()
		{
			DEBUG_ASSERT(s_singletons[GetHash<T>()].Instance,
				"Singleton for type has not been registered!", s_singletons);

			s_singletons[GetHash<T>()].Instance.reset();
			s_singletons.erase(GetHash<T>());
		}

		/**
		 * @brief Get all registered singletons
		 * @return The registry map
		 */
		static const SingletonMap& GetAllRegisteredSingletons();

	private:
		template <typename T> friend class SingletonRegistrar;

		template <typename T>
		inline static size_t GetHash()
		{
			static const size_t hash = typeid(T).hash_code();
			return hash;
		}

		static SingletonMap s_singletons;
	};

	template <class T>
	class SingletonRegistrar
	{
	public:
		inline SingletonRegistrar(T* type)
		{
			type->RegisterSingleton();
		}

		inline ~SingletonRegistrar()
		{
			T::UnregisterSingleton();
		}
	};
}


#define RYU_DECLARE_SINGLETON(Type, ShouldSelfRegister)                    \
public:                                                                    \
	static inline Type* Get() { return s_singletonInstance; }              \
private:                                                                   \
	RYU_DISABLE_COPY_AND_MOVE(Type)                                        \
	void RegisterSingleton()                                               \
	{                                                                      \
		s_singletonInstance = this;                                        \
		if constexpr (ShouldSelfRegister)                                  \
		{                                                                  \
			::Ryu::Utils::SingletonRegistry::Register<Type>(this);         \
		}                                                                  \
	}                                                                      \
	                                                                       \
	static void UnregisterSingleton()                                      \
	{                                                                      \
		if (s_singletonInstance)                                           \
		{                                                                  \
			::Ryu::Utils::SingletonRegistry::Unregister<Type>();           \
			s_singletonInstance = nullptr;                                 \
		}                                                                  \
	}                                                                      \
	friend class ::Ryu::Utils::SingletonRegistrar<Type>;                   \
	::Ryu::Utils::SingletonRegistrar<Type> m_singletonRegistrar{this};     \
	static Type* s_singletonInstance

#define RYU_SINGLETON_IMPL(Type) Type* Type::s_singletonInstance = nullptr
