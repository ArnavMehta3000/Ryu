#pragma once

namespace Ryu::Scripting
{
	class ScriptComponent
	{
	public:
		virtual ~ScriptComponent() = default;

		virtual void Start() {}
		virtual void Update(double dt) {}

	};
}
