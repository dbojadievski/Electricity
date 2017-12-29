#pragma once
#include "IEngineSystem.h"
#include "IEventManager.h"
#include "Math.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>

class InputBase : public IEngineSystem
{
	IDirectInputDevice8 * m_pMouse;
	IDirectInputDevice8 * m_pKeyboard;

	DIMOUSESTATE m_PrevMouseState;
	BYTE m_PrevKeyboardState[255] = { 0 };

	IDirectInput8 * m_pDirectInput;

	IEventManager * m_pEventManager;

	CORE_REAL m_RotationX;
	CORE_REAL m_RotationZ;
	CORE_REAL m_ScaleX = 1.0f;
	CORE_REAL m_ScaleY = 1.0f;

	XMMATRIX m_RotationXM;
	XMMATRIX m_RotationZM;

	void UpdateMouseState();
	void UpdateKeyboardState();
public:
	InputBase(IEventManager * pEventManager);

	void Init();
	void Update(CORE_DOUBLE dT);
	void ShutDown();
};