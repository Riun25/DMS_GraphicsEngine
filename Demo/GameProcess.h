#pragma once
#include <windows.h>

class GameEngine;

class GameProcess
{
public:
	GameProcess();
	~GameProcess();
private:
	// �������� �»�� - �̰����� â�� ��ġ�� �����Ҽ� �ִ�.
	int mWindowPosX;
	int mWindowPosY;

	// ȭ���� ũ��
	int mScreenWidth;
	int mScreenHeight;

	// ���� ����
	HWND mHWnd;
	MSG mMsg;

	//���ӿ���
	GameEngine* mpGameEngine;
public:
	//���� ���μ���
	HRESULT Initialize(HINSTANCE hInstance);		//���ӿ��� �ʱ�ȭ
	void Loop();
	void Finalize();

	// �޽��� �ڵ鷯 (���� �ݹ�)
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

