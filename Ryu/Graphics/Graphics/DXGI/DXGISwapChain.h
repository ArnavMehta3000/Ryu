#include "Graphics/Shared/Interface/ISwapChain.h"

struct IDXGISwapChain4;

namespace Ryu::Graphics
{
	class DXGISwapChain : ISwapChain
	{
	public:
		using NativeType = IDXGISwapChain4;

	private:
		void Present() override;
		void Resize(u32 width, u32 height) override;
		u32 GetCurrentBackBufferIndex() const override;
		NativeObjectType GetNativeObject() const override;

	private:
		ComPtr<IDXGISwapChain4> m_swapChain;
	};
}
