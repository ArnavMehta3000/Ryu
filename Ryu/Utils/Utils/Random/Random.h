#pragma once
#include "Common/Common.h"
#include <random>
#include <limits>

namespace Ryu::Utils
{
	class Random
	{
	public:
		using Generator = std::mersenne_twister_engine<std::uint_fast64_t, 64, 312, 156, 31,
			0xb5026f5aa96619e9, 29,
			0x5555555555555555, 17,
			0x71d67fffeda60000, 37,
			0xfff7eee000000000, 43, 6364136223846793005>;

	public:
		static Generator* GetGenerator();
		static void SetSeed(Generator::result_type seed);
		
		static u64 U64();
		static f32 Float();		
		static bool Bool();
		static bool Bool(f32 probability);
		static f32 Radian();
		static f32 Degree();
		static f32 Range(f32 lower, f32 upper);
		static i32 Range(i32 lower, i32 upper);
		static u32 Range(u32 lower, u32 upper);

	public:
		static constexpr u64 MaxValue = std::numeric_limits<u64>::max() / 2 - 1;
		static constexpr u64 MinValue = std::numeric_limits<u64>::min() / 2 - 1;

	private:
		thread_local static Generator s_mersenne64;
	};
}
