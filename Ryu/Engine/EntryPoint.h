#pragma once
#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

extern "C"
{
	bool CreateGame();
}

int WINAPI wWinMain(
	[[maybe_unused]] HINSTANCE hInstance,
	[[maybe_unused]] HINSTANCE hPrevInstance, 
	[[maybe_unused]] LPWSTR pCmdLine, 
	[[maybe_unused]] int nCmdShow)
{
	Ryu::Logger::Initialize();

	Ryu::Logger::Shutdown();

	return EXIT_SUCCESS;
}