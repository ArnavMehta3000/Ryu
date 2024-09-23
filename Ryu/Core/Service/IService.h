#pragma once
#include <CoreDLLDefines.h>
#include <Core/Concepts.h>
#include <typeindex>
namespace Ryu
{
	namespace Internal
	{
		class RYU_API IService
		{
		public:
			using TypeIndex = std::type_index;
			virtual ~IService() = default;
			virtual TypeIndex GetTypeIndex() const = 0;
		};
	}

	template <typename T>
	class RYU_API ServiceBase : public T
	{
	public:
		Internal::IService::TypeIndex GetTypeIndex() const override
		{
			return std::type_index(typeid(T));
		};
	};
}
