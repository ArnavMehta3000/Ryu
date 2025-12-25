#pragma once
#include "Application/Window/WindowEvents.h"
#include "Application/Window/Input/InputSystem.h"
#include "Application/Event/EventEmitter.h"


namespace Ryu::Window
{
	namespace Internal
	{
		enum class Style : DWORD
		{
			Windowed        = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
			AeroBorderless  = WS_POPUP            | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
			BasicBorderless = WS_POPUP            | WS_THICKFRAME              | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
		};
	}

	enum class WindowType
	{
		Windowed,  // Create a window with a titlebar
		Borderless // Create a window without a titlebar
	};

	// Stores either size OR dimensions of a window
	struct WindowSizePos
	{
		union
		{
			i32 Width;
			i32 X;
		};

		union
		{
			i32 Height;
			i32 Y;
		};

		auto operator<=>(const WindowSizePos& other) const
		{
			return std::tie(Width, Height) <=> std::tie(other.Width, other.Height);
		}

		bool operator==(const WindowSizePos& other) const
		{
			return std::tie(Width, Height) == std::tie(other.Width, other.Height);
		}
	};

	class Window
		: public std::enable_shared_from_this<Ryu::Window::Window>
		, public Event::EventEmitter
	{
		RYU_DISABLE_COPY_AND_MOVE(Window)
	public:
		struct Config  // Internal config representation
		{
			WindowSizePos WindowSize         = { 1280, 720 };
			WindowSizePos WindowMinSize      = { 800, 600 };
			WindowSizePos WindowPos          = { CW_USEDEFAULT, CW_USEDEFAULT };
			std::string Title                = "Ryu Window";
			bool IsVisible                   = true;
			bool CanBorderlessResize         = true;  // Should the window allow resizing by dragging the borders while borderless
			bool CanBorderlessDrag           = false;  // Should the window allow moving my dragging the client area
			WindowType Type                  = WindowType::Windowed;
		};

	public:
		explicit Window(const Window::Config& config);
		~Window();

		bool Create();
		void Destroy();
		void Show() const;
		void Hide() const;
		void Update();

		[[nodiscard]] inline HWND GetHandle() const noexcept { return m_hwnd; }
		[[nodiscard]] inline const std::string& GetTitle() const noexcept { return m_config.Title; }
		[[nodiscard]] inline const WindowSizePos GetSize() const noexcept { return m_config.WindowSize; }
		[[nodiscard]] inline const WindowSizePos GetPosition() const noexcept { return m_config.WindowPos; }
		[[nodiscard]] inline bool IsOpen() const noexcept { return !m_shouldClose; }
		[[nodiscard]] inline WindowType GetType() const noexcept { return m_config.Type; }
		[[nodiscard]] const InputSystem& GetInput() const noexcept { return m_input; }

		void SetWindowType(WindowType newType);
		void SetTitle(const std::string& newTitle);
		void SetSize(i32 width, i32 height);
		void SetPosition(i32 x, i32 y);

		bool IsVisible() const noexcept;
		bool IsMaximized() const noexcept;
		bool IsMinimized() const noexcept;
		bool GetIsResizable() const noexcept;

		operator HWND() const { return m_hwnd; }
		operator bool() const { return m_hwnd != nullptr; }

	private:
		LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HitTest(POINT cursor) const;
		static LRESULT StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void RegisterWindowClass();
		void HandleResizeTracking();
		void EndResizeTracking();

		bool IsCompositionEnabled() const;
		void ToggleBorderless();

		Internal::Style GetStyleInternal() const;

		static inline bool                              s_isWindowClassRegistered = false;
		static inline const wchar_t*                    s_className = L"RyuWindow";
		static inline std::unordered_map<HWND, Window*> s_windowMap;

		HWND           m_hwnd = nullptr;
		Window::Config m_config;
		InputSystem    m_input;
		WindowSizePos  m_prevSize;
		WindowSizePos  m_currentSize;
		bool           m_shouldClose = true;
		bool           m_isResizing  = false;
	};
}
