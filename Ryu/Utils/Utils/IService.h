#pragma once
#include <typeindex>

namespace Ryu
{
	class IService
	{
	public:
		using TypeIndex = std::type_index;
		virtual ~IService() = default;
		virtual TypeIndex GetTypeIndex() const = 0;
	};

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
