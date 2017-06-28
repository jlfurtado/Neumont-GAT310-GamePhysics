#include "Keyboard.h"
#include "MyWindow.h"
#include "GameLogger.h"

// Justin Furtado
// 7/10/2016
// Keyboard.cpp
// Calculates pressed and released states for keys

namespace Engine
{
	KeyToggle Keyboard::s_keyToggles[MAX_KEY];
	Key Keyboard::s_keyStates[MAX_KEY];
	int Keyboard::s_numToggles{ 0 };

	bool Keyboard::KeyWasPressed(int vKey)
	{
		return s_keyStates[vKey].downState && !s_keyStates[vKey].lastDownState;
	}

	bool Keyboard::KeyWasReleased(int vKey)
	{
		return !s_keyStates[vKey].downState && s_keyStates[vKey].lastDownState;
	}

	bool Keyboard::KeyIsDown(int vKey)
	{
		return s_keyStates[vKey].downState;
	}

	bool Keyboard::KeyIsUp(int vKey)
	{
		return !s_keyStates[vKey].downState;
	}

	// returns true once when all keys specified enter the down state
	bool Keyboard::KeysArePressed(int * vKeys, int numKeys)
	{
		if (!vKeys || numKeys <= 0) { Engine::GameLogger::Log(Engine::MessageType::cError, "Bad input detected for KeysArePressed!\n"); return false; }
		bool found = false, total = false; 
		
		// iterate through each key, check if any key was pressed this frame and the other keys are down (makes order-irrelevant check - ie 1 held 2 held 3 pressed is the same as 1 pressed 2 held 3 held)
		for (int i = 0; i < numKeys && !found; ++i)
		{ 
			total = KeyWasPressed(*(vKeys + i));
			for (int j = 0; j < numKeys && total; ++j) { total &= KeyIsDown(*(vKeys + j)); }
			found |= total;
		}

		return found;
	}

	// returns true if every key specified is in the down state, false otherwise
	bool Keyboard::KeysAreDown(int * vKeys, int numKeys)
	{
		if (!vKeys || numKeys <= 0) { Engine::GameLogger::Log(Engine::MessageType::cError, "Bad input detected for KeysAreDown!\n"); return false; }
		bool found = false;
		for (int i = 0; i < numKeys; ++i) { found &= KeyIsDown(*(vKeys + i)); }
		return found;
	}

	// returns true if every key specified is in the up state, false otherwise
	bool Keyboard::KeysAreUp(int * vKeys, int numKeys)
	{
		if (!vKeys || numKeys <= 0) { Engine::GameLogger::Log(Engine::MessageType::cError, "Bad input detected for KeysAreUp!\n"); return false; }
		bool found = false;
		for (int i = 0; i < numKeys; ++i) { found &= KeyIsUp(*(vKeys + i)); }
		return found;
	}

	bool Keyboard::AddToggle(int vKey, bool * pToggle, bool onFirePress)
	{
		if (vKey < 0 || vKey > MAX_KEY) { Engine::GameLogger::Log(Engine::MessageType::Warning, "Invalid key [%d] was entered for AddToggle!\n", vKey); return false; }
		if (!pToggle) { Engine::GameLogger::Log(Engine::MessageType::Error, "Error in AddToggle! Can't toggle nullptr!\n"); return false; }

		s_keyToggles[s_numToggles++] = { onFirePress, pToggle, vKey };
		return true;
	}

	void Keyboard::Update(float /*dt*/)
	{
		UpdateKeys();
		UpdateToggles();
	}

	void Keyboard::UpdateKeys()
	{
		BYTE k[MAX_KEY]{ 0 };
		//GetKeyState(0);
		if (!GetKeyboardState(k)) { GameLogger::Log(MessageType::cError, "Failed to get key states!\n"); return;  }
		for (int vKey = 0; vKey < MAX_KEY; ++vKey)
		{
			s_keyStates[vKey].lastDownState = s_keyStates[vKey].downState;
			s_keyStates[vKey].downState = (k[vKey] >= 128); // todo investigate!!!
			//GameLogger::Log(MessageType::ConsoleOnlyNoMessageType, "%d : %d\n", vKey, (k[vKey]));
		}
	}

	void Keyboard::UpdateToggles()
	{
		// update toggles
		for (int i = 0; i < s_numToggles; ++i)
		{
			if (!s_keyToggles[i].pToggle) { continue; }

			if (s_keyToggles[i].fireOnPress ? KeyWasPressed(s_keyToggles[i].vKey) : KeyWasReleased(s_keyToggles[i].vKey))
			{
				*(s_keyToggles[i].pToggle) = !(*(s_keyToggles[i].pToggle));
			}
		}
	}
}