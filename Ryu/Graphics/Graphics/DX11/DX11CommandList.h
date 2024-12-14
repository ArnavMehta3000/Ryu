#pragma once
#include "Graphics/DX11/DX11Types.h"
#include "Graphics/Shared/Interface/ICommandList.h"

namespace Ryu::Graphics
{
	class DX11Device;

	class DX11CommandList : public ICommandList
	{
	public:
		RYU_DECLARE_GFX_NATIVE_TYPE(DX11::ID3D11CommandList);
		RYU_DECLARE_GFX_NATIVE_TYPE_OP(m_cmdList.Get())

		DX11CommandList(const DX11Device& device, const CommandListDesc& desc);
		~DX11CommandList();

		inline DX11::IDX11DeviceContext* GetDeferredContext() const { return m_deferredContext.Get(); }
		
	private:
		NativeObjectType GetNativeObject() const override;
		void Begin() override;
		void End() override;
		void Reset() override;

	private:
		const DX11Device& m_device;
		ComPtr<NativeType> m_cmdList;
		ComPtr<DX11::IDX11DeviceContext> m_deferredContext;
	};
}
