#include <Core/Log/Log.h>
#include <Core/Log/Logger.h>

void f()
{
	RYU_ASSERT(1 == 2, "Oopsie");
}
int main()
{
	Ryu::Logger::Initialize();

	RYU_TRACE("This is a {} {} message", "trace", 1.2);
	RYU_DEBUG("This is a {} {} message", "debug", 1.2);
	RYU_INFO("This is a {} {} message", "info", 1.2);
	RYU_WARN("This is a {} {} message", "warning", 1.2);
	RYU_ERROR("This is a {} {} message", "error", 1.2);
	RYU_FATAL("This is a {} {} message", "fatal", 1.2);

	f();

	Ryu::Logger::Shutdown();

	return 0;
}