#pragma once

namespace Ryu::Utils { class FrameTimer; }

namespace Ryu::Editor
{
	class EditorApp;

	class IEditorPanel
	{
	public:
		IEditorPanel(EditorApp* app) : m_editorApp(app) {}
		virtual ~IEditorPanel() {}

		virtual void OnImGuiRender() = 0;

	protected:
		[[nodiscard]] inline EditorApp* GetEditorApp() const { return m_editorApp; }

	public:
		bool IsOpen = true;

	private:
		EditorApp* m_editorApp = nullptr;
	};
}
