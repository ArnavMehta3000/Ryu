#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

int main()
{
	Ryu::Logger::Initialize();

	Ryu::Logger::Shutdown();

	return 0;
}