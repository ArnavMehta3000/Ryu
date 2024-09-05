#include "TestApplication.h"

Ryu::Window::Config cfg
{
	.Title = L"Test Application",
	.Type = Ryu::Window::Type::Resizeable,
};

TestApplication::TestApplication() 
	: Ryu::Application(cfg)
{
}

TestApplication::~TestApplication()
{
}
