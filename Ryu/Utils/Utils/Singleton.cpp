#include "Singleton.h"

namespace Ryu::Utils
{
	SingletonRegistry::SingletonMap SingletonRegistry::s_singletons;

	const SingletonRegistry::SingletonMap& SingletonRegistry::GetAllRegisteredSingletons()
	{
		return s_singletons;
	}
}
