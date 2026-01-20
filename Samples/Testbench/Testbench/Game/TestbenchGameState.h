#pragma once
#include "Game/World/WorldManager.h"

class TestbenchGameState
{
public:
    bool Initialize();
    void Tick(float deltaTime);
    void Shutdown();

    void* GetWorldManager();

    //void Serialize(Ryu::Game::SerializationBuffer& buffer) const;
    //bool Deserialize(Ryu::Game::SerializationBuffer& buffer);

    Ryu::Game::WorldManager m_worldManager;
private:
    float m_totalTime = 0.0f;
};
