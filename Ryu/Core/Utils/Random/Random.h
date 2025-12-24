#pragma once
#include "Core/Common/Common.h"
#include <random>
#include <limits>

namespace Ryu::Utils
{
	/**
	 * @brief Static class for random number generation
	 */
	class Random
	{
	public:
		using Generator = std::mersenne_twister_engine<std::uint_fast64_t, 64, 312, 156, 31,
			0xb5026f5aa96619e9, 29,
			0x5555555555555555, 17,
			0x71d67fffeda60000, 37,
			0xfff7eee000000000, 43, 6364136223846793005>;

	public:
		/**
		 * @brief Get the Generator of the random number generator
		 * @return Pointer to the generator
		 */
		static RYU_API Generator* GetGenerator();

		/**
		 * @brief Set the seed of the random number generator
		 * @param seed New seed
		 */
		static RYU_API void SetSeed(Generator::result_type seed);

		/**
		 * @brief Get a random unsigned 64-bit integer
		 * @return Random unsigned 64-bit integer
		 */
		static RYU_API u64 U64();

		/**
		 * @brief Get a random float
		 * @return Random float
		 */
		static RYU_API f32 Float();

		/**
		 * @brief Get a random bool
		 * @return Random bool
		 */
		static RYU_API bool Bool();

		/**
		 * @brief Get a random bool with the given probability
		 * @param probability Probability of returning true
		 * @return Random bool
		 */
		static RYU_API bool Bool(f32 probability);

		/**
		 * @brief Get a random radian value between -pi and pi
		 * @return Random radian
		 */
		static RYU_API f32 Radian();

		/**
		 * @brief Get a random degree value between -180 and 180
		 * @return Random degree
		 */
		static RYU_API f32 Degree();

		/**
		 * @brief Get a random float in the range [lower, upper]
		 * @param lower The lower bound
		 * @param upper The upper bound
		 * @return Random float
		 */
		static RYU_API f32 Range(f32 lower, f32 upper);

		/**
		 * @brief Get a random integer in the range [lower, upper]
		 * @param lower The lower bound
		 * @param upper The upper bound
		 * @return Random integer
		 */
		static RYU_API i32 Range(i32 lower, i32 upper);

		/**
		 * @brief Get a random unsigned integer in the range [lower, upper]
		 * @param lower The lower bound
		 * @param upper The upper bound
		 * @return Random unsigned integer
		 */
		static RYU_API u32 Range(u32 lower, u32 upper);

	public:
		/**
		 * @brief The maximum value of a random unsigned 64-bit integer
		 */
		static constexpr RYU_API u64 MaxValue = std::numeric_limits<u64>::max() / 2 - 1;

		/**
		 * @brief The minimum value of a random unsigned 64-bit integer
		 */
		static constexpr RYU_API u64 MinValue = std::numeric_limits<u64>::min() / 2 - 1;

	private:
		static Generator s_mersenne64;
	};
}
