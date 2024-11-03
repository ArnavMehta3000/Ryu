#pragma once
#include "Utils/ServiceLocator/IService.h"
#include <map>
#include <memory>
#include <stdexcept>

namespace Ryu::Utils
{
	class ServiceLocator
	{
	public:
		template<typename T>
		void RegisterService(std::shared_ptr<T> service)
		{
			static_assert(std::is_base_of<IService, T>::value, "T must inherit from IService");
			
			IService::TypeIndex idx = service->GetTypeIndex();
			if (m_services.find(idx) != m_services.end())
			{
				throw std::runtime_error("Service already registered!");
			}
			m_services[idx] = service;
		}

		template<typename T>
		void UnregisterService()
		{
			IService::TypeIndex idx(typeid(T));
			if (m_services.find(idx) == m_services.end())
			{
				throw std::runtime_error("Service not registered!");
			}

			m_services.erase(idx);
		}

		template<typename T>
		std::shared_ptr<T> ResolveService()
		{
			IService::TypeIndex idx(typeid(T));
			auto it = m_services.find(idx);
			if (it == m_services.end())
			{
				return nullptr;
			}

			return std::static_pointer_cast<T>(it->second);
		}
	private:
		std::map< IService::TypeIndex, std::shared_ptr<IService>> m_services;
	};
}
