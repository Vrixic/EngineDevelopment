#pragma once

#include "Vrixic/GenericDefines.h"
#include <bitset>

typedef unsigned char uint8;

/* Keys */

// CHARACTERS
#define KEY_A 0x41
#define KEY_B 0x42
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_S 0x53
#define KEY_W 0x57

#define KEY_0 0x30
#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34

#define KEY_ESCAPE 0x1B

// ARROWS
#define	KEY_ARROW_LEFT		0x25
#define	KEY_ARROW_UP		0x26
#define	KEY_ARROW_RIGHT		0x27
#define	KEY_ARROW_DOWN		0x28

#define MOUSE_LEFT_DOWN		0x100
#define MOUSE_RIGHT_DOWN	0x101

struct MousePoint
{
	int32 X, Y;

	MousePoint() : X(0), Y(0) { }
	MousePoint(int32 x, int32 y) : X(x), Y(y) { }
};


/* For simplicity a singleton that keeps all key states */
class InputManager
{
public:
	static InputManager& GetInstance()
	{
		static InputManager Instance;
		return Instance;

	}

	InputManager() { }
	InputManager(InputManager& other) = delete;
	void operator=(const InputManager&) = delete;
private:
	std::bitset<258> KeyStates;


	MousePoint CurrentMousePos;
	MousePoint LastMousePos;
	MousePoint DeltaMousePos;

public:

	void SetMousePosition(MousePoint p)
	{
		LastMousePos = CurrentMousePos;
		CurrentMousePos = p;

		DeltaMousePos = { LastMousePos.X - CurrentMousePos.X, LastMousePos.Y - CurrentMousePos.Y };
	}

	void SetKeyState(uint8 key)
	{
		//std::cout << "Key pressed: " << key << "\n";
		KeyStates[key] = 1;
	}

	void UnsetKeyState(uint8 key)
	{
		KeyStates[key] = 0;
	}

	void ResetMouseDelta()
	{
		DeltaMousePos.X = 0;
		DeltaMousePos.Y = 0;
	}

	MousePoint GetCurrentMousePosition()
	{
		return CurrentMousePos;
	}

	MousePoint GetMouseDelta()
	{
		return DeltaMousePos;
	}

	bool GetKeyState(uint8 key)
	{
		return KeyStates[key];
	}

	bool GetMouseState(uint8 mouse)
	{
		return KeyStates[mouse];
	}
};
