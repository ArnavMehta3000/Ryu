#pragma once
#include "Common/Common.h"
#include "Logging/Logger.h"
#include <Elos/Event/Signal.h>
#include <filesystem>
#include <toml++/toml.hpp>
#include <array>

namespace Ryu::Config
{
	namespace fs = std::filesystem;
	class ConfigValueBase;

	class ConfigBase
	{
		friend class ConfigManager;
		friend class ConfigValueBase;
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
		virtual void Serialize(toml::table& table) const { SerializeAll(table); }
		virtual void Deserialize(const toml::table& table) { DeserializeAll(table); }

		void SerializeAll(toml::table& table) const;
		void DeserializeAll(const toml::table& table);

	private:
		std::string m_filename;
		toml::table m_table;
		bool        m_isDirty      = false;
		bool        m_isRegistered = false;
		std::vector<ConfigValueBase*> m_configValues;
	};

	template<typename T>
	concept TomlCompatible = std::is_same_v<T, std::string>
		|| std::is_same_v<T, const char*>
		|| std::is_integral_v<T>
		|| std::is_floating_point_v<T>
		|| std::is_same_v<T, bool>
		|| std::is_same_v<std::remove_cvref_t<T>, toml::array>
		|| std::is_same_v<std::remove_cvref_t<T>, toml::table>;

	// Base class for all ConfigValue types
	class ConfigValueBase
	{
	public:
		ConfigValueBase(ConfigBase* owner, const std::string_view& section, const std::string_view& key)
			: m_owner(owner), m_section(section.data()), m_key(key.data()) 
		{
			if (owner)
			{
				owner->m_configValues.push_back(this);
			}
		}

		virtual ~ConfigValueBase() = default;

		inline NODISCARD const std::string& GetSection() const { return m_section; }
		inline NODISCARD const std::string& GetKey() const { return m_key; }

		virtual void Serialize(toml::table& table) const = 0;
		virtual void Deserialize(const toml::table& table) = 0;

	protected:
		void MarkDirty()
		{
			if (m_owner)
			{
				m_owner->MarkDirty();
			}
		}

		toml::table& GetOrCreateSectionTable(toml::table& table) const;		
		const toml::table* GetSectionTable(const toml::table& table) const;

	protected:
		RYU_LOG_DECLARE_CATEGORY(ConfigValue);
		ConfigBase* m_owner;
		std::string m_section;
		std::string m_key;
	};

	template<typename T>
	class ConfigValue : ConfigValueBase
	{
	public:
		using ValueType = T;
		using ValidationCallback = std::function<bool(const T&)>;
		using ChangeCallback = Elos::Signal<const T&, const T&>;

		ConfigValue(ConfigBase* owner, const std::string_view section, const std::string_view key,
			const ValueType& defaultValue = ValueType{}, ValidationCallback validator = nullptr)
			: ConfigValueBase(owner, section, key), m_value(defaultValue), m_validator(validator)
		{
		}

		operator ValueType() const { return m_value; }
		inline NODISCARD const T& Get() const { return m_value; }

		ConfigValue& operator=(const ValueType& value)
		{
			Set(value);
			return *this;
		}

		void Set(const ValueType& value)
		{
			if (m_value != value && IsValid(value))
			{
				ValueType oldValue = m_value;
				m_value = value;
				MarkDirty();
				
				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		void Serialize(toml::table& table) const
		{
			if constexpr (TomlCompatible<ValueType>)
			{
				toml::table& sectionTable = GetOrCreateSectionTable(table);
				sectionTable.insert_or_assign(m_key, m_value);
			}
		}

		void Deserialize(const toml::table& table)
		{
			const toml::table* sectionTable = GetSectionTable(table);
			if (!sectionTable)
			{
				return;
			}

			DeserializeFromTable(*sectionTable);
		}

		bool IsValid(const ValueType& value) const
		{
			return m_validator ? m_validator(value) : true;
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
					else
					{
						RYU_LOG_WARN(LogConfigValue,
							"Type mismatch for '{}': expected string", m_key);
					}
				}
				else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
				{
					if (auto val = node->as_integer())
					{
						m_value = static_cast<T>(val->get());
					}
					else
					{
						RYU_LOG_WARN(LogConfigValue,
							"Type mismatch for '{}': expected integer", m_key);
					}
				}
				else if constexpr (std::is_floating_point_v<T>)
				{
					if (auto val = node->as_floating_point())
					{
						m_value = static_cast<T>(val->get());
					}
					else
					{
						RYU_LOG_WARN(LogConfigValue,
							"Type mismatch for '{}': expected floating point", m_key);
					}
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					if (auto val = node->as_boolean())
					{
						m_value = val->get();
					}
					else
					{
						RYU_LOG_WARN(LogConfigValue,
							"Type mismatch for '{}': expected boolean", m_key);
					}
				}
			}
		}

