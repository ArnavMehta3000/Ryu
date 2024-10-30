#pragma once
#include <Plugin/PluginInterface.h>
#include <memory>

namespace Sample
{
	class SamplePlugin : public Ryu::Plugin::PluginInterface
	{
		RYU_DISABLE_COPY_AND_MOVE(SamplePlugin);

	public:
		static constexpr std::string_view GetFilename() { return "SamplePlugin"; }

		std::string_view GetDescription() const noexcept override;
		std::string_view GetName() const noexcept override;
		u32 GetVersion() const noexcept override;

		SamplePlugin() {};
		~SamplePlugin() {};

	private:
		bool OnActivate() override;
		void OnDeactivate() override;

	public:
		u32 SomeData{ 10 };
	};
}

// Create static plugin interface
//namespace Ryu::Plugin
//{
//	template<>
//	struct StaticPluginProvider<Sample::SamplePlugin>
//	{
//		static std::unique_ptr<Sample::SamplePlugin> Instantiate()
//		{
//			return std::make_unique<Sample::SamplePlugin>();
//		}
//	};
//}

