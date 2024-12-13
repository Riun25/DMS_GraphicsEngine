#pragma once
#include <windows.h>

class GameEngine;

class GameProcess
{
public:
	GameProcess();
	~GameProcess();
private:
	// 윈도우의 좌상단 - 이것으로 창의 위치를 지정할수 있다.
	int mWindowPosX;
	int mWindowPosY;

	// 화면의 크기
	int mScreenWidth;
	int mScreenHeight;

	// 윈도 관련
	HWND mHWnd;
	MSG mMsg;

	//게임엔진
	GameEngine* mpGameEngine;
public:
	//게임 프로세스
	HRESULT Initialize(HINSTANCE hInstance);		//게임엔진 초기화
	void Loop();
	void Finalize();

	// 메시지 핸들러 (윈도 콜백)
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

