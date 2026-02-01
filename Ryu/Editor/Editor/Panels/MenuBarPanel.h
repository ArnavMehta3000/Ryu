#pragma once
#include "Editor/Panels/IEditorPanel.h"

namespace Ryu::Editor
{
	class OutlinerPanel;
	class EntityDetailsPanel;

	class MenuBarPanel : public IEditorPanel
	{
	public:
		static constexpr auto Name = "Menu Bar";

	public:
		using IEditorPanel::IEditorPanel;

		void OnImGuiRender() override;

	private:
		void CachePanels();

	private:
		OutlinerPanel* m_outliner = nullptr;
		EntityDetailsPanel* m_entityDetails = nullptr;
	};
}
