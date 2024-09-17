#include <Core/Plugin/PluginAPI.h>

class PLUGIN_API RyuTest : public Ryu::PluginBase<RyuTest>
{
public:
    RyuTest();
    virtual ~RyuTest();
    bool Initialize(const Ryu::PluginAPI& api) override;
    void Shutdown() override;
};