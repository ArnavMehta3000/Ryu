#pragma once
#include "Common/ObjectMacros.h"
#include "Window/Events.h"
#include "Window/WindowConfig.h"
#include <unordered_map>
#include <vector>
#include <functional>

namespace Ryu::Window
{
	class Window
	{
		RYU_DISABLE_COPY_AND_MOVE(Window);
	public:
		explicit Window(const WindowConfig& config = {});
		~Window();

		bool Create();
		void Destroy();
		void Show();
		void Hide();
		void Update();

		RYU_PROPERTY(Title, std::string);
		inline NODISCARD const std::string& GetTitle() const noexcept { return m_config.Title; }
		void SetTitle(const std::string& title);
		
		RYU_PROPERTY(Size, std::pair<i32, i32>);
		inline NODISCARD const std::pair<i32, i32> GetSize() const noexcept { return { m_config.Width, m_config.Height }; }
		void SetSize(i32 width, i32 height);
		
		RYU_PROPERTY(Position, std::pair<i32, i32>);
		inline NODISCARD const std::pair<i32, i32> GetPosition() const noexcept { return { m_config.X, m_config.Y }; }
		void SetPosition(i32 x, i32 y);
		
		RYU_PROPERTY(CanResize, bool);
		NODISCARD bool GetCanResize() const noexcept;
		void SetCanResize(bool canResize);
		
		RYU_SET_ONLY_PROPERTY(HasMaximizeButton, bool);
		void SetHasMaximizeButton(bool hasMaximizeButton);
		
		RYU_SET_ONLY_PROPERTY(HasMinimizeButton, bool);
		void SetHasMinimizeButton(bool hasMinimizeButton);
		
		RYU_SET_ONLY_PROPERTY(HasCloseButton, bool);
		void SetHasCloseButton(bool hasCloseButton);

		RYU_GET_ONLY_PROPERTY(Handle, HWND);
		NODISCARD HWND GetHandle() const noexcept { return m_hwnd; }
		
		RYU_GET_ONLY_PROPERTY(IsVisible, bool);
		NODISCARD bool GetIsVisible() const noexcept;
		
		RYU_GET_ONLY_PROPERTY(IsMaximized, bool);
		NODISCARD bool GetIsMaximized() const noexcept;
		
		RYU_GET_ONLY_PROPERTY(IsMinimized, bool);
		NODISCARD bool GetIsMinimized() const noexcept;
		
		RYU_GET_ONLY_PROPERTY(IsResizable, bool);
		NODISCARD bool GetIsResizable() const noexcept;

		template <EventListener T>
		void AddEventListener(T&& listener)
		{
			m_eventListeners.emplace_back(std::forward<T>(listener));
		}

		void RemoveAllEventListeners();

		static inline const std::vector<WindowEvent>& GetPendingEvents() { return s_pendingEvents; }
		static void ClearPendingEvents();

		operator HWND() const { return m_hwnd; }
		operator bool() const { return m_hwnd != nullptr; }

	private:
		LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void RegisterWindowClass();
		void UpdateWindowStyle();
		DWORD GetWindowStyle() const;
		DWORD GetExtendedWindowStyle() const;
		void DispatchEvent(WindowEvent event);
		void HandleResizeTracking();
		void EndResizeTracking();

		static inline bool s_isWindowClassRegistered = false;
		static inline const wchar_t* s_className = L"RyuWindow";
		static inline std::unordered_map<HWND, Window*> s_windowMap;
		static inline std::vector<WindowEvent> s_pendingEvents;

		HWND m_hwnd = nullptr;
		WindowConfig m_config;
		std::vector<std::function<void(const WindowEvent&)>> m_eventListeners;
		bool m_shouldClose= false;

		// Resize tracking
		bool m_isResizing = false;
		std::pair<i32, i32> m_prevSize;
		std::pair<i32, i32> m_currentSize;
	};
}
