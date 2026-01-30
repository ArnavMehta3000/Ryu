#pragma once
#include "Editor/Panels/IEditorPanel.h"
#include "Game/World/World.h"

namespace Ryu::Editor
{
	class OutlinerPanel : public IEditorPanel
	{
	public:
		static constexpr auto Name = "Outliner";

	public:
		OutlinerPanel(EditorApp* app, Game::World* world);
		~OutlinerPanel() override = default;

		void OnImGuiRender() override;

	private:
		Game::World* m_world = nullptr;
	};
}
