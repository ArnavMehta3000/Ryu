#include "Editor/Panels/OutlinerPanel.h"

namespace Ryu::Editor
{
	OutlinerPanel::OutlinerPanel(EditorApp* app, Game::World* world)
		: IEditorPanel(app)
		, m_world(world)
	{
	}
	
	void OutlinerPanel::OnImGuiRender()
	{
	}
}
