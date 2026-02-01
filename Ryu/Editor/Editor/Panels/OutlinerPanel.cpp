#include "Editor/Panels/OutlinerPanel.h"
#include "Game/Components/EntityMetadata.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	OutlinerPanel::OnEntitySelectedEvent::OnEntitySelectedEvent() : m_world(nullptr), m_handle(Game::InvalidEntityHandle) { };

	OutlinerPanel::OnEntitySelectedEvent::OnEntitySelectedEvent(Game::World* world, Ryu::Game::EntityHandle handle) : m_world(world), m_handle(handle) { }
	
	Game::Entity OutlinerPanel::OnEntitySelectedEvent::GetEntity()
	{
		return m_world ? m_world->GetEntityFromHandle(m_handle) : Game::Entity();
	}


	OutlinerPanel::OutlinerPanel(EditorApp* app, Game::World* world)
		: IEditorPanel(app)
		, m_world(world)
		, m_selectedEntity(Game::InvalidEntityHandle)
	{
	}
	
	void OutlinerPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
		
		if (IsOpen)
		{
			if (ImGui::Begin("Outliner##OutlinerPanel", &IsOpen, ImGuiWindowFlags_None))
			{
				if (!m_world) [[unlikely]]
				{
					// No world loaded
					ImGui::Text("No world loaded");
				}
				else
				{
					DrawOutliner(m_world->GetRegistry());
				}
			}
			ImGui::End();
		}

		// Since this function is called once per frame, handle event queue here
		// ProcessEventQueue();  // Commented this since I don't plan to use queued events with ImGui here
	}

	Game::Entity OutlinerPanel::GetSelectedEntity() const
	{
		return m_world ? m_world->GetEntityFromHandle(m_selectedEntity) : Game::Entity();
	}

	void OutlinerPanel::DrawOutliner(Game::Registry& registry)
	{
		// Get all entities
		auto view = registry.view<Game::EntityMetadata>();		
		ImGui::BeginChild("EntityList##OutlinerPanel");
		{
			for (Game::EntityHandle entity : view)
			{
				if (registry.valid(entity))
				{
					const auto& metadata = registry.get<Game::EntityMetadata>(entity);

					DrawEntityNode(registry, entity, metadata);					
				}
			}
		}
		ImGui::EndChild();

		// Context menu for creating new entities
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
		{
			ImGui::OpenPopup("Entity Context Menu##OutlinerPanel");
		}

		if (ImGui::BeginPopup("Entity Context Menu##OutlinerPanel"))
		{
			if (ImGui::MenuItem("Create New Entity"))
			{
				m_world->CreateEntity();  // World will add the Game::EntityMetadata component
			}
			ImGui::EndPopup();
		}
	}
	
	void OutlinerPanel::DrawEntityNode([[maybe_unused]] Game::Registry& registry, Game::EntityHandle entity, const Game::EntityMetadata& metadata)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		// Highlight selected entity
		if (entity == m_selectedEntity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		const std::string UUID = metadata.GetUUIDPretty();
		const std::string nodeId = metadata.Name + "##" + UUID;

		ImGui::TreeNodeEx(nodeId.c_str(), flags);

		// Handle entity selection
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			SelectEntity(entity);
		}

		// Context menu for entity
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			SelectEntity(entity);
			ImGui::OpenPopup("Entity Context Menu##OutlinerPanel");
		}

		if (ImGui::BeginPopup("Entity Context Menu##OutlinerPanel"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				m_world->DestroyEntity(entity);
			}
			ImGui::EndPopup();
		}
	}
	
	void OutlinerPanel::SelectEntity(Game::EntityHandle entity)
	{
		if (m_selectedEntity != entity)
		{
			m_selectedEntity = entity;
		}		
	}
}
