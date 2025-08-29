#pragma once

#include "core.h"

namespace sor
{
	// this would contain letter key + numbers, for modifiers (Shift, Ctrl, Alt) there would be another enum
	enum class EKeyCodeFlags : u32
	{
		S		= BIT_FLAG(0),
		F		= BIT_FLAG(1)
	};
	DEFINE_ENUM_OPS(EKeyCodeFlags);

	enum class EMouseInputFlags : u8
	{
		LEFT_BUTTON		= BIT_FLAG(0),
		RIGHT_BUTTON	= BIT_FLAG(1)
	};
	DEFINE_ENUM_OPS(EMouseInputFlags);

	enum class EInputType : u8
	{
		DOWN,		// sent first frame key is pressed and also continuously as long as the key is pressed 
		PRESSED,	// sent only the first frame key is pressed (not implemented)
		UP,			
		RELEASED,	
		COUNT
	};
	DEFINE_ENUM_OPS(EInputType);

	// determines input registered each frame
	struct DeviceInput
	{
		EKeyCodeFlags keyCodeFlagsForInputType[(int)EInputType::COUNT];
		EMouseInputFlags mouseFlagsForInputType[(int)EInputType::COUNT];
		Vec2i mouseDelta;

		void Clear()
		{
			memset(keyCodeFlagsForInputType, 0, sizeof(EInputType) * (int) EInputType::COUNT);
			memset(mouseFlagsForInputType, 0, sizeof(EInputType) * (int) EInputType::COUNT);
		}

		void WriteKeyInput(EKeyCodeFlags keyInput, EInputType inputType)
		{
			keyCodeFlagsForInputType[(int)inputType] |= keyInput;
		}

		void WriteMouseInput(EMouseInputFlags mouseInput, EInputType inputType)
		{
			mouseFlagsForInputType[(int)mouseInput] |= mouseInput;
		}

		bool ReadKeyInput(EKeyCodeFlags keyInput, EInputType inputType)
		{
			return (bool) (keyCodeFlagsForInputType[(int)inputType] & keyInput);
		}

		bool ReadMouseInput(EMouseInputFlags mouseInput, EInputType inputType)
		{
			return (bool) (mouseFlagsForInputType[(int)inputType] & mouseInput);
		}
	};

	inline DeviceInput g_DeviceInput {};
}