#include "Testbench/TestbenchApp.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"

// Currently testing gameinput

RYU_LOG_DECLARE_CATEGORY(TestbenchApp);
using namespace Ryu;

void CALLBACK OnDeviceEnumerated(
	_In_ GameInputCallbackToken callbackToken,
	_In_ void* context,
	_In_ IGameInputDevice* device,
	_In_ uint64_t timestamp,
	_In_ GameInputDeviceStatus currentStatus,
	_In_ GameInputDeviceStatus previousStatus)
{
	UNREFERENCED_PARAMETER(timestamp);
	UNREFERENCED_PARAMETER(callbackToken);
	UNREFERENCED_PARAMETER(currentStatus);
	UNREFERENCED_PARAMETER(previousStatus);

	if (TestbenchApp* app = static_cast<TestbenchApp*>(context))
	{
		if (device)
		{
			const GameInputDeviceInfo* info = nullptr;
			if (SUCCEEDED((device->GetDeviceInfo(&info))))
			{

				if (info->supportedInput == GameInputKindGamepad)
				{
					RYU_LOG_INFO(LogTestbenchApp, "Gamepad connected");
				}

				if (info->supportedInput == GameInputKindKeyboard)
				{
					RYU_LOG_INFO(LogTestbenchApp, "Keyboard connected");
				}

				if (info->supportedInput == GameInputKindMouse)
				{
					RYU_LOG_INFO(LogTestbenchApp, "Mouse connected");
				}
			}
		}
	}
}

TestbenchApp::TestbenchApp(std::shared_ptr<Window::Window> window)
	: App::App(window)
{
}

bool TestbenchApp::OnInit()
{
	RYU_LOG_INFO(LogTestbenchApp, "Initializing Testbench App");

	HRESULT hr = ::GameInputCreate(&m_gameInput);
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_gameInput->RegisterDeviceCallback(
		nullptr,
		GameInputKindGamepad | GameInputKindKeyboard | GameInputKindMouse,
		GameInputDeviceAnyStatus,
		GameInputBlockingEnumeration,
		this,
		OnDeviceEnumerated,
		&m_deviceEnumToken);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO(LogTestbenchApp, "Shutting down Testbench App");
}

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo&)
{
	//PollKeyboard();
	//PollMouse();
}

void TestbenchApp::PollKeyboard()
{
	HRESULT hr = S_OK;
	ComPtr<IGameInputReading> reading;
	ComPtr<IGameInputDevice> device;

	hr = m_gameInput->GetCurrentReading(
		GameInputKindKeyboard,
		nullptr,
		&reading);
	if (SUCCEEDED(hr))
	{
		// Lock onto the first device we find input from
		reading->GetDevice(&device);

		const GameInputDeviceInfo* info = nullptr;
		if SUCCEEDED((device->GetDeviceInfo(&info)))
		{
			if (auto kb = info->keyboardInfo)
			{
				std::vector<GameInputKeyState> states(kb->keyCount);
				u32 validBufferEntries = reading->GetKeyState(kb->keyCount, states.data());

				//Convert  vector to string
				std::string keys = "Keys: ";
				for (auto& state : states)
				{
					char key = static_cast<char>(state.codePoint);
					keys += key;
				}

				if (states.size() > 0)
				{
					RYU_LOG_INFO(LogTestbenchApp, "Buffer entries ({}) - {}", validBufferEntries, keys);
				}
			}
		}
	}
}

void TestbenchApp::PollMouse()
{
	HRESULT hr = S_OK;
	ComPtr<IGameInputReading> reading;
	ComPtr<IGameInputDevice> device;

	hr = m_gameInput->GetCurrentReading(
		GameInputKindMouse,
		nullptr,
		&reading);
	if (SUCCEEDED(hr))
	{
		// Lock onto the first device we find input from
		reading->GetDevice(&device);

		// Probably have to use GetNextReading, values in this state are cumulative
		GameInputMouseState state{};
		if (reading->GetMouseState(&state))
		{
			RYU_LOG_INFO(LogTestbenchApp,
				"AbsPos({}, {}) | Pos({}, {}) | Wheel({}, {}) | PosEnum({}) | Buttons({})",
				state.absolutePositionX, state.absolutePositionY,
				state.positionX, state.positionY,
				state.wheelX, state.wheelY,
				static_cast<u32>(state.positions),
				static_cast<u32>(state.buttons));
		}
	}
	else
	{
		device = nullptr;
	}
}
