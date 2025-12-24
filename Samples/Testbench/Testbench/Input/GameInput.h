#pragma once
#include "Core/Common/Common.h"
#include <GameInput/GameInput.h>
#include <wrl/client.h>

#ifndef GAMEINPUT_API_VERSION
#define GAMEINPUT_API_VERSION 0
#endif

#if GAMEINPUT_API_VERSION == 1
using namespace GameInput::v1;
#elif GAMEINPUT_API_VERSION == 2
using namespace GameInput::v2;
#endif

using Microsoft::WRL::ComPtr;

class GInput
{
	friend void CALLBACK OnDeviceEnumerated(
		_In_ GameInputCallbackToken callbackToken,
		_In_ void* context,
		_In_ IGameInputDevice* device,
		_In_ uint64_t timestamp,
		_In_ GameInputDeviceStatus currentStatus,
		_In_ GameInputDeviceStatus previousStatus);
public:
	bool Init();
	void Shutdown();

	bool IsInitialized() const { return m_gameInput != nullptr; }

	void PollKeyboard(bool enableLog = false);
	void PollMouse(bool enableLog = false);

private:
	ComPtr<IGameInput> m_gameInput;
	GameInputCallbackToken m_deviceEnumToken{ 0 };
};
