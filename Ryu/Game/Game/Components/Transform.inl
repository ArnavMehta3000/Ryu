#include "Utils/Serializer.h"

namespace Ryu::Utils
{
	// Serializer for Transform
	template<>
	struct Serializer<Game::Transform>
	{
		static void Serialize(const Game::Transform& t, toml::table& table)
		{
			table.insert("position", toml::array{ t.Position.x, t.Position.y, t.Position.z });
			table.insert("rotation", toml::array{ t.Rotation.x, t.Rotation.y, t.Rotation.z, t.Rotation.w });
			table.insert("scale", toml::array{ t.Scale.x, t.Scale.y, t.Scale.z });
		}
	};

	// Deserializer for Transform
	template<>
	struct Deserializer<Game::Transform>
	{
		static Game::Transform Deserialize(const toml::table& table)
		{
            Game::Transform t;
			Deserialize(t, table);
            return t;
		}

		static void Deserialize(Game::Transform& t, const toml::table& table)
		{
			if (auto pos = table["position"].as_array())
			{
				t.Position.x = pos->at(0).value_or(0.0f);
				t.Position.y = pos->at(1).value_or(0.0f);
				t.Position.z = pos->at(2).value_or(0.0f);
			}

			if (auto rot = table["rotation"].as_array())
			{
				t.Rotation.x = rot->at(0).value_or(0.0f);
				t.Rotation.y = rot->at(1).value_or(0.0f);
				t.Rotation.z = rot->at(2).value_or(0.0f);
				t.Rotation.w = rot->at(3).value_or(1.0f);
			}

			if (auto scale = table["scale"].as_array())
			{
				t.Scale.x = scale->at(0).value_or(1.0f);
				t.Scale.y = scale->at(1).value_or(1.0f);
				t.Scale.z = scale->at(2).value_or(1.0f);
			}
		}
	};
}
