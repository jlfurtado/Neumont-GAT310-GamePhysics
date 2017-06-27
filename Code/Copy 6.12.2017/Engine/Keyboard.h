#ifndef KEYBOARD_H
#define KEYBOARD_H

// Justin Furtado
// 7/10/2016
// Keyboard.h
// Calculates pressed and released states for keys

#include "ExportHeader.h"

namespace Engine
{
	struct Key
	{
		int vKey{ 0 };
		bool lastDownState{ false };
		bool downState{ false };
	};

	struct KeyToggle
	{
		bool fireOnPress{ true };
		bool *pToggle{ nullptr };
		int vKey{ 0 };
	};

	class ENGINE_SHARED Keyboard
	{
	public:
		// public methods
		bool static KeyWasPressed(int vKey);
		bool static KeyWasReleased(int vKey);
		bool static KeyIsDown(int vKey);
		bool static KeyIsUp(int vKey);
		bool static KeysArePressed(int *vKeys, int numKeys);
		bool static KeysAreDown(int *vKeys, int numKeys);
		bool static KeysAreUp(int *vKeys, int numKeys);
		bool static AddToggle(int vKey, bool *pToggle, bool onFirePress = true);
		void static Update(float dt);

	private:
		// Methods
		static void UpdateKeys();
		static void UpdateToggles();

		// data
		static const int MAX_KEY = 256;
		static KeyToggle s_keyToggles[MAX_KEY];
		static Key s_keyStates[MAX_KEY];
		static int s_numToggles;
	};

}

#endif // ifndef KEYBOARD_H