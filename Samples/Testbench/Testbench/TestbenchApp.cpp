#include "Testbench/TestbenchApp.h"
#include "Profiling/Profiling.h"
#include "App/AppConfig.h"
#
#pragma comment(lib, "GameInput.lib")

// Currently testing gameinput

RYU_LOG_DECLARE_CATEGORY(TestbenchApp);

static void CALLBACK OnDeviceEnumerated(
	_In_ GameInputCallbackToken callbackToken,
	_In_ void* context,
	_In_ IGameInputDevice* device,
	_In_ uint64_t timestamp,
	_In_ GameInputDeviceStatus currentStatus,
	_In_ GameInputDeviceStatus previousStatus)
{
	if (TestbenchApp* app = static_cast<TestbenchApp*>(context))
	{
		if (device)
		{
			if (const GameInputDeviceInfo* info = device->GetDeviceInfo())
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

				// Save gamepad
				if (info->supportedRumbleMotors)
				{
					app->m_gameInputDevice = device;
				}
			}
		}
	}
}

static void CALLBACK OnReading(
	_In_ GameInputCallbackToken callbackToken,
	_In_ void* context,
	_In_ IGameInputReading* reading,
	_In_ bool hasOverrunOccurred)
{
	if (TestbenchApp* app = static_cast<TestbenchApp*>(context))
	{

		RYU_LOG_INFO(LogTestbenchApp, "Reading");
	}
}


using namespace Ryu;

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

	hr = m_gameInput->RegisterReadingCallback(
		nullptr,
		GameInputKindGamepad | GameInputKindKeyboard | GameInputKindMouse,
		0.0f,
		this,
		OnReading,
		&m_readingToken);
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

	if (m_gameInputDevice)
	{
		m_gameInputDevice->Release();
		m_gameInputDevice = nullptr;
	}

	if (m_currentReading)
	{
		m_currentReading->Release();
		m_currentReading = nullptr;
	}

	if (m_gameInput)
	{
		m_gameInput->Release();
		m_gameInput = nullptr;
	}
}

void TestbenchApp::OnTick(const Ryu::Utils::TimeInfo&)
{
	if (m_currentReading)
	{
		m_currentReading->Release();
		m_currentReading = nullptr;
	}

	if (m_gameInputDevice)
	{
		if (auto info = m_gameInputDevice->GetDeviceInfo())
		{
			GameInputRumbleParams params{ 0, 0, 0, 0 };
			m_gameInputDevice->SetRumbleState(&params);
		}
	}


	HRESULT hr = m_gameInput->GetCurrentReading(
		GameInputKindGamepad | GameInputKindKeyboard | GameInputKindMouse,
		nullptr,
		&m_currentReading
	);
}
