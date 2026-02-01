#pragma once
#include "Editor/Panels/ComponentPanel.h"
#include "Editor/Panels/OutlinerPanel.h"

namespace Ryu::Editor
{
	class EntityDetailsPanel : public IEditorPanel
	{
	public:
		static constexpr auto Name = "Entity Details";
		using ComponentFactory = std::function<void(Game::Entity&)>;

	public:
		EntityDetailsPanel(EditorApp* app, OutlinerPanel* outlinerPanel);
		virtual ~EntityDetailsPanel() override;

		void OnImGuiRender() override;

		void RegisterComponentType(const std::string& componentName, ComponentFactory factory);

		template <typename ComponentType>
		void RegisterComponentPanel(std::unique_ptr<IComponentPanel> panel)
		{
			m_componentPanels[typeid(ComponentType).hash_code()] = std::move(panel);
		}

		inline Game::Entity GetSelectedEntity() const { return m_outlinerPanel->GetSelectedEntity(); }

	private:
		std::vector<IComponentPanel*> GetComponentPanelsForSelectedEntity();
		void DrawComponents();
		void DrawEntityMetadata();
		void DrawComponentPanel(IComponentPanel* panel);
		void DrawAddComponentButton();
		
	private:
		using ComponentPanelMap = std::unordered_map<u64, std::unique_ptr<IComponentPanel>>;
		using ComponentFactoryMap = std::unordered_map<std::string, ComponentFactory>;

		Game::Entity        m_selectedEntity;
		OutlinerPanel*      m_outlinerPanel;
		ComponentPanelMap   m_componentPanels;
		ComponentFactoryMap m_availableComponents;
	};
}
