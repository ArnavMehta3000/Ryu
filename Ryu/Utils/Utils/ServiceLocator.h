#pragma once
#include "Common/Common.h"
#include <mutex>
#include <memory>
#include <expected>
#include <typeindex>
#include <functional>
#include <unordered_map>

#if defined(RYU_BUILD_DEBUG)
#include <source_location>
#endif

#include <ServiceErrorType.Generated.h>

namespace Ryu::Utils
{
	// Base interface for services
	struct IService { virtual ~IService() = default; };

	template <typename T>
	concept ServiceType = std::is_base_of_v<IService, T>;

	class ServiceLocator
	{
		RYU_DISABLE_COPY_AND_MOVE(ServiceLocator)

		class IServiceHolder
		{
		public:
			virtual ~IServiceHolder() = default;
			virtual void* GetRaw() = 0;
		};

		template <ServiceType T>
		class ServiceHolder final : public IServiceHolder
		{
		public:
			template<typename Factory>
			ServiceHolder(Factory&& factory) : m_factory(std::forward<Factory>(factory)) {}
			
			inline void* GetRaw() override
			{
				CreateIfNeeded();
				return m_instance.get();
			}

			inline T* Get()
			{
				CreateIfNeeded();
				return m_instance.get();
			}

		private:
			void CreateIfNeeded()
			{
				if (!m_instance)
				{
					m_instance = m_factory();
				}
			}

		private:
			std::function<std::unique_ptr<T>()> m_factory;
			std::unique_ptr<T> m_instance;
		};

		struct ServiceInfo
		{
			std::unique_ptr<IServiceHolder> Holder;
#if defined(RYU_BUILD_DEBUG)
			std::source_location RegistrationLocation;
#endif
    	};

	public:
		ServiceLocator() = default;
		~ServiceLocator() { m_services.clear(); }

		template <ServiceType T, typename Factory>
			requires std::invocable<Factory> && std::same_as<std::invoke_result_t<Factory>, std::unique_ptr<T>>
#if defined(RYU_BUILD_DEBUG)
		std::expected<void, ServiceError> RegisterService(
			Factory&& factory,
			std::source_location loc = std::source_location::current())
#else
		std::expected<void, ServiceError> RegisterService(
			Factory&& factory)
#endif
		{
			std::type_index typeId = std::type_index(typeid(T));

			{
				std::lock_guard<std::mutex> lock(m_servicesMutex);

				if (m_services.contains(typeId))
				{
					return std::unexpected(ServiceError::AlreadyRegistered);
				}

				ServiceInfo info;
				info.Holder = std::make_unique<ServiceHolder<T>>(std::forward<Factory>(factory));

#if defined(RYU_BUILD_DEBUG)
				info.RegistrationLocation = loc;
#endif

				m_services[typeId] = std::move(info);
			}

			return {};
		}


		template <ServiceType T>
		std::expected<T*, ServiceError> GetService()
		{
			std::lock_guard<std::mutex> lock(m_servicesMutex);

			auto it = m_services.find(std::type_index(typeid(T)));
			if (it == m_services.end())
			{
				return std::unexpected(ServiceError::NotFound);
			}

			auto* holder = static_cast<ServiceHolder<T>*>(it->second.Holder.get());
			// The holder will create if needed
			if (auto* service = holder->Get())
			{
				return service;
			}

			return std::unexpected(ServiceError::CreationFailed);
		}

		template <ServiceType T>
		bool HasService() const 
		{
			std::lock_guard<std::mutex> lock(m_servicesMutex);
			return m_services.contains(std::type_index(typeid(T)));
		}
		
		template <ServiceType T>
		void UnregisterService()
		{
			std::lock_guard<std::mutex> lock(m_servicesMutex);
			m_services.erase(std::type_index(typeid(T)));
		}

	private:
		mutable std::mutex m_servicesMutex;
		std::unordered_map<std::type_index, ServiceInfo> m_services;
	};

	// NOTE: "Refer to Globals/Globals.h for static service registration"
}
