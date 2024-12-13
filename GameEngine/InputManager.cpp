#include "InputManager.h"

//싱글톤 인스턴트 초기화
InputManager* InputManager::mpInstance = nullptr;
HWND InputManager::mHwnd = 0;
InputManager::InputManager()
{
	Initialize();
}

InputManager::~InputManager()
{

}

InputManager* InputManager::getInstance()
{
	if (mpInstance == nullptr)				// 인스턴스가 아직 생성되지 않았다면
	{
		mpInstance = new InputManager();		// 인스턴스 생성
		mpInstance->Initialize();
	}
	return mpInstance;						// 생성된 인스턴스 반환
}

void InputManager::destroyInstance()
{
	if (mpInstance != nullptr)
	{
		delete mpInstance;
		mpInstance = nullptr;
	}
}

void InputManager::SetHwnd(HWND _hwnd)
{
	mHwnd = _hwnd;
}

void InputManager::Initialize()
{
	mkeyVec =
	{
		VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
		'Q','W','E','R','T','Y','U','I',
		'O','P','A','S','D','F','G','Z',
		'X','C','V','B', 'N', 'M',
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		VK_MENU, VK_CONTROL, VK_SPACE, VK_RETURN, VK_ESCAPE,
		VK_LSHIFT, VK_RSHIFT, VK_LBUTTON, VK_RBUTTON, VK_TAB
	};

	for (int i = 0; i < static_cast<int>(KEY::LAST); ++i)
		mkeyInfo.push_back(KeyInfo{ KEY_STATE::NONE,false });
}

void InputManager::Update()
{
	if (nullptr != GetFocus())
	{
		MousePos();
		InFocus();
	}
	else
	{
		OutFocus();
	}
}

void InputManager::Finalize()
{

}

///창이 포커스 되어 있을 떄
void InputManager::InFocus()
{
	for (int i = 0; i < static_cast<int>(KEY::LAST); ++i)
	{
		if (GetAsyncKeyState(mkeyVec[i]) & 0x8000)
		{
			if (mkeyInfo[i].isPushed)
			{
				mkeyInfo[i].state = KEY_STATE::HOLD;

			}
			else
			{
				mkeyInfo[i].state = KEY_STATE::TAP;
				//디버그 모드용
				//std::cout << std::endl;
				//std::cout << static_cast<char>(m_key[i]) << std::endl;
				//std::cout << "마우스 위치 : " << m_currentMousePos.x << " / " << m_currentMousePos.y << std::endl;
			}

			mkeyInfo[i].isPushed = true;
		}
		else
		{
			if (mkeyInfo[i].isPushed)
			{
				mkeyInfo[i].state = KEY_STATE::AWAY;
			}
			else
			{
				mkeyInfo[i].state = KEY_STATE::NONE;
			}


			mkeyInfo[i].isPushed = false;
		}
	}
}

void InputManager::OutFocus()
{
	for (int i = 0; i < static_cast<int>(KEY::LAST); ++i)
	{
		mkeyInfo[i].isPushed = false;
		if (KEY_STATE::TAP == mkeyInfo[i].state ||
			KEY_STATE::HOLD == mkeyInfo[i].state)
		{
			mkeyInfo[i].state = KEY_STATE::AWAY;
		}
		else if (KEY_STATE::AWAY == mkeyInfo[i].state)
		{
			mkeyInfo[i].state = KEY_STATE::NONE;
		}
	}
}

//마우스의 현재포지션을 업데이트한다.
void InputManager::MousePos()
{
	mPrevMousePos = mCurrentMousePos;					   // 이전 프레임의 마우스의 위치를 업데이트한다.
	POINT temp;
	GetCursorPos(&temp);
	ScreenToClient(mHwnd, &temp);
	mCurrentMousePos.x = static_cast<float>(temp.x);							   // 현재의 마우스의 위치를 업데이트한다.
	mCurrentMousePos.y = static_cast<float>(temp.y);							   // 현재의 마우스의 위치를 업데이트한다.

}

//마우스의 움직인 정도를 계산한다. (현재 프레임의 마우스 위치 - 이전 프레임의 마우스 위치)
DirectX::SimpleMath::Vector2 InputManager::GetDeltaMousePos()
{
	mDeltaMousePos.x = static_cast<float>(mCurrentMousePos.x - mPrevMousePos.x);
	mDeltaMousePos.y = static_cast<float>(mCurrentMousePos.y - mPrevMousePos.y);

	return mDeltaMousePos;
}

KEY_STATE InputManager::GetKeyState(KEY _Key)
{
	return mkeyInfo[static_cast<int>(_Key)].state;
}

Vector2 InputManager::GetMousePos()
{
	return mCurrentMousePos;
}