#pragma once
#include "Event/Event.h"

namespace Ryu::Game
{
	// Called before world starts to be created
	struct WorldCreationBeginEvent : Event::Event {};

	// Called after world has been created
	struct WorldCreationEndEvent : Event::Event {};

	// Called before world starts to be destroyed
	struct WorldDestructionBeginEvent : Event::Event {};

	// Called after world has been destroyed
	struct WorldDestructionEndEvent : Event::Event {};
}
