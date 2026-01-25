#include "Editor/Panels/HotReloadPanel.h"

#include "Core/Profiling/Profiling.h"
#include "Core/Logging/Logger.h"
#include "Engine/HotReload/GameModuleHost.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	HotReloadPanel::HotReloadPanel(EditorApp* app, Engine::GameModuleHost* gameModuleHost)
		: IEditorPanel(app)
		, m_moduleHost(gameModuleHost)
	{
	}

	void HotReloadPanel::OnImGuiRender()
	{
		RYU_PROFILE_SCOPE();

		if (!m_moduleHost)
		{
			ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No module loaded"); 
			return;
		}

		if (ImGui::Begin("Hot Reload##HotReloadPanel"))
		{
			const auto& info = m_moduleHost->GetModuleInfo();

			ImGui::Text("Module: %s", info.Name);
			ImGui::Text("Version: %u.%u.%u",
				info.Version.Major, info.Version.Minor, info.Version.Patch);

			ImGui::Separator();

			bool autoReload = m_moduleHost->IsAutoReloadEnabled();
			if (ImGui::Checkbox("Auto Reload##HotReloadPanel", &autoReload))
			{
				m_moduleHost->EnableAutoReload(autoReload);
			}

			if (ImGui::Button("Force Reload##HotReloadPanel"))
			{
				auto result = m_moduleHost->Reload();
				if (!result)
				{
					RYU_LOG_ERROR("Hot-reload failed: {}", Engine::ToString(result.error()));
				}
			}
		}
		ImGui::End();
	}
}
