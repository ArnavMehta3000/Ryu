#pragma once
#include <wrl/client.h>
#include <type_traits>
#include <concepts>

namespace Ryu::Gfx
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	namespace Internal
	{
		template <typename T>
		struct IsComPtr : std::false_type {};

		template<typename U>
		struct IsComPtr<ComPtr<U>> : std::true_type {};

		template<typename T>
		concept ComPtrType = IsComPtr<std::remove_cvref_t<T>>::value;

		template <typename T>
		concept RawComPtrType = std::is_pointer_v<T>
			&& std::is_class_v<std::remove_pointer_t<T>>
			&& requires(T p) { p->Release(); };
	}
	

	// Release a ComPtr directly by calling Reset
	template<Internal::ComPtrType T>
	inline void ComRelease(T& ptr)
	{
		ptr.Reset();
	}


	/* If using this with ComPtr<T>, then ensure to call Detach() and then pass the l-value. Eg:
	auto* ptr = comPtr.Detach();
	ComRelease(ptr);
	*/
	template<Internal::RawComPtrType T>
	inline void ComRelease(T& ptr)
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
}
