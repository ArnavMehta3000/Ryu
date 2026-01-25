#pragma once
#include "Editor/Panels/IEditorPanel.h"

namespace Ryu::Engine { class GameModuleHost; }

namespace Ryu::Editor
{
	class HotReloadPanel : public IEditorPanel
	{
	public:
		static constexpr std::string_view Name{ "Hot Reload" };

	public:
		HotReloadPanel(EditorApp* app, Engine::GameModuleHost* gameModuleHost);

		void OnImGuiRender() override;

	private:
		Engine::GameModuleHost* m_moduleHost;
	};
}
