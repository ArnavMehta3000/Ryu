#include "Graphics/Common.h"

namespace Ryu::App { class WindowBase; }
namespace Ryu::Graphics { class ISurface; }

namespace Ryu::Graphics::DX11::Core
{
	bool Init();
	void Shutdown();
	ISurface* CreateSurface(App::WindowBase* window);
	void OnResizeSurface(u32 width, u32 height);
	void RenderSurface();

	ID3D11Device5* const GetDevice();
	ID3D11DeviceContext4* const GetImContext();
}
