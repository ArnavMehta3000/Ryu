#pragma once
#include "Game/World/WorldManager.h"

class TestbenchGame
{
public:
    bool Initialize();
    void Tick(float deltaTime);
    void Shutdown();

    void* GetActiveWorld() { return m_worldManager.GetActiveWorld(); }

    //void Serialize(Ryu::Game::SerializationBuffer& buffer) const;
    //bool Deserialize(Ryu::Game::SerializationBuffer& buffer);

private:
    Ryu::Game::WorldManager m_worldManager;
    float m_totalTime = 0.0f;
};
