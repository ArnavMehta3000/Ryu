#include "CVar.h"
#include <format>

namespace Ryu::Config
{
    template <Internal::CVarAllTypes T>
    inline const T CVar<T>::Get() const noexcept
    {
        // Return default values if nothing is set
        // If string, there is a chance the optional might have a value, 
        // but it will be an empty string
        if constexpr (IsSame<T, std::string>)
        {
            if (m_cliValue.has_value())
            {
                auto& value = m_cliValue.value();
                return value.empty() ? m_value : value;
            }
            else
            {
				return m_value;
            }
		}
        else
        {
            return m_cliValue.value_or(m_value);
        }
    }

    template <Internal::CVarAllTypes T>
    inline bool CVar<T>::Set(const T& value)
    {
        if (IsReadOnly())
        {
            return false;
        }

        m_value = value;
        ExecuteCallback();
        return true;
    }

    template <Internal::CVarAllTypes T>
    inline CVar<T>& CVar<T>::operator=(const T& value)
    {
        Set(value);
        return *this;
    }

    template <Internal::CVarAllTypes T>
	inline std::string CVar<T>::GetAsString() const
	{
        if constexpr (Internal::CVarVectorType<T>)
        {
            std::string result;
            for (size_t i = 0; i < m_value.size(); ++i)
            {
                if (i > 0)
                {
                    result += ",";
                }

                if constexpr (IsSame<T, std::vector<std::string>>)
                {
                    result += m_value[i];
                }
                else if constexpr (IsSame<T, std::vector<bool>>)
                {
                    result += m_value[i] ? "true" : "false";
                }
                else
                {
                    result += std::format("{}", m_value[i]);
                }
            }

            return result;
        }
        else  // For scalars
        {
            if constexpr (IsSame<T, std::string>)
            {
                return m_value;
            }
            else if constexpr (IsSame<T, bool>)
            {
                return m_value ? "true" : "false";
            }
            else
            {
                return std::format("{}", m_value);
            }
        }
	}

    template <Internal::CVarAllTypes T>
    inline bool CVar<T>::SetFromString(std::string_view value)
    {
        if (IsReadOnly())
        {
            return false;
        }

        try
        {
            // For vectors
            if constexpr (Internal::CVarVectorType<T>)
            {
                try
                {
                    T newValue;
                    auto parts = SplitString(value, ',');

                    for (const auto& part : parts)
                    {
                        // Trim whitespace
                        std::string_view trimmed = part;
                        while (!trimmed.empty() && std::isspace(trimmed.front()))
                        {
                            trimmed.remove_prefix(1);
                        }
                        while (!trimmed.empty() && std::isspace(trimmed.back()))
                        {
                            trimmed.remove_suffix(1);
                        }

                        if (trimmed.empty())
                        {
                            continue;
                        }

                        Internal::VectorElementType_t<T> element;
                        if (ParseElement(trimmed, element))
                        {
                            newValue.push_back(std::move(element));
                        }
                        else
                        {
                            return false; // Failed to parse element
                        }
                    }

                    m_value = std::move(newValue);
                    ExecuteCallback();
                    return true;
                }
                catch (...)
                {
                    return false;
                }
            }
            else  // For scalars
            {
                if constexpr (IsSame<T, std::string>)
                {
                    m_value = std::string(value);
                }
                else if constexpr (IsSame<T, bool>)
                {
                    std::string lower(value);
                    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                    m_value = (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
                }
                else if constexpr (IsSame<T, int>)
                {
                    m_value = std::stoi(std::string(value));
                }
                else if constexpr (IsSame<T, float>)
                {
                    m_value = std::stof(std::string(value));
                }
                else if constexpr (IsSame<T, double>)
                {
                    m_value = std::stod(std::string(value));
                }
            }

            ExecuteCallback();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    template <Internal::CVarAllTypes T>
    inline void CVar<T>::ExecuteCallback() const
    {
        if (m_callback)
        {
            m_callback(m_value);
        }
    }

    template <Internal::CVarAllTypes T>
    inline size_t CVar<T>::GetVectorSize() const
    {
        // vector will have a size function
        if constexpr (requires(T t) { { t.size() } -> std::convertible_to<std::size_t>; })
        {
			return m_value.size();
        }
		else
		{
			return 1;
		}
    }

    template<Internal::CVarAllTypes T>
    template<typename U>
    requires Internal::CVarVectorType<U>
    inline bool CVar<T>::PushBack(const Internal::VectorElementType_t<U>& element)
    {
        static_assert(IsSame<T, U>, "Template parameter mismatch");

        if (IsReadOnly())
        {
            return false;
        }

        m_value.push_back(element);
        ExecuteCallback();
        return true;
    }

    template<Internal::CVarAllTypes T>
    template<typename U>
    requires Internal::CVarVectorType<U>
    inline bool CVar<T>::PopBack()
    {
        static_assert(IsSame<T, U>, "Template parameter mismatch");

        if (IsReadOnly() || m_value.empty())
        {
            return false;
        }

        m_value.pop_back();
        ExecuteCallback();
        return true;
    }

    template<Internal::CVarAllTypes T>
    template<typename U>
    requires Internal::CVarVectorType<U>
    inline void CVar<T>::Clear()
    {
        static_assert(IsSame<T, U>, "Template parameter mismatch");

        if (IsReadOnly())
        {
            return;
        }

        m_value.clear();
        ExecuteCallback();
    }

    template<Internal::CVarAllTypes T>
    template<typename U>
    requires Internal::CVarVectorType<U>
    inline size_t CVar<T>::Size() const
    {
        static_assert(IsSame<T, U>, "Template parameter mismatch");
        return m_value.size();
    }

    template<Internal::CVarAllTypes T>
    template<typename U>
    requires Internal::CVarVectorType<U>
    inline bool CVar<T>::SetAt(size_t index, const Internal::VectorElementType_t<U>& value)
    {
        static_assert(IsSame<T, U>, "Template parameter mismatch");

        if (IsReadOnly() || index >= m_value.size())
        {
            return false;
        }

        m_value[index] = value;
        ExecuteCallback();
        return true;
    }

    template<Internal::CVarAllTypes T>
    template<typename U>
    requires Internal::CVarVectorType<U>
    inline const Internal::VectorElementType_t<U>& CVar<T>::GetAt(size_t index) const
    {
        static_assert(IsSame<T, U>, "Template parameter mismatch");
        return m_value.at(index); // Throws if out of bounds
    }

    template<Internal::CVarAllTypes T>
    inline std::vector<std::string_view> CVar<T>::SplitString(std::string_view str, char delimiter) const
    {
        std::vector<std::string_view> result;

        size_t start = 0;
        size_t end = str.find(delimiter);

        while (end != std::string_view::npos)
        {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        // Add the last part
        result.push_back(str.substr(start));

        return result;
    }

    template<Internal::CVarAllTypes T>
    template<typename ElementType>
    inline bool CVar<T>::ParseElement(std::string_view str, ElementType& out) const
    {
        try
        {
            if constexpr (IsSame<ElementType, std::string>)
            {
                out = std::string(str);
                return true;
            }
            else if constexpr (IsSame<ElementType, bool>)
            {
                std::string lower(str);
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                out = (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
                return true;
            }
            else if constexpr (IsSame<ElementType, i32>)
            {
                out = std::stoi(std::string(str));
                return true;
            }
            else if constexpr (IsSame<ElementType, f32>)
            {
                out = std::stof(std::string(str));
                return true;
            }

            return false;
        }
        catch (...)
        {
            return false;
        }
    }
}
