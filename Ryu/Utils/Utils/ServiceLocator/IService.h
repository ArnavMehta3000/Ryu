#pragma once
#include <typeindex>

namespace Ryu
{
	/**
	 * @brief An interface for a service type
	 * @details Create an interface for a service type
	 */
	class IService
	{
	public:
		using TypeIndex = std::type_index;
		virtual ~IService() = default;
		virtual TypeIndex GetTypeIndex() const = 0;
	};

	/**
	 * @brief A base class for services
	 * @tparam T The class that implements the service
	 */
	template <typename T>
	class ServiceBase : public T
	{
	public:
		IService::TypeIndex GetTypeIndex() const override
		{
			return std::type_index(typeid(T));
		};
	};
}
