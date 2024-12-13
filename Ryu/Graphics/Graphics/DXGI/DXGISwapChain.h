#include "Graphics/Shared/Interface/ISwapChain.h"

struct IDXGISwapChain4;

namespace Ryu::Graphics
{
	class DXGISwapChain : public ISwapChain
	{
	public:
		using NativeType = IDXGISwapChain4;

		explicit DXGISwapChain(NativeType* swapChain);

	private:
		void Present() override;
		void Resize(u32 width, u32 height) override;
		u32 GetCurrentBackBufferIndex() const override;
		NativeObjectType GetNativeObject() const override;

	private:
		ComPtr<IDXGISwapChain4> m_swapChain;
	};
}
