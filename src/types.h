#pragma once

namespace sor
{
	using u64 = uint_fast64_t;
	using u32 = uint32_t;
	using u16 = uint16_t;
	using u8 = uint8_t;

	using i32 = int32_t;
	using i16 = int16_t;
	using i8 = int8_t;

	template <typename T, int size>
	using RawArrVariableSize = T[size];

	template <typename T>
	using RawArr3 = RawArrVariableSize<T, 3>;

	template <typename T>
	using RawArr4 = RawArrVariableSize<T, 4>;

}