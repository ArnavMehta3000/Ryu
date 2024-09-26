#pragma once
#include <Core/Utils/Factory.h>

namespace Ryu
{
	class NativeScript : public Factory<NativeScript, std::string_view>
	{
	public:
		NativeScript(Key) {}
		virtual ~NativeScript()  = default;
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
	};
}

// TEST CODE
class MyScript : public Ryu::NativeScript::Registrar<MyScript>
{
public:
	MyScript(std::string_view sv) : m_str(sv) {}

	static std::string_view GetStaticName() { return "MyScriptComponent"; }

	void OnCreate() override {}
	void OnDestroy() override {}

private:
	std::string m_str;
};

void ExampleUse()
{
	auto script = Ryu::NativeScript::Create(MyScript::GetStaticName(), "Hello World");
	script->OnCreate();
}
