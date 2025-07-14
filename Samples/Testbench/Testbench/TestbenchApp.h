#pragma once
#include "App/Application.h"
#include "External/GameInput/GameInput.h"
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

class TestbenchApp : public Ryu::App::App
{
	friend void CALLBACK OnDeviceEnumerated(
		_In_ GameInputCallbackToken callbackToken,
		_In_ void* context,
		_In_ IGameInputDevice* device,
		_In_ uint64_t timestamp,
		_In_ GameInputDeviceStatus currentStatus,
		_In_ GameInputDeviceStatus previousStatus);

public:
	explicit TestbenchApp(std::shared_ptr<Ryu::Window::Window> window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;

private:
	void PollKeyboard();
	void PollMouse();

private:
	ComPtr<IGameInput> m_gameInput;
	GameInputCallbackToken m_deviceEnumToken{ 0 };
	GameInputCallbackToken m_readingToken{ 0 };
};
