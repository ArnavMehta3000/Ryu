#pragma once
#include "App/Application.h"
#include <GameInput.h>

class TestbenchApp : public Ryu::App::App
{
	friend void CALLBACK OnDeviceEnumerated(
		_In_ GameInputCallbackToken callbackToken,
		_In_ void* context,
		_In_ IGameInputDevice* device,
		_In_ uint64_t timestamp,
		_In_ GameInputDeviceStatus currentStatus,
		_In_ GameInputDeviceStatus previousStatus);

	friend void CALLBACK OnReading(
		_In_ GameInputCallbackToken callbackToken,
		_In_ void* context,
		_In_ IGameInputReading* reading,
		_In_ bool hasOverrunOccurred);

public:
	explicit TestbenchApp(std::shared_ptr<Ryu::Window::Window> window);

	bool OnInit() override;
	void OnShutdown() override;
	void OnTick(const Ryu::Utils::TimeInfo& timeInfo) override;

private:
	IGameInput* m_gameInput{ nullptr };
	IGameInputDevice* m_gameInputDevice{ nullptr };
	IGameInputReading* m_currentReading{ nullptr };
	GameInputCallbackToken m_deviceEnumToken{ 0 };
	GameInputCallbackToken m_readingToken{ 0 };
};
