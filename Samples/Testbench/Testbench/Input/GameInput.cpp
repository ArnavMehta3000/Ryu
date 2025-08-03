#include "Testbench/Input/GameInput.h"
#include "Profiling/Profiling.h"
#include "Logger/Logger.h"

RYU_LOG_DECLARE_CATEGORY(GameInput);
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

	RYU_PROFILE_SCOPE();

	if (GInput* input = static_cast<GInput*>(context))
	{
		if (device)
		{
			const GameInputDeviceInfo* info = nullptr;
			if (SUCCEEDED((device->GetDeviceInfo(&info))))
			{

				if (info->supportedInput & GameInputKindGamepad)
				{
					RYU_LOG_INFO(LogGameInput, "Gamepad connected");
				}

				if (info->supportedInput == GameInputKindKeyboard)
				{
					RYU_LOG_INFO(LogGameInput, "Keyboard connected");
				}

				if (info->supportedInput == GameInputKindMouse)
				{
					RYU_LOG_INFO(LogGameInput, "Mouse connected");
				}
			}
		}
	}
}

bool GInput::Init()
{
	RYU_PROFILE_SCOPE();

	if (FAILED(::GameInputCreate(&m_gameInput)))
	{
		return false;
	}

	if (FAILED(m_gameInput->RegisterDeviceCallback(
		nullptr,
		GameInputKindGamepad | GameInputKindKeyboard | GameInputKindMouse,
		GameInputDeviceAnyStatus,
		GameInputBlockingEnumeration,
		this,
		OnDeviceEnumerated,
		&m_deviceEnumToken)))
	{
		return false;
	}

	return true;
}

void GInput::Shutdown()
{
	RYU_PROFILE_SCOPE();

	if (m_gameInput)
	{
		m_gameInput.Reset();
	}
}

void GInput::PollKeyboard(bool enableLog)
{
	if (!m_gameInput)
	{
		return;
	}

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

				if (states.size() > 0 && enableLog)
				{
					RYU_LOG_INFO(LogGameInput, "Buffer entries ({}) - {}", validBufferEntries, keys);
				}
			}
		}
	}
}

void GInput::PollMouse(bool enableLog)
{
	if (!m_gameInput)
	{
		return;
	}

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
		if (reading->GetMouseState(&state) && enableLog)
		{
			RYU_LOG_INFO(LogGameInput,
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

