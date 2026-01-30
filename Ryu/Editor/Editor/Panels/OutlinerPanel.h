#pragma once
#include "Editor/Panels/IEditorPanel.h"
#include "Game/World/World.h"

namespace Ryu::Game { struct EntityMetadata; }

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

		inline void SetWorld(Game::World* world) { m_world = world; }
		inline void SetSelectedEntity(Game::EntityHandle entity) { m_selectedEntity = entity; }
		Game::Entity GetSelectedEntity() const;

	private:
		void DrawOutliner(Game::Registry& registry);
		void DrawEntityNode(Game::Registry& registry, Game::EntityHandle entity, const Game::EntityMetadata& metadata);

	private:
		Game::World* m_world;
		Game::EntityHandle m_selectedEntity;
	};
}
