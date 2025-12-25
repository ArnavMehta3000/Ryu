#pragma once
#include "Asset/AssetHandle.h"

namespace Ryu::Asset
{
	template <typename T>
	class AssetManager;

	template<typename T>
	class AssetPtr
	{
		RYU_DISABLE_COPY(AssetPtr)
	public:
		AssetPtr() = default;
		AssetPtr(AssetManager<T>* manager, AssetHandle<T> handle);
		~AssetPtr();

		AssetPtr(AssetPtr&& other) noexcept;
		AssetPtr& operator=(AssetPtr&& other) noexcept
		{
			if (this != &other)
			{
				Release();
				m_manager       = other.m_manager;
				m_handle        = other.m_handle;
				m_resource      = std::move(other.m_resource);
				other.m_manager = nullptr;
				other.m_handle  = {};
			}
			return *this;
		}

		void Release();

		T* operator->() { return m_resource.get(); }
		const T* operator->() const { return m_resource.get(); }

		T& operator*() { return *m_resource; }
		const T& operator*() const { return *m_resource; }

		T* Get() { return m_resource.get(); }
		const T* Get() const { return m_resource.get(); }

		explicit operator bool() const { return m_resource != nullptr; }
		bool IsValid() const { return m_resource != nullptr; }

		[[nodiscard]] inline AssetHandle<T> GetHandle() const { return m_handle; }

	private:
		AssetManager<T>*   m_manager = nullptr;
		AssetHandle<T>     m_handle;
		std::shared_ptr<T> m_resource;
	};
	
	template<typename T>
	inline AssetPtr<T>::AssetPtr(AssetManager<T>* manager, AssetHandle<T> handle)
		: m_manager(manager)
		, m_handle(handle)
	{
		if (m_manager && m_handle.IsValid())
		{
			m_resource = m_manager->Get(m_handle);
		}
	}
	
	template<typename T>
	inline AssetPtr<T>::~AssetPtr()
	{
		Release();
	}
	
	template<typename T>
	inline AssetPtr<T>::AssetPtr(AssetPtr&& other) noexcept
		: m_manager(other.m_manager)
		, m_handle(other.m_handle)
		, m_resource(std::move(other.m_resource))
	{
		other.m_manager = nullptr;
		other.m_handle = {};
	}
	
	template<typename T>
	inline void AssetPtr<T>::Release()
	{
		if (m_manager && m_resource)
		{
			m_manager->Unload(m_handle);
			m_resource.reset();
			m_manager = nullptr;
		}
	}
}
