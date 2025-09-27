#include "Graphics/Core/GfxDescriptorAllocator.h"

namespace Ryu::Gfx
{
	GfxDescriptorAllocator::GfxDescriptorAllocator(GfxDevice* device, const GfxDescriptorAllocator::Desc& desc)
		: GfxDescriptorAllocatorBase(device, desc.Type, desc.DescriptorCount, desc.IsShaderVisible)
		, m_tailDescriptor(m_headDescriptor)
	{
		m_tailDescriptor.Increment(m_descriptorSize, desc.DescriptorCount - 1);
		m_freeRanges.emplace_back(m_headDescriptor, m_tailDescriptor);
	}

	GfxDescriptor GfxDescriptorAllocator::AllocateDescriptor()
	{
		Range& range = m_freeRanges.front();
		
		GfxDescriptor handle = range.Begin;
		range.Begin.Increment(m_descriptorSize);
		if (range.Begin == range.End)
		{
			m_freeRanges.pop_front();
		}

		return handle;
	}

	void GfxDescriptorAllocator::FreeDescriptor(GfxDescriptor descriptor)
	{
		GfxDescriptor incrementedDescriptor= descriptor;
		incrementedDescriptor.Increment(m_descriptorSize);

		bool found = false;
		Range range{ .Begin = descriptor, .End = incrementedDescriptor };
		
		for (auto current = std::begin(m_freeRanges);
			current != std::end(m_freeRanges) && found;
			++current)
		{
			if (current->Begin == incrementedDescriptor)
			{
				current->Begin = descriptor;
				found = true;
			}
			else if (current->End == incrementedDescriptor)
			{
				current->End.Increment(m_descriptorSize);
				found = true;
			}
			else if (current->Begin.GetIndex() > descriptor.GetIndex())
			{
				m_freeRanges.insert(current, range);
				found = true;
			}
		}

		if (!found)
		{
			m_freeRanges.push_back(range);
		}
	}
}
