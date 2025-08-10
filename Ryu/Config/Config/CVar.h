#pragma once
#include "Common/Common.h"
#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <optional>

namespace Ryu::Config
{
	namespace Internal
	{

		// Helper to get the element type from vector
		template<typename T>
		struct VectorElementType { using type = T; };

		template<typename U>
		struct VectorElementType<std::vector<U>> { using type = U; };

		template<typename T>
		using VectorElementType_t = typename VectorElementType<T>::type;
		
		template <typename T>
		concept CVarType = IsSame<T, i32> || IsSame<T, f32> || IsSame<T, bool> || IsSame<T, std::string>;
	
		template <typename T>
		concept CVarVectorType = !IsSame<T, std::string> && requires(T t)
		{
			typename T::value_type;
			{ t.size() } -> std::convertible_to<std::size_t>;
			{ t.push_back(std::declval<typename T::value_type>()) };
		} && 
		(IsSame<typename T::value_type, i32>
		|| IsSame<typename T::value_type, f32>
		|| IsSame<typename T::value_type, bool>
		|| IsSame<typename T::value_type, std::string>);

		template <typename T>
		concept CVarAllTypes = CVarType<T> || CVarVectorType<T>;
	}

	enum class CVarFlags
	{
		None     = 0,
		ReadOnly = RYU_ENUM_BIT(0),
		Debug    = RYU_ENUM_BIT(1),  // Debug only
		// Maybe add more in the future like Hidden/ThreadSpecific/Editor/etc
	};
	RYU_ENUM_ENABLE_BITMASK_OPERATORS(CVarFlags)

	class ICVarBase
	{
	public:
		virtual ~ICVarBase()                               = default;
		virtual std::string GetAsString() const            = 0;
		virtual bool SetFromString(std::string_view value) = 0;
		virtual std::string_view GetName() const           = 0;
		virtual std::string_view GetDescription() const    = 0;
		virtual CVarFlags GetFlags() const                 = 0;
		virtual bool IsReadOnly() const                    = 0;
		virtual void ExecuteCallback() const               = 0;
		virtual bool IsVector() const                      = 0;
		virtual size_t GetVectorSize() const               = 0;
	};

	template <Internal::CVarAllTypes T>
	class CVar : public ICVarBase
	{
		friend class CmdLine;
	public:
		using ValueType = T;
		using CallbackType = std::function<void(const T&)>;

		// Auto registration
		constexpr CVar(std::string_view name, const T defaultValue, std::string_view description = "", CVarFlags flags = CVarFlags::None, CallbackType callback = nullptr);
		
		const T Get() const noexcept;
		bool Set(const T& value);
		operator const T () const noexcept { return Get(); }
		CVar& operator=(const T& value);

		template<typename U = T>
		requires Internal::CVarVectorType<U>
		bool PushBack(const Internal::VectorElementType_t<U>& element);

		template<typename U = T>
		requires Internal::CVarVectorType<U>
		bool PopBack();

		template<typename U = T>
		requires Internal::CVarVectorType<U>
		void Clear();

		template<typename U = T>
		requires Internal::CVarVectorType<U>
		size_t Size() const;

		template<typename U = T>
		requires Internal::CVarVectorType<U>
		bool SetAt(size_t index, const Internal::VectorElementType_t<U>& value);

		template<typename U = T>
		requires Internal::CVarVectorType<U>
		const Internal::VectorElementType_t<U>& GetAt(size_t index) const;

		std::string GetAsString() const override;
		bool SetFromString(std::string_view value) override;
		std::string_view GetName() const override { return m_name; }
		std::string_view GetDescription() const override { return m_description; }
		CVarFlags GetFlags() const override { return m_flags; }
		bool IsReadOnly() const override { return (m_flags & CVarFlags::ReadOnly) != CVarFlags::None; }
		void ExecuteCallback() const override;
		bool IsVector() const override { return Internal::CVarVectorType<T>; }
		size_t GetVectorSize() const override;

	private:
		std::vector<std::string_view> SplitString(std::string_view str, char delimiter = ',') const;
		
		template<typename ElementType>
		bool ParseElement(std::string_view str, ElementType& out) const;

	private:
		std::string m_name;
		std::string m_description;
		T m_value;
		mutable std::optional<T> m_cliValue;
		CVarFlags m_flags;
		CallbackType m_callback;
	};

	using CVarVecInt = CVar<std::vector<i32>>;
	using CVarVecFloat = CVar<std::vector<f32>>;
	using CVarVecBool = CVar<std::vector<bool>>;
	using CVarVecString = CVar<std::vector<std::string>>;
}

#include "Config/CVar.inl"
