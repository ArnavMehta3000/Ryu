#pragma once
#include "Common/ObjectMacros.h"
#include "Common/Enum.h"
#include "Logger/LogCategory.h"
#include <string>
#include <filesystem>
#include <toml++/toml.hpp>
#include <type_traits>

namespace Ryu::Config
{
	namespace fs = std::filesystem;

	class ConfigBase
	{
		friend class ConfigManager;
		RYU_LOG_CATEGORY(ConfigManager);
	public:
		ConfigBase(const std::string& filename) : m_filename(filename) {}
		virtual ~ConfigBase();

		bool Load();
		bool Save() const;
		inline void MarkDirty() { m_isDirty = true; }

		inline NODISCARD bool IsDirty() const { return m_isDirty; }
		inline NODISCARD const toml::table& GetTable() const { return m_table; }
		inline NODISCARD toml::table& GetTable() { return m_table; }

		NODISCARD fs::path GetFullPath() const;

	protected:
		virtual void Serialize(toml::table& table) const = 0;
		virtual void Deserialize(const toml::table& table) = 0;

	private:
		std::string m_filename;
		toml::table m_table;
		bool        m_isDirty = false;
		bool        m_isRegistered = false;
	};

	template<typename T>
	concept TomlCompatible =
		std::is_same_v<T, std::string> ||
		std::is_same_v<T, const char*> ||
		std::is_integral_v<T> ||
		std::is_floating_point_v<T> ||
		std::is_same_v<T, bool> ||
		std::is_same_v<std::remove_cvref_t<T>, toml::array> ||
		std::is_same_v<std::remove_cvref_t<T>, toml::table>;

	template<typename T>
	class ConfigValue
	{
	public:
		// Constructor now separates the section name and key
		ConfigValue(ConfigBase* owner, const std::string& section, const std::string& key, const T& defaultValue = T{})
			: m_owner(owner), m_section(section), m_key(key), m_value(defaultValue)
		{
		}

		// Implicit conversion to T
		operator T() const { return m_value; }

		// Assignment operator
		ConfigValue& operator=(const T& value)
		{
			if (m_value != value)
			{
				m_value = value;
				m_owner->MarkDirty();
			}
			return *this;
		}

		NODISCARD const T& Get() const { return m_value; }

		void Set(const T& value)
		{
			if (m_value != value)
			{
				m_value = value;
				m_owner->MarkDirty();
			}
		}

		NODISCARD const std::string& GetSection() const { return m_section; }
		NODISCARD const std::string& GetKey() const { return m_key; }

		void Serialize(toml::table& table) const
		{
			if constexpr (TomlCompatible<T>)
			{
				// Make sure section table exists
				if (!m_section.empty())
				{
					if (!table.contains(m_section))
					{
						table.insert(m_section, toml::table{});
					}
					auto& sectionTable = *table[m_section].as_table();
					sectionTable.insert_or_assign(m_key, m_value);
				}
				else
				{
					// Root level key
					table.insert_or_assign(m_key, m_value);
				}
			}
		}

		void Deserialize(const toml::table& table)
		{
			if (!m_section.empty())
			{
				// Look for the section first
				if (auto sectionNode = table.get(m_section))
				{
					if (auto sectionTable = sectionNode->as_table())
					{
						DeserializeFromTable(*sectionTable);
					}
				}
			}
			else
			{
				// Root level key
				DeserializeFromTable(table);
			}
		}

