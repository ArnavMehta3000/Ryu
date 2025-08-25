#pragma once
#include "Graphics/Core/DX12.h"
#include "Graphics/Core/DescriptorHandle.h"
#include <bitset>

namespace Ryu::Gfx
{
	class SwapChain
	{
		RYU_DISABLE_COPY_AND_MOVE(SwapChain)
	public:
		enum class Flag
		{
			AllowTearing,
			EnableVSync,
			_COUNT
		};

		struct BackBuffer
		{
			ComPtr<DX12::Resource> Resource;
			DescriptorHandle       RTVHandle;
		};

	public:
		SwapChain() = default;
		~SwapChain();

		void Create(HWND window, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		void Destroy();

		void Present();
		void Resize();

		inline bool CheckFlag(Flag flag) { return m_flags.test((u64)flag); }
		inline NODISCARD const auto& Get() { return m_swapChain; }

	private:
		void CreateSwapChain();

	private:
		static constexpr u64 FlagCount = static_cast<u64>(Flag::_COUNT);

		std::bitset<FlagCount>  m_flags;
		HWND                    m_window = nullptr;
		DXGI_FORMAT             m_format;
		ComPtr<DXGI::SwapChain> m_swapChain;
		ComPtr<DX12::Resource>  m_depthStencil;
		FrameArray<BackBuffer>  m_renderTargets;
		u32                     m_currentFrameIndex = 0;
		CD3DX12_VIEWPORT        m_viewport;
		CD3DX12_RECT            m_scissorRect;
	};
}
