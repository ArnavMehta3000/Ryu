#include "Testbench/TestbenchGame.h"
#include "Testbench/TestbenchWorld.h"
#include "Game/Core/GameServices.h"

bool TestbenchGame::Initialize()
{
    RYU_GAME_LOG_INFO("TestbenchGame initializing");

    m_worldManager.CreateWorld<TestbenchWorld>();

    return true;
}

void TestbenchGame::Tick(float deltaTime)
{
    m_totalTime += deltaTime;
    //m_worldManager.Tick(deltaTime);
}

void TestbenchGame::Shutdown()
{
    RYU_GAME_LOG_INFO("TestbenchGame shutting down");
}

//void TestbenchGame::Serialize(Ryu::Game::SerializationBuffer& buffer) const
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
//bool TestbenchGame::Deserialize(Ryu::Game::SerializationBuffer& buffer)
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
