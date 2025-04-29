#include "Ref.h"

namespace Ryu::Memory
{
	namespace Internal
	{
		RefControlBlock::RefControlBlock()
			: m_strongCount(0)
			, m_weakCount(0)
			, m_expired(false)
		{}

		void RefControlBlock::AddStrongRef()
		{
			++m_strongCount;
		}

		bool RefControlBlock::ReleaseStrongRef()
		{
			return --m_strongCount == 0;
		}

		u32 RefControlBlock::GetStrongCount() const
		{
			return m_strongCount;
		}

		void RefControlBlock::AddWeakRef()
		{
			++m_weakCount;
		}

		bool RefControlBlock::ReleaseWeakRef()
		{
			return --m_weakCount == 0;
		}

		u32 RefControlBlock::GetWeakCount() const
		{
			return m_weakCount;
		}

		bool RefControlBlock::IsAlive() const
		{
			return m_strongCount > 0 && !m_expired;
		}

		void RefControlBlock::MarkExpired()
		{
			m_expired = true; m_strongCount = 0;
		}

		bool RefControlBlock::IsExpired() const
		{
			return m_expired;
		}
	}

	RefCounted::RefCounted() 
		: m_controlBlock(new Internal::RefControlBlock())
	{}

	RefCounted::RefCounted(const RefCounted&)
		: m_controlBlock(new Internal::RefControlBlock()) 
	{}

	u32 RefCounted::AddRef() const
	{
		m_controlBlock->AddStrongRef();
		return m_controlBlock->GetStrongCount();
	}

	u32 RefCounted::Release() const
	{
		if (m_controlBlock->ReleaseStrongRef())
		{
			m_controlBlock->MarkExpired();
			delete this;
			return 0;
		}
		return m_controlBlock->GetStrongCount();
	}

	u32 RefCounted::GetRefCount() const
	{
		return m_controlBlock->GetStrongCount();
	}

	void RefCounted::AddWeakRef() const
	{
		m_controlBlock->AddWeakRef();
	}

	void RefCounted::ReleaseWeakRef() const
	{
		if (m_controlBlock->ReleaseWeakRef() && m_controlBlock->GetStrongCount() == 0)
		{
			delete m_controlBlock;
		}
	}

	u32 RefCounted::GetWeakCount() const
	{
		return m_controlBlock->GetWeakCount();
	}
	
	Internal::RefControlBlock * RefCounted::GetControlBlock() const
	{
		return m_controlBlock;
	}

	RefCounted::~RefCounted()
	{
		if (m_controlBlock->GetWeakCount() == 0)
		{
			delete m_controlBlock;
		}
	}
}