	private:
		void DeserializeFromTable(const toml::table& table)
		{
			if (auto node = table.get(m_key))
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					if (auto val = node->as_string())
					{
						m_value = val->get();
					}
				}
				else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
				{
					if (auto val = node->as_integer())
					{
						m_value = static_cast<T>(val->get());
					}
				}
				else if constexpr (std::is_floating_point_v<T>)
				{
					if (auto val = node->as_floating_point())
					{
						m_value = static_cast<T>(val->get());
					}
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					if (auto val = node->as_boolean())
					{
						m_value = val->get();
					}
				}
			}
		}

		ConfigBase* m_owner;
		std::string m_section;
		std::string m_key;
		T m_value;
	};

	// Specialization for enum
	template <IsEnum T>
	class ConfigValue<T>
	{
	public:
		ConfigValue(ConfigBase* owner, const std::string& section, const std::string& key, const T& defaultValue = T{})
			: m_owner(owner), m_section(section), m_key(key), m_value(defaultValue)
		{
		}

		operator T() const { return m_value; }

		ConfigValue& operator=(const T& value)
		{
			if (m_value != value)
			{
				m_value = value;
				m_owner->MarkDirty();
			}
			return *this;
		}

		NODISCARD const T& Get() const { return m_value; }

		void Set(const T& value)
		{
			if (m_value != value)
			{
				m_value = value;
				m_owner->MarkDirty();
			}
		}

		void Serialize(toml::table& table) const
		{
			std::string enumStr = std::string(::Ryu::EnumToString(m_value));

			// Make sure section table exists
			if (!m_section.empty())
			{
				if (!table.contains(m_section))
				{
					table.insert(m_section, toml::table{});
				}
				auto& sectionTable = *table[m_section].as_table();
				sectionTable.insert_or_assign(m_key, enumStr);
			}
			else
			{
				// Root level key
				table.insert_or_assign(m_key, enumStr);
			}
		}

		void Deserialize(const toml::table& table)
		{
			if (!m_section.empty())
			{
				// Look for the section first
				if (auto sectionNode = table.get(m_section))
				{
					if (auto sectionTable = sectionNode->as_table())
					{
						DeserializeFromTable(*sectionTable);
					}
				}
			}
			else
			{
				// Root level key
				DeserializeFromTable(table);
			}
		}

	private:
		void DeserializeFromTable(const toml::table& table)
		{
			if (auto node = table.get(m_key))
			{
				if (auto strVal = node->as_string())
				{
					// Convert string to enum
					m_value = StringToEnum(strVal->get(), m_value);
				}
			}
		}

		ConfigBase* m_owner;
		std::string m_section;
		std::string m_key;
		T m_value;

	};


	// Special handling for BitMaskEnabled enums (flags)
	template<BitMaskEnabled T>
	class ConfigValue<T>
	{
	public:
		ConfigValue(ConfigBase* owner, const std::string& section, const std::string& key, const T& defaultValue = T{})
			: m_owner(owner), m_section(section), m_key(key), m_value(defaultValue)
		{
		}

		operator T() const { return m_value; }

		ConfigValue& operator=(const T& value)
		{
			if (m_value != value)
			{
				m_value = value;
				m_owner->MarkDirty();
			}
			return *this;
		}

		NODISCARD const T& Get() const { return m_value; }

		void Set(const T& value)
		{
			if (m_value != value)
			{
				m_value = value;
				m_owner->MarkDirty();
			}
		}
		
		bool HasFlag(T flag) const
		{
			return (m_value & flag) == flag;
		}

		void SetFlag(T flag, bool value = true)
		{
			if (value && !HasFlag(flag))
			{
				m_value |= flag;
				m_owner->MarkDirty();
			}
			else if (HasFlag(flag))
			{
				m_value &= ~flag;
				m_owner->MarkDirty();
			}
		}

		void Serialize(toml::table& table) const
		{
			toml::array flagsArray;

			// Check each possible flag value
			for (int i = 0; i < 32; ++i) // Assuming 32-bit enum
			{
				T flag = static_cast<T>(1 << i);

				// If the flag is set in the value, add its name to the array
				if (HasFlag(flag))
				{
					flagsArray.push_back(std::string(::Ryu::EnumToString(flag)));
				}
			}

			// If no flags are set, add an empty string or marker
			if (flagsArray.empty())
			{
				flagsArray.push_back("None");
			}

			// Make sure section table exists
			if (!m_section.empty())
			{
				if (!table.contains(m_section))
				{
					table.insert(m_section, toml::table{});
				}
				auto& sectionTable = *table[m_section].as_table();
				sectionTable.insert_or_assign(m_key, std::move(flagsArray));
			}
			else
			{
				// Root level key
				table.insert_or_assign(m_key, std::move(flagsArray));
			}
		}

		void Deserialize(const toml::table& table)
		{
			if (!m_section.empty())
			{
				// Look for the section first
				if (auto sectionNode = table.get(m_section))
				{
					if (auto sectionTable = sectionNode->as_table())
					{
						DeserializeFromTable(*sectionTable);
					}
				}
			}
			else
			{
				// Root level key
				DeserializeFromTable(table);
			}
		}

		private:
			void DeserializeFromTable(const toml::table& table)
			{
				if (auto node = table.get(m_key))
				{
					if (auto arrayVal = node->as_array())
					{
						// Start with no flags set
						m_value = static_cast<T>(0);

						// Add each flag from the array
						for (const auto& flagNode : *arrayVal)
						{
							if (auto flagStr = flagNode.as_string())
							{
								std::string flagName = flagStr->get();
								if (flagName != "None")
								{
									// Find the matching enum value
									for (int i = 0; i < 32; ++i) // Assuming 32-bit enum
									{
										T flag = static_cast<T>(1 << i);
										if (std::string(EnumToString(flag)) == flagName)
										{
											m_value |= flag;
											break;
										}
									}
								}
							}
						}
					}
					else if (auto strVal = node->as_string())
					{
						// Handle case where it might be a single string instead of an array
						std::string flagName = strVal->get();
						if (flagName != "None")
						{
							// Find the matching enum value
							for (int i = 0; i < 32; ++i) // Assuming 32-bit enum
							{
								T flag = static_cast<T>(1 << i);
								if (std::string(EnumToString(flag)) == flagName)
								{
									m_value = flag;
									break;
								}
							}
						}
						else
						{
							m_value = static_cast<T>(0);
						}
					}
				}
			}

			ConfigBase* m_owner;
			std::string m_section;
			std::string m_key;
			T m_value;
	};
}

// Macro to help with declaring config structs
#define RYU_DECLARE_CONFIG(ThisType, filename)                                                     \
	public:                                                                                        \
		static ThisType& Get() { return ::Ryu::Utils::Singleton<ThisType>::Get(); }                \
	private:                                                                                       \
		friend class ::Ryu::Utils::Singleton<ThisType>;                                            \
		friend class ::Ryu::Config::ConfigBase;                                                    \
	public:                                                                                        \
		ThisType() : ::Ryu::Config::ConfigBase(filename) { Load(); ::Ryu::Config::ConfigManager::Get().RegisterConfig(this); }
