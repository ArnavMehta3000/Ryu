#include "Testbench/Game/TestbenchGameState.h"

#include "Game/Core/GameServices.h"
#include "Testbench/Game/TestbenchWorld.h"

bool TestbenchGameState::Initialize()
{
    RYU_GAME_LOG_INFO("TestbenchGameState initializing");

    m_worldManager.CreateWorld<TestbenchWorld>();

    return true;
}

void TestbenchGameState::Tick(float deltaTime)
{
    m_totalTime += deltaTime;
    //m_worldManager.Tick(deltaTime);
}

void TestbenchGameState::Shutdown()
{
    RYU_GAME_LOG_INFO("TestbenchGameState shutting down");
}

void* TestbenchGameState::GetWorldManager()
{
    return &m_worldManager;
}

//void TestbenchGameState::Serialize(Ryu::Game::SerializationBuffer& buffer) const
//{
//    buffer.Write(m_totalTime);
//
//    if (auto* world = m_worldManager.GetActiveWorld())
//    {
//        buffer.Write(true);
//        world->Serialize(buffer);
//    }
//    else
//    {
//        buffer.Write(false);
//    }
//}
//
//bool TestbenchGameState::Deserialize(Ryu::Game::SerializationBuffer& buffer)
//{
//    if (!buffer.Read(m_totalTime))
//        return false;
//
//    bool hasWorld;
//    if (!buffer.Read(hasWorld))
//        return false;
//
//    if (hasWorld)
//    {
//        auto* world = m_worldManager.CreateWorld<TestbenchWorld>();
//        if (!world->Deserialize(buffer))
//            return false;
//    }
//
//    RYU_GAME_LOG_INFO("State restored: totalTime={}", m_totalTime);
//    return true;
//}
