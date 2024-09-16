#pragma once
#include <Core/Application/IApplication.h>
#include <string>

namespace Ryu
{
	enum class PluginLoadOrder
	{
		PreInit,
		PostInit,
		Default = PostInit,
	};

	enum class PluginTickOrder
	{
		None,
		PreUpdate,
		PostUpdate,
		Default = PostUpdate
	};

	enum class PluginRenderOrder
	{
		None,
		PreRender,
		PostRender,
		Default = PostRender
	};

	struct RYU_API PluginAPI
	{
		IApplication* App{ nullptr };
	};

	struct PluginData
	{
		std::string_view Name;
		std::string_view Version;
		PluginLoadOrder LoadOrder;
		PluginTickOrder TickOrder;
		PluginRenderOrder RenderOrder;
	};

	class RYU_API IPlugin
	{
	public:
		IPlugin() = default;
		virtual ~IPlugin() = default;
		virtual bool Initialize(const PluginAPI& api) = 0;
		virtual void OnUpdate(MAYBE_UNUSED const f32 dt) {};
		virtual void OnRender(MAYBE_UNUSED const f32 dt) {};
		virtual void Shutdown() = 0;
	};

	// Auto generated function
	inline const PluginData* GetGeneratedPluginData();

	template<typename T>
	class RYU_API PluginBase : public IPlugin
	{
	public:
		virtual ~PluginBase() = default;
		
		static T* GetInstance() { return s_instance; }
		static const PluginData& GetData() { return *s_data; }

	protected:
		static void ConstructPlugin(T* instance, const PluginData* data = GetGeneratedPluginData())
		{
			s_instance = instance;
			s_data = data;
		}
	private:
		static const PluginData* s_data;
		static T* s_instance;
	};

	template <typename T>
	const PluginData* PluginBase<T>::s_data = nullptr;

	template <typename T>
	T* PluginBase<T>::s_instance = nullptr;
};