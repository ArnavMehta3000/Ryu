#pragma once
#include "Editor/Panels/ComponentPanels/ComponentPanel.h"
#include "Game/Components/MeshRenderer.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	class MeshRendererPanel : public ComponentPanel<Game::MeshRenderer>
	{
	protected:
		void DrawComponentUI(Game::MeshRenderer& component, Game::Entity)
		{
			ImGui::Checkbox("Is Visible##MeshRendererPanel", &component.IsVisible);
			ImGui::Text("Asset ID: %llu", component.MeshHandle.id);
		}
	};
}
