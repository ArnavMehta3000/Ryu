class TestGameInstance : ScriptableGameInstance
{
	private float m_gameTime = 0;

	void OnInit()
	{
		DebugLog("INIT GAME INSTANCE");
	}

	void OnStart()
	{
	}

	void OnStop()
	{
	}

	void OnUpdate(double dt)
	{
		m_gameTime += dt;
		DebugLog("UPDATE GAME INSTANCE - " + m_gameTime);
	}
}
