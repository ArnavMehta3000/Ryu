#pragma once
#include "Graphics/Core/GfxDescriptorAllocatorBase.h"
#include <list>

namespace Ryu::Gfx
{
	class GfxDescriptorAllocator : public GfxDescriptorAllocatorBase
	{
	public:
		struct Desc
		{
			DescriptorHeapType Type;
			u32 DescriptorCount  = 0;
			bool IsShaderVisible = false;
		};
	
	public:
		GfxDescriptorAllocator(GfxDevice* device, const GfxDescriptorAllocator::Desc& desc);
		~GfxDescriptorAllocator() = default;

		[[nodiscard]] GfxDescriptor AllocateDescriptor();
		void FreeDescriptor(GfxDescriptor descriptor);

	private:
		struct Range
		{
			GfxDescriptor Begin;
			GfxDescriptor End;
		};

		GfxDescriptor m_tailDescriptor;
		std::list<Range> m_freeRanges;
	};
}
