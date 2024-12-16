#pragma once
#include "Common/ObjectMacros.h"
#include "GraphicsRHI/IGraphicsObject.h"

namespace Ryu::Graphics
{
	enum class CommandListType
	{
		Graphics,
		Compute,
		Copy
	};

	struct CommandListDesc
	{
		CommandListType Type;
	};

	class ICommandList : public IGraphicsObject
	{
	public:
		RYU_DISABLE_COPY(ICommandList);

		ICommandList(const CommandListDesc& desc) : m_desc(desc) {}
		virtual ~ICommandList() = default;

		inline const CommandListDesc& GetDesc() const { return m_desc; }

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Reset() = 0;

	protected:
		CommandListDesc m_desc{};
	};
}
