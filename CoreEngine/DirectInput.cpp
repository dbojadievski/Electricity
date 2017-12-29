#include "InputBase.h"
#include "KeyEvents.h"
#include "MouseEvents.h"

extern HINSTANCE g_hInstance;
extern HWND g_Window;


InputBase::InputBase(IEventManager * pEventManager)
{
	assert(pEventManager);
	this->m_pEventManager = pEventManager;
}

void
InputBase::Init()
{
	this->m_pMouse = NULL;
	this->m_pKeyboard = NULL;

	this->m_pDirectInput = NULL;

	this->m_RotationX = 0;
	this->m_RotationZ = 0;

	this->m_RotationXM = XMMatrixIdentity();
	this->m_RotationZM = XMMatrixIdentity();

	HRESULT errCode = S_OK;

	errCode = DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&this->m_pDirectInput, NULL);
	assert(errCode == S_OK);
	if (errCode != S_OK)
	{
		this->m_pDirectInput->Release();
		this->m_pDirectInput = NULL;
		goto end;
	}

	errCode = this->m_pDirectInput->CreateDevice(GUID_SysKeyboard, &this->m_pKeyboard, NULL);
	assert(errCode == S_OK);
	if (errCode != S_OK)
	{
		this->m_pKeyboard->Unacquire();
		this->m_pKeyboard = NULL;
		goto end;
	}

	errCode = this->m_pDirectInput->CreateDevice(GUID_SysMouse, &this->m_pMouse, NULL);
	assert(errCode == S_OK);
	if (errCode != S_OK)
	{
		this->m_pMouse->Unacquire();
		this->m_pMouse = NULL;
		goto end;
	}

	errCode = this->m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	this->m_pKeyboard->SetCooperativeLevel(g_Window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(g_Window, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

end:
	return;
}

void
InputBase::ShutDown()
{
	if (this->m_pMouse)
	{
		this->m_pMouse->Unacquire();
		this->m_pMouse = NULL;
	}

	if (this->m_pKeyboard)
	{
		this->m_pKeyboard->Unacquire();
		this->m_pKeyboard = NULL;
	}

	if (this->m_pDirectInput)
	{
		this->m_pDirectInput->Release();
		this->m_pDirectInput = NULL;
	}

	this->m_RotationX = 0;
	this->m_RotationZ = 0;

	this->m_RotationXM = XMMatrixIdentity();
	this->m_RotationZM = XMMatrixIdentity();
}

void
InputBase::Update(CORE_DOUBLE dT)
{
	UpdateMouseState();
	UpdateKeyboardState();
}

void 
InputBase::UpdateMouseState()
{
	DIMOUSESTATE currMouseState;

	this->m_pMouse->Acquire();
	this->m_pMouse->GetDeviceState(sizeof(currMouseState), (LPVOID)&currMouseState);

	size_t numButtonsOnMouse = ARRAYSIZE(currMouseState.rgbButtons);
	for (size_t currMouseButtonIdx = 0; currMouseButtonIdx < numButtonsOnMouse; currMouseButtonIdx++)
	{
		if (currMouseState.rgbButtons[currMouseButtonIdx] != this->m_PrevMouseState.rgbButtons[currMouseButtonIdx] && currMouseState.rgbButtons[currMouseButtonIdx])
		{
			MouseButtonPressedEventData * pEventData = new MouseButtonPressedEventData();
			pEventData->m_Button = (MouseButton)currMouseButtonIdx;
			this->m_pEventManager->VQueueEvent(pEventData);
		}
		else if (currMouseState.rgbButtons[currMouseButtonIdx] != this->m_PrevMouseState.rgbButtons[currMouseButtonIdx] && !currMouseState.rgbButtons[currMouseButtonIdx])
		{
			/* LMB up / released. */
			MouseButtonReleasedEventData * pEventData = new MouseButtonReleasedEventData();
			pEventData->m_Button = (MouseButton)currMouseButtonIdx;
			this->m_pEventManager->VQueueEvent(pEventData);
		}
		else if (currMouseState.rgbButtons[currMouseButtonIdx] == this->m_PrevMouseState.rgbButtons[currMouseButtonIdx] && currMouseState.rgbButtons[currMouseButtonIdx])
		{
			/* Mouse down. */
			MouseButtonDownEventData * pEventData = new MouseButtonDownEventData();
			pEventData->m_Button = (MouseButton)currMouseButtonIdx;
			this->m_pEventManager->VQueueEvent(pEventData);
		}
	}

	this->m_PrevMouseState = currMouseState;
}

void 
InputBase::UpdateKeyboardState()
{
	BYTE keyboardState[255];

	this->m_pKeyboard->Acquire();
	this->m_pKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	for (BYTE currKeyIdx = 0; currKeyIdx < sizeof(keyboardState); currKeyIdx++)
	{
		if (keyboardState[currKeyIdx] && !this->m_PrevKeyboardState[currKeyIdx])
		{
			KeyPressEventData * pData = new KeyPressEventData();
			pData->m_KeyCode = currKeyIdx;
			this->m_pEventManager->VQueueEvent(pData);
		}
		else if (!keyboardState[currKeyIdx] && this->m_PrevKeyboardState[currKeyIdx])
		{
			KeyUpEventData * pData = new KeyUpEventData();
			pData->m_KeyCode = currKeyIdx;
			this->m_pEventManager->VQueueEvent(pData);
		}
		else if (keyboardState[currKeyIdx])
		{
			KeyDownEventData * pData = new KeyDownEventData();
			pData->m_KeyCode = currKeyIdx;
			this->m_pEventManager->VQueueEvent(pData);
		}

		this->m_PrevKeyboardState[currKeyIdx] = keyboardState[currKeyIdx];
	}
}


void
CreateDirectInput(HINSTANCE hInstance)
{
	assert(hInstance);
}