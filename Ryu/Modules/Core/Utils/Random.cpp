#include <cassert>
#include <numbers>

namespace Ryu::Utils
{
	namespace
	{
		template<typename T>
		T GetRandomImpl(Random::Generator& gen, T lower, T upper)
		{
			assert(lower <= upper && "lower must be less than or equal to upper");

			auto period = upper - lower + 1; // Get(..., 3, 3) -> 3
			auto number = static_cast<T>(gen() % std::numeric_limits<T>::max());
			return lower + number % period;
		}
	}

	Random::Generator Random::s_mersenne64{ static_cast<Generator::result_type>(std::random_device{}()) };

	Random::Generator* Random::GetGenerator()
	{
		return std::addressof(s_mersenne64);
	}

	void Random::SetSeed(Generator::result_type seed)
	{
		Random::s_mersenne64.seed(seed);
	}

	u64 Random::U64()
	{
		return s_mersenne64();
	}

	f32 Random::Float()
	{
		return f32(s_mersenne64()) / f32(s_mersenne64.max() - 1);
	}

	bool Random::Bool()
	{
		return (bool)GetRandomImpl(s_mersenne64, 0, 1);
	}

	bool Random::Bool(f32 probability)
	{
		return Random::Float() <= probability;
	}

	f32 Random::Radian()
	{
		constexpr auto pi =std::numbers::pi_v<f32>;
		return Random::Range(-pi, pi);
	}

	f32 Random::Degree()
	{
		return Random::Range(-180.0f, 180.0f);
	}

	f32 Random::Range(f32 lower, f32 upper)
	{
		return lower + Random::Float() * (upper - lower);
	}

	i32 Random::Range(i32 lower, i32 upper)
	{
		return GetRandomImpl(s_mersenne64, lower, upper);
	}

	u32 Random::Range(u32 lower, u32 upper)
	{
		return GetRandomImpl(s_mersenne64, lower, upper);
	}
}