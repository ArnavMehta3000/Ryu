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
		std::optional<ComPtr<T>> GetNativeObjectAs() const
		{
			try
			{
				return std::any_cast<ComPtr<T>>(GetNativeObject());
			}
			catch (const std::bad_any_cast&)
			{
				return std::nullopt;
			}
		}

		template<typename T>
		T* GetNativeObjectRawPtr() const
		{
			try
			{
				return std::any_cast<ComPtr<T>>(GetNativeObject()).Get();
			}
			catch (const std::bad_any_cast&)
			{
				return nullptr;
			}
		}

		inline void SetRenderer(std::shared_ptr<Renderer> renderer) { m_renderer = renderer; }
		std::shared_ptr<Renderer> GetRenderer() const { return m_renderer.lock(); }

	private:
		std::weak_ptr<Renderer> m_renderer;
	};
}
