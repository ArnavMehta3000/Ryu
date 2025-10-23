#pragma once
#include <wrl/client.h>
#include <type_traits>
#include <concepts>
#include <memory>
#include <span>

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

	template<typename T>
	concept Releasable = requires (T t)
	{
		{ t.Release() };
	};

	struct ReleaseDeleter
	{
		template<Releasable T>
		void operator()(T* allocation)
		{
			allocation->Release();
		}
	};

	template<Releasable T>
	using ReleasablePtr = std::unique_ptr<T, ReleaseDeleter>;

	// Make a unique_ptr of a ComPtr
	template<Releasable T>
	ReleasablePtr<T> MakeReleasablePtr(T* ptr)
	{
		return ReleasablePtr<T>(ptr);
	}

	struct ReleasableObject
	{
		virtual ~ReleasableObject() = default;
		virtual void Release() = 0;
	};

	template<Releasable T>
	struct ReleasableResource : ReleasableObject
	{
		ReleasableResource(T* r)
		{
			resource = r;
		}

		virtual ~ReleasableResource()
		{
			Release();
		}

		virtual void Release() override
		{
			if (resource) 
			{
				resource->Release();
				resource = nullptr;
			}
		}

		T* resource;
	};
}
