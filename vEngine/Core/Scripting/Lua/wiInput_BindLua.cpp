#include "Core/Scripting/Lua/wiInput_BindLua.h"
#include "Core/Scripting/Lua/wiMath_BindLua.h"

namespace wi::lua
{

	const char Input_BindLua::className[] = "Input";

	Luna<Input_BindLua>::FunctionType Input_BindLua::methods[] = {
		lunamethod(Input_BindLua, Down),
		lunamethod(Input_BindLua, Press),
		lunamethod(Input_BindLua, Hold),
		lunamethod(Input_BindLua, GetPointer),
		lunamethod(Input_BindLua, SetPointer),
		lunamethod(Input_BindLua, GetPointerDelta),
		lunamethod(Input_BindLua, HidePointer),
		lunamethod(Input_BindLua, GetAnalog),
		lunamethod(Input_BindLua, GetTouches),
		lunamethod(Input_BindLua, SetControllerFeedback),
		{ NULL, NULL }
	};
	Luna<Input_BindLua>::PropertyType Input_BindLua::properties[] = {
		{ NULL, NULL }
	};

	int Input_BindLua::Down(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			wi::input::BUTTON button = (wi::input::BUTTON)wi::lua::SGetInt(L, 1);
			int playerindex = 0;
			if (argc > 1)
			{
				playerindex = wi::lua::SGetInt(L, 2);
			}
			wi::lua::SSetBool(L, wi::input::Down(button, playerindex));
			return 1;
		}
		else
			wi::lua::SError(L, "Down(int code, opt int playerindex = 0) not enough arguments!");
		return 0;
	}
	int Input_BindLua::Press(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			wi::input::BUTTON code = (wi::input::BUTTON)wi::lua::SGetInt(L, 1);
			int playerindex = 0;
			if (argc > 1)
			{
				playerindex = wi::lua::SGetInt(L, 2);
			}
			wi::lua::SSetBool(L, wi::input::Press(code, playerindex));
			return 1;
		}
		else
			wi::lua::SError(L, "Press(int code, opt int playerindex = 0) not enough arguments!");
		return 0;
	}
	int Input_BindLua::Hold(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			wi::input::BUTTON button = (wi::input::BUTTON)wi::lua::SGetInt(L, 1);
			uint32_t duration = 30;
			if (argc > 1)
			{
				duration = wi::lua::SGetInt(L, 2);
			}
			bool continuous = false;
			if (argc > 2)
			{
				continuous = wi::lua::SGetBool(L, 3);
			}
			int playerindex = 0;
			if (argc > 3)
			{
				playerindex = wi::lua::SGetInt(L, 4);
			}
			wi::lua::SSetBool(L, wi::input::Hold(button, duration, continuous, playerindex));
			return 1;
		}
		else
			wi::lua::SError(L, "Hold(int code, opt int duration = 30, opt boolean continuous = false, opt int playerindex = 0) not enough arguments!");
		return 0;
	}
	int Input_BindLua::GetPointer(lua_State* L)
	{
		XMFLOAT4 P = wi::input::GetPointer();
		Luna<Vector_BindLua>::push(L, new Vector_BindLua(XMLoadFloat4(&P)));
		return 1;
	}
	int Input_BindLua::SetPointer(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				wi::input::SetPointer(*vec);
			}
			else
				wi::lua::SError(L, "SetPointer(Vector props) argument is not a Vector!");
		}
		else
			wi::lua::SError(L, "SetPointer(Vector props) not enough arguments!");
		return 0;
	}
	int Input_BindLua::GetPointerDelta(lua_State* L)
	{
		Luna<Vector_BindLua>::push(L, new Vector_BindLua(XMLoadFloat2(&wi::input::GetMouseState().delta_position)));
		return 1;
	}
	int Input_BindLua::HidePointer(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			wi::input::HidePointer(wi::lua::SGetBool(L, 1));
		}
		else
			wi::lua::SError(L, "HidePointer(bool value) not enough arguments!");
		return 0;
	}
	int Input_BindLua::GetAnalog(lua_State* L)
	{
		XMFLOAT4 result = XMFLOAT4(0, 0, 0, 0);

		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			wi::input::GAMEPAD_ANALOG type = (wi::input::GAMEPAD_ANALOG)wi::lua::SGetInt(L, 1);
			int playerindex = 0;
			if (argc > 1)
			{
				playerindex = wi::lua::SGetInt(L, 2);
			}
			result = wi::input::GetAnalog(type, playerindex);
		}
		else
			wi::lua::SError(L, "GetAnalog(int type, opt int playerindex = 0) not enough arguments!");

		Luna<Vector_BindLua>::push(L, new Vector_BindLua(XMLoadFloat4(&result)));
		return 1;
	}
	int Input_BindLua::GetTouches(lua_State* L)
	{
		auto& touches = wi::input::GetTouches();
		for (auto& touch : touches)
		{
			Luna<Touch_BindLua>::push(L, new Touch_BindLua(touch));
		}
		return (int)touches.size();
	}
	int Input_BindLua::SetControllerFeedback(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			ControllerFeedback_BindLua* fb = Luna<ControllerFeedback_BindLua>::lightcheck(L, 1); // Ferdinand Braun
			if (fb != nullptr)
			{
				int playerindex = 0;
				if (argc > 1)
				{
					playerindex = wi::lua::SGetInt(L, 2);
				}
				wi::input::SetControllerFeedback(fb->feedback, playerindex);
			}
			else
			{
				wi::lua::SError(L, "SetControllerFeedback(ControllerFeedback feedback, opt int playerindex = 0) first argument is not a ControllerFeedback!");
			}
		}
		else
			wi::lua::SError(L, "SetControllerFeedback(ControllerFeedback feedback, opt int playerindex = 0) not enough arguments!");
		return 0;
	}

	void Input_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Input_BindLua>::Register(wi::lua::GetLuaState(), "vEngine");
			wi::lua::RunText("input = Input()");

			wi::lua::RunText("MOUSE_BUTTON_LEFT			= 1");
			wi::lua::RunText("MOUSE_BUTTON_RIGHT			= 2");
			wi::lua::RunText("MOUSE_BUTTON_MIDDLE		= 3");

			wi::lua::RunText("KEYBOARD_BUTTON_UP			= 4");
			wi::lua::RunText("KEYBOARD_BUTTON_DOWN		= 5");
			wi::lua::RunText("KEYBOARD_BUTTON_LEFT		= 6");
			wi::lua::RunText("KEYBOARD_BUTTON_RIGHT		= 7");
			wi::lua::RunText("KEYBOARD_BUTTON_SPACE		= 8");
			wi::lua::RunText("KEYBOARD_BUTTON_RSHIFT		= 9");
			wi::lua::RunText("KEYBOARD_BUTTON_LSHIFT		= 10");
			wi::lua::RunText("KEYBOARD_BUTTON_F1			= 11");
			wi::lua::RunText("KEYBOARD_BUTTON_F2			= 12");
			wi::lua::RunText("KEYBOARD_BUTTON_F3			= 13");
			wi::lua::RunText("KEYBOARD_BUTTON_F4			= 14");
			wi::lua::RunText("KEYBOARD_BUTTON_F5			= 15");
			wi::lua::RunText("KEYBOARD_BUTTON_F6			= 16");
			wi::lua::RunText("KEYBOARD_BUTTON_F7			= 17");
			wi::lua::RunText("KEYBOARD_BUTTON_F8			= 18");
			wi::lua::RunText("KEYBOARD_BUTTON_F9			= 19");
			wi::lua::RunText("KEYBOARD_BUTTON_F10		= 20");
			wi::lua::RunText("KEYBOARD_BUTTON_F11		= 21");
			wi::lua::RunText("KEYBOARD_BUTTON_F12		= 22");
			wi::lua::RunText("KEYBOARD_BUTTON_ENTER		= 23");
			wi::lua::RunText("KEYBOARD_BUTTON_ESCAPE		= 24");
			wi::lua::RunText("KEYBOARD_BUTTON_HOME		= 25");
			wi::lua::RunText("KEYBOARD_BUTTON_RCONTROL	= 26");
			wi::lua::RunText("KEYBOARD_BUTTON_LCONTROL	= 27");
			wi::lua::RunText("KEYBOARD_BUTTON_DELETE		= 28");
			wi::lua::RunText("KEYBOARD_BUTTON_BACK		= 29");
			wi::lua::RunText("KEYBOARD_BUTTON_PAGEDOWN	= 30");
			wi::lua::RunText("KEYBOARD_BUTTON_PAGEUP		= 31");

			wi::lua::RunText("GAMEPAD_BUTTON_UP			= 257");
			wi::lua::RunText("GAMEPAD_BUTTON_LEFT		= 258");
			wi::lua::RunText("GAMEPAD_BUTTON_DOWN		= 259");
			wi::lua::RunText("GAMEPAD_BUTTON_RIGHT		= 260");
			wi::lua::RunText("GAMEPAD_BUTTON_1			= 261");
			wi::lua::RunText("GAMEPAD_BUTTON_2			= 262");
			wi::lua::RunText("GAMEPAD_BUTTON_3			= 263");
			wi::lua::RunText("GAMEPAD_BUTTON_4			= 264");
			wi::lua::RunText("GAMEPAD_BUTTON_5			= 265");
			wi::lua::RunText("GAMEPAD_BUTTON_6			= 266");
			wi::lua::RunText("GAMEPAD_BUTTON_7			= 267");
			wi::lua::RunText("GAMEPAD_BUTTON_8			= 268");
			wi::lua::RunText("GAMEPAD_BUTTON_9			= 269");
			wi::lua::RunText("GAMEPAD_BUTTON_10			= 270");
			wi::lua::RunText("GAMEPAD_BUTTON_11			= 271");
			wi::lua::RunText("GAMEPAD_BUTTON_12			= 272");
			wi::lua::RunText("GAMEPAD_BUTTON_13			= 273");
			wi::lua::RunText("GAMEPAD_BUTTON_14			= 274");

			//Analog
			wi::lua::RunText("GAMEPAD_ANALOG_THUMBSTICK_L	= 0");
			wi::lua::RunText("GAMEPAD_ANALOG_THUMBSTICK_R	= 1");
			wi::lua::RunText("GAMEPAD_ANALOG_TRIGGER_L		= 2");
			wi::lua::RunText("GAMEPAD_ANALOG_TRIGGER_R		= 3");

			//Touch
			wi::lua::RunText("TOUCHSTATE_PRESSED		= 0");
			wi::lua::RunText("TOUCHSTATE_RELEASED	= 1");
			wi::lua::RunText("TOUCHSTATE_MOVED		= 2");
		}

		Touch_BindLua::Bind();
		ControllerFeedback_BindLua::Bind();
	}







	const char Touch_BindLua::className[] = "Touch";

	Luna<Touch_BindLua>::FunctionType Touch_BindLua::methods[] = {
		lunamethod(Touch_BindLua, GetState),
		lunamethod(Touch_BindLua, GetPos),
		{ NULL, NULL }
	};
	Luna<Touch_BindLua>::PropertyType Touch_BindLua::properties[] = {
		{ NULL, NULL }
	};

	int Touch_BindLua::GetState(lua_State* L)
	{
		wi::lua::SSetInt(L, (int)touch.state);
		return 1;
	}
	int Touch_BindLua::GetPos(lua_State* L)
	{
		Luna<Vector_BindLua>::push(L, new Vector_BindLua(XMLoadFloat2(&touch.pos)));
		return 1;
	}

	void Touch_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Touch_BindLua>::Register(wi::lua::GetLuaState(), "vEngine");
		}
	}







	const char ControllerFeedback_BindLua::className[] = "ControllerFeedback";

	Luna<ControllerFeedback_BindLua>::FunctionType ControllerFeedback_BindLua::methods[] = {
		lunamethod(ControllerFeedback_BindLua, SetVibrationRight),
		lunamethod(ControllerFeedback_BindLua, SetVibrationLeft),
		lunamethod(ControllerFeedback_BindLua, SetLEDColor),
		{ NULL, NULL }
	};
	Luna<ControllerFeedback_BindLua>::PropertyType ControllerFeedback_BindLua::properties[] = {
		{ NULL, NULL }
	};

	int ControllerFeedback_BindLua::SetVibrationLeft(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			feedback.vibration_left = wi::lua::SGetFloat(L, 1);
		}
		else
		{
			wi::lua::SError(L, "SetVibrationLeft(float value) not enough arguments!");
		}
		return 0;
	}
	int ControllerFeedback_BindLua::SetVibrationRight(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			feedback.vibration_right = wi::lua::SGetFloat(L, 1);
		}
		else
		{
			wi::lua::SError(L, "SetVibrationRight(float value) not enough arguments!");
		}
		return 0;
	}
	int ControllerFeedback_BindLua::SetLEDColor(lua_State* L)
	{
		int argc = wi::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec != nullptr)
			{
				feedback.led_color = wi::Color::fromFloat4(*vec);
			}
			else
			{
				feedback.led_color.rgba = wi::lua::SGetInt(L, 1);
			}
		}
		else
		{
			wi::lua::SError(L, "SetLEDColor(int hexcolor) not enough arguments!");
		}
		return 0;
	}

	void ControllerFeedback_BindLua::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<ControllerFeedback_BindLua>::Register(wi::lua::GetLuaState(), "vEngine");
		}
	}

}