	public:
		ChangeCallback OnChangedSingal;

	private:
		ValueType          m_value;
		ValidationCallback m_validator;
	};

	// Specialization for enum
	template <IsEnum T>
	class ConfigValue<T> : public ConfigValueBase
	{
	public:
		using ValueType = T;
		using ValidationCallback = std::function<bool(const T&)>;
		using ChangeCallback = Elos::Signal<const T&, const T&>;

		ConfigValue(ConfigBase* owner, const std::string_view section, const std::string_view key,
			const ValueType& defaultValue = ValueType{}, ValidationCallback validator = nullptr)
			: ConfigValueBase(owner, section, key), m_value(defaultValue), m_validator(validator)
		{
		}

		operator ValueType() const { return m_value; }
		inline NODISCARD const ValueType& Get() const { return m_value; }

		ConfigValue& operator=(const ValueType& value)
		{
			Set(value);
			return *this;
		}

		void Set(const ValueType& value)
		{
			if (m_value != value && IsValid(value))
			{
				ValueType oldValue = m_value;
				m_value = value;
				MarkDirty();
				
				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		void Serialize(toml::table& table) const
		{
			static_assert(std::is_enum_v<T>, "T must be an enum type");

			const std::string enumStr(::Ryu::EnumToString(m_value));
			toml::table& sectionTable = GetOrCreateSectionTable(table);
			sectionTable.insert_or_assign(m_key, enumStr);
		}

		void Deserialize(const toml::table& table)
		{
			const toml::table* sectionTable = GetSectionTable(table);
			if (!sectionTable)
			{
				return;
			}

			DeserializeFromTable(*sectionTable);
		}

		bool IsValid(const ValueType& value) const
		{
			return m_validator ? m_validator(value) : true;
		}

	private:
		void DeserializeFromTable(const toml::table& table)
		{
			if (auto node = table.get(m_key))
			{
				if (auto strVal = node->as_string())
				{
					m_value = ::Ryu::StringToEnum(strVal->get(), m_value);
				}
				else
				{
					RYU_LOG_WARN(LogConfigValue,
						"Type mismatch for '{}': expected string for enum", m_key);
				}
			}
		}

	public:
		ChangeCallback OnChangedSingal;

	private:
		ValueType          m_value;
		ValidationCallback m_validator;
	};


	// Special handling for BitMaskEnabled enums (flags)
	template<typename T> requires std::is_enum_v<T>
	class ConfigValue<T> : public ConfigValueBase
	{
	public:
		using ValueType = T;
		using ValidationCallback = std::function<bool(const T&)>;
		using ChangeCallback = Elos::Signal<const T&, const T&>;

		ConfigValue(ConfigBase* owner, const std::string_view section, const std::string_view key,
			ValueType& defaultValue = ValueType{}, ValidationCallback validator = nullptr)
			: ConfigValueBase(owner, section, key), m_value(defaultValue), m_validator(validator)
		{
		}

		operator ValueType() const { return m_value; }
		inline NODISCARD const ValueType& Get() const { return m_value; }

		ConfigValue& operator=(const ValueType& value)
		{
			Set(value);
			return *this;
		}

		void Set(const ValueType& value)
		{
			if (m_value != value && IsValid(value))
			{
				ValueType oldValue = m_value;
				m_value = value;
				MarkDirty();
				
				OnChangedSingal.Emit(oldValue, m_value);
			}
		}
		
		bool HasFlag(ValueType flag) const
		{
			return (m_value & flag) == flag;
		}

		void SetFlag(T flag, bool value = true)
		{
			if (value && !HasFlag(flag))
			{
				ValueType oldValue = m_value;
				m_value |= flag;
				MarkDirty();

				OnChangedSingal.Emit(oldValue, m_value);
			}
			else if (HasFlag(flag))
			{
				ValueType oldValue = m_value;
				m_value &= ~flag;
				MarkDirty();

				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		void Serialize(toml::table& table) const
		{
			toml::array flagsArray;

			// Check each possible flag value
			for (u32 i = 0; i < 32; ++i) // Assuming 32-bit enum
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

			auto& sectionTable = GetOrCreateSectionTable(table);
			sectionTable.insert_or_assign(m_key, std::move(flagsArray));
		}

		void Deserialize(const toml::table& table)
		{
			const toml::table* sectionTable = GetSectionTable(table);
			if (!sectionTable)
			{
				return;
			}

			DeserializeFromTable(*sectionTable);
		}
		
		bool IsValid(const ValueType& value) const
		{
			return m_validator ? m_validator(value) : true;
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
								for (u32 i = 0; i < 32; ++i) // Assuming 32-bit enum
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
						for (u32 i = 0; i < 32; ++i) // Assuming 32-bit enum
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
				else
				{
					RYU_LOG_WARN(LogConfigValue,
						"Type mismatch for '{}': expected array for flags", m_key);
				}
			}
		}
	
	public:
		ChangeCallback OnChangedSingal;

	private:
		ValueType          m_value;
		ValidationCallback m_validator;
	};

	namespace Internal
	{
		// Helper to check if type is std::vector
		template<typename T>
		struct IsVector : std::false_type {};
		
		template<typename T, typename A>
		struct IsVector<std::vector<T, A>> : std::true_type {};

		template<typename T>
		inline constexpr bool IsVector_v = IsVector<T>::value;

		// Helper to check if type is std::array
		template<typename T>
		struct IsArray : std::false_type {};

		template<typename T, std::size_t N>
		struct IsArray<std::array<T, N>> : std::true_type {};

		template<typename T>
		inline constexpr bool IsArray_v = IsArray<T>::value;

		// Helper to get the element type from a container
		template<typename T>
		struct ContainerValueType { using type = void; };

		template<typename T, typename A>
		struct ContainerValueType<std::vector<T, A>> { using type = T; };

		template<typename T, std::size_t N>
		struct ContainerValueType<std::array<T, N>> { using type = T; };

		template<typename T>
		using ContainerValueType_t = typename ContainerValueType<T>::type;
	}

	// ConfigValue specialization for std::vector
	template<typename T>
		requires Internal::IsVector_v<T> && TomlCompatible<Internal::ContainerValueType_t<T>>
	class ConfigValue<T> : public ConfigValueBase
	{
	public:
		using ValueType = T;
		using ElementType = Internal::ContainerValueType_t<T>;
		using ValidationCallback = std::function<bool(const T&)>;
		using ChangeCallback = Elos::Signal<const T&, const T&>;

		ConfigValue(ConfigBase* owner, const std::string_view section, const std::string_view key,
			const ValueType& defaultValue = ValueType{}, ValidationCallback validator = nullptr)
			: ConfigValueBase(owner, section, key), m_value(defaultValue), m_validator(validator)
		{
		}

		operator const T& () const { return m_value; }
		inline NODISCARD const T& Get() const { return m_value; }

		ConfigValue& operator=(const T& value)
		{
			Set(value);
			return *this;
		}

		void Set(const T& value)
		{
			if (m_value != value && IsValid(value))
			{
				ValueType oldValue = m_value;
				m_value = value;
				MarkDirty();

				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		NODISCARD const ElementType& operator[](size_t index) const
		{
			return m_value[index];
		}

		void SetElement(size_t index, const ElementType& value)
		{
			if (index < m_value.size() && m_value[index] != value)
			{
				ValueType oldValue = m_value;
				m_value[index] = value;
				MarkDirty();

				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		void Serialize(toml::table& table) const
		{
			// Convert to TOML array
			toml::array arr;
			for (const auto& element : m_value)
			{
				arr.push_back(element);
			}

			toml::table& sectionTable = GetOrCreateSectionTable(table);
			sectionTable.insert_or_assign(m_key, std::move(arr));
		}

		void Deserialize(const toml::table& table)
		{
			const toml::table* sectionTable = GetSectionTable(table);
			if (!sectionTable)
			{
				return;
			}

			DeserializeFromTable(*sectionTable);
		}

		bool IsValid(const ValueType& value) const
		{
			return m_validator ? m_validator(value) : true;
		}

	private:
		void DeserializeFromTable(const toml::table& table)
		{
			if (auto node = table.get(m_key))
			{
				if (auto arr = node->as_array())
				{
					m_value.clear();
					m_value.reserve(arr->size());

					// Add each element
					for (const auto& element : *arr)
					{
						if constexpr (std::is_same_v<ElementType, std::string>)
						{
							if (auto val = element.as_string())
							{
								m_value.push_back(val->get());
							}
						}
						else if constexpr (std::is_integral_v<ElementType> && !std::is_same_v<ElementType, bool>)
						{
							if (auto val = element.as_integer())
							{
								m_value.push_back(static_cast<ElementType>(val->get()));
							}
						}
						else if constexpr (std::is_floating_point_v<ElementType>)
						{
							if (auto val = element.as_floating_point())
							{
								m_value.push_back(static_cast<ElementType>(val->get()));
							}
						}
						else if constexpr (std::is_same_v<ElementType, bool>)
						{
							if (auto val = element.as_boolean())
							{
								m_value.push_back(val->get());
							}
						}
					}
				}
				else
				{
					RYU_LOG_WARN(LogConfigValue,
						"Type mismatch for '{}': expected array", m_key);
				}
			}
		}

	public:
		ChangeCallback OnChangedSingal;

	private:
		ValueType          m_value;
		ValidationCallback m_validator;
	};

	// ConfigValue specialization for std::array
	template<typename T>
		requires Internal::IsArray_v<T>&& TomlCompatible<Internal::ContainerValueType_t<T>>
	class ConfigValue<T> : public ConfigValueBase
	{
	public:
		using ValueType = T;
		static constexpr std::size_t Size = std::tuple_size_v<T>;
		using ElementType = Internal::ContainerValueType_t<T>;
		using ValidationCallback = std::function<bool(const T&)>;
		using ChangeCallback = Elos::Signal<const T&, const T&>;

		ConfigValue(ConfigBase* owner, const std::string_view section, const std::string_view key,
			const ValueType& defaultValue = ValueType{}, ValidationCallback validator = nullptr)
			: ConfigValueBase(owner, section, key), m_value(defaultValue), m_validator(validator)
		{
		}

		operator const T& () const { return m_value; }
		inline NODISCARD const T& Get() const { return m_value; }

		ConfigValue& operator=(const T& value)
		{
			Set(value);
			return *this;
		}

		void Set(const T& value)
		{
			if (m_value != value && IsValid(value))
			{
				ValueType oldValue = m_value;
				m_value = value;
				MarkDirty();

				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		NODISCARD const ElementType& operator[](size_t index) const
		{
			return m_value[index];
		}

		void SetElement(size_t index, const ElementType& value)
		{
			if (index < Size && m_value[index] != value)
			{
				ValueType oldValue = m_value;
				m_value[index] = value;
				MarkDirty();

				OnChangedSingal.Emit(oldValue, m_value);
			}
		}

		void Serialize(toml::table& table) const
		{
			toml::array arr;
			for (const auto& element : m_value)
			{
				arr.push_back(element);
			}

			toml::table& sectionTable = GetOrCreateSectionTable(table);
			sectionTable.insert_or_assign(m_key, std::move(arr));
		}

		void Deserialize(const toml::table& table)
		{
			const toml::table* sectionTable = GetSectionTable(table);
			if (!sectionTable)
			{
				return;
			}

			DeserializeFromTable(*sectionTable);
		}

	private:
		void DeserializeFromTable(const toml::table& table)
		{
			if (auto node = table.get(m_key))
			{
				if (auto arr = node->as_array())
				{
					for (size_t i = 0; i < std::min(Size, arr->size()); ++i)
					{
						const auto& element = (*arr)[i];

						if constexpr (std::is_same_v<ElementType, std::string>)
						{
							if (auto val = element.as_string())
							{
								m_value[i] = val->get();
							}
						}
						else if constexpr (std::is_integral_v<ElementType> && !std::is_same_v<ElementType, bool>)
						{
							if (auto val = element.as_integer())
							{
								m_value[i] = static_cast<ElementType>(val->get());
							}
						}
						else if constexpr (std::is_floating_point_v<ElementType>)
						{
							if (auto val = element.as_floating_point())
							{
								m_value[i] = static_cast<ElementType>(val->get());
							}
						}
						else if constexpr (std::is_same_v<ElementType, bool>)
						{
							if (auto val = element.as_boolean())
							{
								m_value[i] = val->get();
							}
						}
					}
				}
			}
		}

	public:
		ChangeCallback OnChangedSingal;

	private:
		ValueType          m_value;
		ValidationCallback m_validator;
	};

	using ConfigSection = std::string_view;
}
