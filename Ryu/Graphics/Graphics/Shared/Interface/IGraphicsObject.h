#pragma once
#include "Graphics/Shared/ComPtr.h"
#include <any>
#include <optional>
#include <memory>

namespace Ryu::Graphics
{
	class Renderer;

	using NativeObjectType = std::any;

	class IGraphicsObject
	{
	public:
		virtual ~IGraphicsObject() = default;
		virtual NativeObjectType GetNativeObject() const = 0;

		template<typename T>
		std::optional<T*> GetNativeObjectAs() const
		{
			try
			{
				return std::any_cast<T>(GetNativeObject());
			}
			catch (const std::bad_any_cast&)
			{
				return std::nullopt;
			}
		}

		inline void SetRenderer(Renderer* renderer) { m_renderer = renderer; }
		Renderer* GetRenderer() const { return m_renderer; }

	private:
		Renderer* m_renderer{ nullptr };
	};

}

#define RYU_DECLARE_GFX_NATIVE_TYPE(Type) using NativeType = Type
#define RYU_GET_GFX_NATIVE_TYPE(Object, Type) Object->GetNativeObjectAs<Type>().value_or(nullptr)
