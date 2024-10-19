#pragma once
#include "Common/ObjectMacros.h"
#include <any>
#include <map>
#include <string>
#include <typeinfo>
#include <libassert/assert.hpp>

namespace Ryu::Utils
{
	class SingletonRegistry
	{

	public:
		struct SingletonEntry
		{
			std::string Name;
			std::any Instance;
		};

		using SingletonMap = std::map<size_t, SingletonEntry>;

		template <typename T>
		inline static T* Get()
		{
			try
			{
				auto val = std::any_cast<T*>(s_singletons[GetHash<T>()].Instance);
				return val;
			}
			catch (const std::bad_any_cast&)
			{
				return nullptr;
			}
		}

		template <typename T>
		inline static T* GetChecked()
		{
			auto ptr = Get<T>();
			ASSERT(ptr, "Singleton type was not registered!", typeid(T).name());
			return ptr;
		}

		template <typename T>
		inline static void Register(T* instance)
		{
			DEBUG_ASSERT(instance, "Invalid instance pointer");
			DEBUG_ASSERT(!s_singletons[GetHash<T>()].Instance.has_value(),
				"Singleton for type has already been registered!", s_singletons);

			s_singletons[GetHash<T>()] = { typeid(T).name(), instance };
		}

		template <typename T>
		inline static void Unregister()
		{
			DEBUG_ASSERT(s_singletons[GetHash<T>()].Instance.has_value(),
				"Singleton for type has not been registered!", s_singletons);
			s_singletons.erase(GetHash<T>());
		}

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


#define RYU_DECLARE_SINGLETON(Type)                                    \
public:                                                                \
	static inline Type* Get() { return s_singletonInstance; }          \
private:                                                               \
	RYU_DISABLE_COPY_AND_MOVE(Type)                                    \
	void RegisterSingleton()                                           \
	{                                                                  \
		s_singletonInstance = this;                                    \
		::Ryu::Utils::SingletonRegistry::Register<Type>(this);         \
	}                                                                  \
	                                                                   \
	static void UnregisterSingleton()                                  \
	{                                                                  \
		if (s_singletonInstance)                                       \
		{                                                              \
			::Ryu::Utils::SingletonRegistry::Unregister<Type>();       \
			s_singletonInstance = nullptr;                             \
		}                                                              \
	}                                                                  \
	friend class ::Ryu::Utils::SingletonRegistrar<Type>;               \
	::Ryu::Utils::SingletonRegistrar<Type> m_singletonRegistrar{this}; \
	static Type* s_singletonInstance

#define RYU_SINGLETON_IMPL(Type) Type* Type::s_singletonInstance = nullptr
