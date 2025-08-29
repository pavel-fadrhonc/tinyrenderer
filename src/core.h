#pragma once

#include "types.h"

namespace sor
{
	constexpr inline u32 BIT_FLAG(u8 bitShift)
	{
		return 1 << bitShift;
	}

#define DEFINE_ENUM_OPS(enumName) \
	constexpr inline enumName operator|(enumName flags1, enumName flags2)\
	{\
		return (enumName) ((int)flags1 | (int) flags2);\
	}\
\
	constexpr inline enumName operator&(enumName flags1, enumName flags2)\
	{\
		return (enumName) ((int)flags1 & (int) flags2);\
	}\
\
	constexpr inline enumName& operator|=(enumName& flags1, enumName flags2)\
	{\
		flags1 = flags1 | flags2;\
		return flags1;\
	}\
\
	constexpr inline enumName& operator&=(enumName& flags1, enumName flags2)\
	{\
		flags1 = flags1 & flags2;\
		return flags1;\
	} 
}