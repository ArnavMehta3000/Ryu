#pragma once
#include "Common/Concepts.h"
#include "Utils/StringConv.h"
#include "GraphicsRHI/Utils/ComPtr.h"
#include "GraphicsRHI/Utils/Logging.h"
#include "GraphicsRHI/API.h"
#include <any>
#include <optional>
#include <memory>

struct ID3D11Device;
struct ID3D12Device;
struct ID3D11DeviceChild;
struct ID3D12Object;
struct IDXGIObject;

namespace Ryu::Graphics
{
	class IRenderer;
	using NativeObjectType = std::any;

	template<typename T>
	concept DX11Object = std::derived_from<T, ID3D11DeviceChild> || std::derived_from<T, ID3D11Device>;

	template<typename T>
	concept DX12Object = std::derived_from<T, ID3D12Object> || std::derived_from<T, ID3D12Device>;

	template<typename T>
	concept DXGIObject = std::derived_from<T, IDXGIObject>;

	template<typename T>
	concept DirectXObject = DX11Object<T> || DX12Object<T> || DXGIObject<T>;

	template<typename T>
	struct GetAPITypeT
	{
		static constexpr API Value =
			DX11Object<T> ? API::DirectX11 :
			DX12Object<T> ? API::DirectX12 :
			DXGIObject<T> ? API::DirectX11 :
			API::DirectX11;
	};

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
				return std::any_cast<T*>(GetNativeObject());
			}
			catch (const std::bad_any_cast&)
			{
				return std::nullopt;
			}
		}

		inline void SetRendererInterface(IRenderer* renderer) { m_renderer = renderer; }
		inline IRenderer* GetRendererInterface() const { return m_renderer; }

	private:
		IRenderer* m_renderer{ nullptr };
	};
}

// Declare the native type of the graphics object
#define RYU_DECLARE_GFX_NATIVE_TYPE(Type) using NativeType = Type

// Declare native type implicit conversion operator
#define RYU_DECLARE_GFX_NATIVE_TYPE_OP(ReturnValue)                        \
private:                                                                   \
inline NativeObjectType GetNativeObject() const { return ReturnValue; }    \
inline NativeType* GetNamedNativeObject() const { return ReturnValue; }    \
public:                                                                    \
operator NativeType*() const { return ReturnValue; }

// Get native type of the graphics object
#define RYU_GET_GFX_NATIVE_TYPE(Object, Type) (Object)->GetNativeObjectAs<Type>().value_or(nullptr)

namespace Ryu::Graphics
{
	template<DirectXObject TNative, API TApi = GetAPITypeT<TNative>::Value>
	class IGraphicsRHIObject 
	{
	public:
		virtual ~IGraphicsRHIObject() = default;
		RYU_DECLARE_GFX_NATIVE_TYPE(TNative);

		void SetName(std::string_view name)
		{
			if (auto* native = GetNamedNativeObject())
			{
				SetNameImpl(native, name);
			}
		}

	protected:
		virtual inline TNative* GetNamedNativeObject() const = 0;

	private:
		void SetNameImpl(TNative* native, std::string_view name)
		{
			if constexpr (DX12Object<TNative>)
			{
				const std::string nameStr(name);
				DX12_NAME_OBJECT(native, Utils::ToWideStr(nameStr));
			}
			else if constexpr (DX11Object<TNative> || DXGIObject<TNative>)
			{
				DX11_NAME_OBJECT(native, name);
			}
		}
	};
}
