#pragma once
#include "Core/Common/StandardTypes.h"
#include <utility>
#include <concepts>
#include <atomic>

namespace Ryu::Memory
{
	template <typename T> class Ref;
	template <typename T> class WeakRef;

	// Control block for managing reference counts
	namespace Internal
	{
		class RefControlBlock
		{
		public:
			RefControlBlock();

			void AddStrongRef();
			bool ReleaseStrongRef();
			u32 GetStrongCount() const;
			void AddWeakRef();
			bool ReleaseWeakRef();
			u32 GetWeakCount() const;
			bool IsAlive() const;
			void MarkExpired();
			bool IsExpired() const;

		private:
			std::atomic<u32> m_strongCount;
			std::atomic<u32> m_weakCount;
			std::atomic<bool> m_expired;
		};
	}

	// Base class for reference counted objects
	class IRefCounted
	{
	public:
		virtual ~IRefCounted() = default;

		virtual u32 AddRef() const = 0;
		virtual u32 Release() const = 0;
		virtual u32 GetRefCount() const = 0;
		virtual void AddWeakRef() const = 0;
		virtual void ReleaseWeakRef() const = 0;
		virtual u32 GetWeakCount() const = 0;
		virtual Internal::RefControlBlock* GetControlBlock() const = 0;
	};

	// Concept for classes that can be reference and weak-reference counted
	template <typename T>
	concept RefCountable = requires(T * t)
	{
		{ t->AddRef() } -> std::same_as<u32>;
		{ t->Release() } -> std::same_as<u32>;
		{ t->GetRefCount() } -> std::same_as<u32>;
		{ t->AddWeakRef() };
		{ t->ReleaseWeakRef() };
		{ t->GetWeakCount() } -> std::same_as<u32>;
		{ t->GetControlBlock() } -> std::same_as<Internal::RefControlBlock*>;
	};

	// Mixin class that implements reference counting
	class RefCounted : public IRefCounted
	{
	public:
		RefCounted();
		RefCounted(const RefCounted&);
		virtual ~RefCounted();
		
		RefCounted& operator=(const RefCounted&) { return *this; }

		u32 AddRef() const override;
		u32 Release() const override;
		u32 GetRefCount() const override;
		void AddWeakRef() const override;
		void ReleaseWeakRef() const override;
		u32 GetWeakCount() const override;
		Internal::RefControlBlock* GetControlBlock() const override;

	private:
		mutable Internal::RefControlBlock* m_controlBlock;
	};

	template <typename T>
	class Ref
	{
		template<typename U> friend class Ref;
		template<typename U> friend class WeakRef;

