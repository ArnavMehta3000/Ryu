#include "Game/IGameModule.h"
#include "Testbench/TestbenchApp.h"

class TestModule final : public Ryu::Game::IGameModule
{
public:
	std::shared_ptr<Ryu::App::App> CreateApplication(std::shared_ptr<Ryu::Window::Window> window) override
	{
		return std::make_shared<TestbenchApp>(window);
	}

	constexpr const char* GetName() const override
	{
		return "Testbench App";
	}
};

RYU_IMPLEMENT_GAME_MODULE(TestModule)
