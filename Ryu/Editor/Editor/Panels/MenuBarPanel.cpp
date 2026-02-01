#include "Editor/Panels/MenuBarPanel.h"

#include "Core/Profiling/Profiling.h"
#include "Editor/Application/EditorApp.h"
#include "Editor/Panels/EntityDetailsPanel.h"
#include "Editor/Panels/OutlinerPanel.h"
#include "Editor/Panels/AssetsPanel.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	void MenuBarPanel::OnImGuiRender()
	{
		RYU_PROFILE_SCOPE();

		CachePanels();

		if (ImGui::BeginMainMenuBar())
		{
			// Panel view control
			if (ImGui::BeginMenu("Window##MenuBarPanel"))
			{
				ImGui::MenuItem("Outliner##MenuBarPanel", nullptr, &m_outliner->IsOpen);
				ImGui::MenuItem("Entity Details##MenuBarPanel", nullptr, &m_entityDetails->IsOpen);
				ImGui::MenuItem("Assets##MenuBarPanel", nullptr, &m_assetsPanel->IsOpen);

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();
	}
	
	void MenuBarPanel::CachePanels()
	{
		RYU_PROFILE_SCOPE();

		// Only re-cache if any panel is missing
		if (m_outliner 
			&& m_entityDetails
			&& m_assetsPanel)
		{
			return;
		}

		EditorApp* app = GetEditorApp();

		if (!m_outliner)
		{
			m_outliner = app->GetEditorPanel<OutlinerPanel>();
		}

		if (!m_entityDetails)
		{
			m_entityDetails = app->GetEditorPanel<EntityDetailsPanel>();
		}

		if (!m_assetsPanel)
		{
			m_assetsPanel = app->GetEditorPanel<AssetsPanel>();
		}
	}
}
