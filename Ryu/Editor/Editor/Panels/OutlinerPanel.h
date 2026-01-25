#pragma once
#include "Editor/Panels/IEditorPanel.h"
#include "Game/World/World.h"

namespace Ryu::Editor
{
	class OutlinerPanel : public IEditorPanel
	{
	public:
		static constexpr std::string_view Name{ "Outliner" };

	public:
		OutlinerPanel(EditorApp* app, Game::World* world);

		void OnImGuiRender();

	private:
		Game::World* m_world = nullptr;
	};
}