	public:
		constexpr Ref() noexcept = default;
		constexpr Ref(std::nullptr_t) noexcept {}
		explicit Ref(T* ptr) noexcept : m_ptr(ptr) { IncrementRefCount(); }
		Ref(const Ref& other) noexcept : m_ptr(other.m_ptr) { IncrementRefCount(); }
		Ref(Ref&& other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

		template<typename U> requires std::convertible_to<U*, T*>
		Ref(const Ref<U>& other) noexcept : m_ptr(other.Get()) { IncrementRefCount(); }

		template<typename U> requires std::convertible_to<U*, T*>
		Ref(Ref<U>&& other) noexcept : m_ptr(other.Release()) {}

		virtual ~Ref() { DecrementRefCount(); }

		template<typename U, typename... Args>
		static Ref<U> Create(Args&&... args) { return Ref<U>(new U(std::forward<Args>(args)...)); }

		T* Get() const noexcept { return m_ptr; }

		T* Release() noexcept 
		{
			T* ptr = m_ptr;
			m_ptr = nullptr;
			return ptr;
		}

		void Reset(T* ptr = nullptr) noexcept 
		{
			if (m_ptr != ptr) 
			{
				DecrementRefCount();
				m_ptr = ptr;
				IncrementRefCount();
			}
		}

		void Swap(Ref& other) noexcept 
		{
			std::swap(m_ptr, other.m_ptr);
		}

		u32 GetRefCount() const noexcept 
		{
			if (m_ptr)
			{
				if constexpr (RefCountable<T>) 
				{
					return m_ptr->GetRefCount();
				}
				else if constexpr (std::derived_from<T, IRefCounted>) 
				{
					return m_ptr->GetRefCount();
				}
			}
			return 0;
		}

		u32 GetWeakCount() const noexcept
		{
			if (m_ptr)
			{
				if constexpr (RefCountable<T>)
				{
					return m_ptr->GetWeakCount();
				}
				else if constexpr (std::derived_from<T, IRefCounted>)
				{
					return m_ptr->GetWeakCount();
				}
			}
			return 0;
		}

		Ref& operator=(const Ref& other) noexcept
		{
			if (this != &other) 
			{
				Ref(other).Swap(*this);
			}
			return *this;
		}

		Ref& operator=(Ref&& other) noexcept
		{
			if (this != &other)
			{
				DecrementRefCount();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		template<typename U> requires std::convertible_to<U*, T*>
		Ref& operator=(const Ref<U>& other) noexcept
		{
			Ref(other).Swap(*this);
			return *this;
		}

		template<typename U> requires std::convertible_to<U*, T*>
		Ref& operator=(Ref<U>&& other) noexcept
		{
			Ref(std::move(other)).Swap(*this);
			return *this;
		}

		Ref& operator=(std::nullptr_t) noexcept 
		{
			DecrementRefCount();
			m_ptr = nullptr;
			return *this;
		}

		T* operator->() const noexcept { return m_ptr; }
		T& operator*() const noexcept { return *m_ptr; }

		explicit operator bool() const noexcept { return m_ptr != nullptr; }

		bool operator==(const Ref& other) const noexcept { return m_ptr == other.m_ptr; }
		bool operator!=(const Ref& other) const noexcept { return m_ptr != other.m_ptr; }
		bool operator==(std::nullptr_t) const noexcept { return m_ptr == nullptr; }
		bool operator!=(std::nullptr_t) const noexcept { return m_ptr != nullptr; }

	private:
		void IncrementRefCount() const noexcept
		{
			if (m_ptr) 
			{
				if constexpr (RefCountable<T>) 
				{
					m_ptr->AddRef();
				}
				else if constexpr (std::derived_from<T, IRefCounted>) 
				{
					m_ptr->AddRef();
				}
			}
		}

		void DecrementRefCount() const noexcept
		{
			if (m_ptr)
			{
				if constexpr (RefCountable<T>) 
				{
					m_ptr->Release();
				}
				else if constexpr (std::derived_from<T, IRefCounted>) 
				{
					m_ptr->Release();
				}
			}
		}

	private:
		T* m_ptr = nullptr;
	};

	template<typename T, typename... Args> Ref<T> CreateRef(Args&&... args) { return Ref<T>(new T(std::forward<Args>(args)...)); }
	template<typename T> Ref<T> MakeRef(const Ref<T>& ref) { return Ref<T>(ref); }
	template<typename T> Ref<T> MakeRef(T* ptr) { return Ref<T>(ptr); }
	template<typename To, typename From> Ref<To> RefCast(const Ref<From>& ref) { return Ref<To>(dynamic_cast<To*>(ref.Get())); }

	template<typename T, typename U> bool operator==(const Ref<T>& lhs, const Ref<U>& rhs) noexcept { return lhs.Get() == rhs.Get(); }
	template<typename T, typename U> bool operator!=(const Ref<T>& lhs, const Ref<U>& rhs) noexcept { return lhs.Get() != rhs.Get(); }
	template<typename T> bool operator==(const Ref<T>& ref, std::nullptr_t) noexcept { return !ref; }
	template<typename T> bool operator!=(const Ref<T>& ref, std::nullptr_t) noexcept { return (bool)ref; }
	template<typename T> bool operator==(std::nullptr_t, const Ref<T>& ref) noexcept { return !ref; }
	template<typename T> bool operator!=(std::nullptr_t, const Ref<T>& ref) noexcept { return (bool)ref; }


	template <typename T>
	class WeakRef
	{
		template<typename U> friend class WeakRef;

	public:
		constexpr WeakRef() noexcept = default;
		constexpr WeakRef(std::nullptr_t) noexcept {}

		WeakRef(const Ref<T>& ref) noexcept 
			: m_ptr(ref.m_ptr)
			, m_controlBlock(nullptr)
		{
			if (m_ptr)
			{
				if constexpr (RefCountable<T>)
				{
					m_controlBlock = m_ptr->GetControlBlock();
					m_ptr->AddWeakRef();
				}
				else if constexpr (std::derived_from<T, IRefCounted>)
				{
					m_controlBlock = m_ptr->GetControlBlock();
					m_ptr->AddWeakRef();
				}
			}
		}

		WeakRef(const WeakRef& other) noexcept
			: m_ptr(other.m_ptr)
			, m_controlBlock(other.m_controlBlock)
		{
			IncrementWeakCount();
		}

		WeakRef(WeakRef&& other) noexcept
			: m_ptr(other.m_ptr)
			, m_controlBlock(other.m_controlBlock)
		{
			other.m_ptr = nullptr;
			other.m_controlBlock = nullptr;
		}

		template<typename U> requires std::convertible_to<U*, T*>
		WeakRef(const WeakRef<U>& other) noexcept
			: m_ptr(other.m_ptr)
			, m_controlBlock(other.m_controlBlock)
		{
			IncrementWeakCount();
		}

		template<typename U> requires std::convertible_to<U*, T*>
		WeakRef(WeakRef<U>&& other) noexcept
			: m_ptr(other.m_ptr)
			, m_controlBlock(other.m_controlBlock)
		{
			other.m_ptr = nullptr;
			other.m_controlBlock = nullptr;
		}

		~WeakRef() { DecrementWeakCount(); }

		void IncrementWeakCount() noexcept
		{
			if (m_controlBlock)
			{
				m_controlBlock->AddWeakRef();
			}
		}

		void DecrementWeakCount() noexcept
		{
			if (m_controlBlock)
			{
				if constexpr (RefCountable<T> || std::derived_from<T, IRefCounted>)
				{
					// Instead of using m_ptr which may be dangling, use the control block directly
					const bool shouldDeleteControlBlock = m_controlBlock->ReleaseWeakRef() && m_controlBlock->GetStrongCount() == 0;
					if (shouldDeleteControlBlock)
					{
						delete m_controlBlock;
						m_controlBlock = nullptr;  // Prevent dangling pointer
					}
				}
			}
		}

		void Reset() noexcept
		{
			DecrementWeakCount();
			m_ptr = nullptr;
			m_controlBlock = nullptr;
		}

		// Attempt to lock the weak reference and get a strong reference
		Ref<T> Lock() const noexcept
		{
			if (IsValid())
			{
				return Ref<T>(m_ptr);
			}
			return Ref<T>(nullptr);
		}

		bool IsValid() const noexcept
		{
			return m_controlBlock && m_controlBlock->IsAlive();
		}

		bool IsExpired() const noexcept
		{
			return !IsValid();
		}

		u32 GetWeakCount() const noexcept
		{
			if (m_controlBlock)
			{
				return m_controlBlock->GetWeakCount();
			}
			return 0;
		}

		u32 GetRefCount() const noexcept
		{
			if (m_controlBlock)
			{
				return m_controlBlock->GetStrongCount();
			}
			return 0;
		}

		void Swap(WeakRef& other) noexcept
		{
			std::swap(m_ptr, other.m_ptr);
			std::swap(m_controlBlock, other.m_controlBlock);
		}

		WeakRef& operator=(const WeakRef& other) noexcept
		{
			if (this != &other)
			{
				WeakRef(other).Swap(*this);
			}
			return *this;
		}

		WeakRef& operator=(WeakRef&& other) noexcept
		{
			if (this != &other)
			{
				DecrementWeakCount();
				m_ptr = other.m_ptr;
				m_controlBlock = other.m_controlBlock;
				other.m_ptr = nullptr;
				other.m_controlBlock = nullptr;
			}
			return *this;
		}

		WeakRef& operator=(const Ref<T>& ref) noexcept
		{
			WeakRef(ref).Swap(*this);
			return *this;
		}

		template<typename U> requires std::convertible_to<U*, T*>
		WeakRef& operator=(const WeakRef<U>& other) noexcept
		{
			WeakRef(other).Swap(*this);
			return *this;
		}

		template<typename U> requires std::convertible_to<U*, T*>
		WeakRef& operator=(WeakRef<U>&& other) noexcept
		{
			WeakRef(std::move(other)).Swap(*this);
			return *this;
		}

		WeakRef& operator=(std::nullptr_t) noexcept
		{
			Reset();
			return *this;
		}

		bool operator==(const WeakRef& other) const noexcept
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const WeakRef& other) const noexcept
		{
			return m_ptr != other.m_ptr;
		}

	private:
		T* m_ptr = nullptr;
		Internal::RefControlBlock* m_controlBlock = nullptr;
	};

	template<typename T> WeakRef<T> MakeWeak(const Ref<T>& ref) { return WeakRef<T>(ref); }
	template<typename To, typename From> WeakRef<To> WeakCast(const WeakRef<From>& ref)
	{
		if (auto strong = ref.Lock())
		{
			return WeakRef<To>(RefCast<To>(strong));
		}
		return WeakRef<To>();
	}

	template<typename T, typename U> bool operator==(const WeakRef<T>& lhs, const WeakRef<U>& rhs) noexcept
	{
		auto lock1 = lhs.Lock();
		auto lock2 = rhs.Lock();
		return lock1 == lock2;
	}

	template<typename T, typename U> bool operator!=(const WeakRef<T>& lhs, const WeakRef<U>& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}

namespace std
{
	template<typename T>
	struct hash<Ryu::Memory::Ref<T>>
	{
		size_t operator()(const Ryu::Memory::Ref<T>& ref) const noexcept
		{
			return std::hash<T*>()(ref.Get());
		}
	};

	template<typename T>
	struct hash<Ryu::Memory::WeakRef<T>>
	{
		size_t operator()(const Ryu::Memory::WeakRef<T>& ref) const noexcept
		{
			if (auto locked = ref.Lock())
			{
				return std::hash<T*>()(locked.Get());
			}
			return 0;
		}
	};
}
