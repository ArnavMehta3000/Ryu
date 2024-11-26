[Factory = CreateTestbenchGameInstance]
[FriendlyName = Testbench Game Instance]
class TestbenchGameInstance : IGameInstance
{
	double totalTime = 0.0;

	void OnStart()
	{
		RyuLog(LogLevel::Trace, "Starting the game instance");
	}

	void OnEnd()
	{
		RyuLog(LogLevel::Trace, "Ending the game instance");
	}

	void OnUpdate(double dt)
	{
		totalTime += dt;
		RyuLog(LogLevel::Trace, "Updating the game instance (total time): " + totalTime);
	}
}

TestbenchGameInstance @CreateTestbenchGameInstance()
{
	return @TestbenchGameInstance();
}
