namespace Ryu::Game
{
	template<Utils::Serializable T>
	inline toml::table World::SerializeComponent(EntityHandle handle)
	{
		toml::table table;
		Utils::Serialize(m_registry.get<T>(handle), table);
		return table;
	}
	
	template<Utils::Serializable ...Ts>
	inline auto World::SerializeComponents(EntityHandle handle)
	{
		return std::make_tuple(
			([&]()
			{
				toml::table table;
				Utils::Serialize(m_registry.get<Ts>(handle), table);
				return table;
			})()...);
	}
	
	template<Utils::Deserializable T>
	inline void World::DeserializeIntoExistingComponent(EntityHandle handle, const toml::table& table)
	{
		Utils::Deserialize(m_registry.get<T>(handle), table);
	}
}
