#pragma once

#if defined(RYU_RHI_D3D11)
#define RYU_RHI_HAS_D3D11 1
#else
#define RYU_RHI_HAS_D3D11 0
#endif

#if defined(RYU_RHI_D3D12)
#define RYU_RHI_HAS_D3D12 1
#else
#define RYU_RHI_HAS_D3D12 0
#endif

// Check if we have at least one API
static_assert(RYU_RHI_HAS_D3D11 || RYU_RHI_HAS_D3D12,
	"Ryu needs at least one RHI to be enabled in xmake config to be built!");
