#include "GameProcess.h"
#include "GameEngine.h"
//#include "../GameEngine/GameEngine.h"
#include "imgui.h"

#ifdef _DEBUG
#include<iostream>
#endif

GameProcess::GameProcess()
{
	mWindowPosX = 0;
	mWindowPosY = 0;
	mScreenWidth = 1920;
	mScreenHeight = 1080;
	mHWnd = {};
	mMsg = {};

	mpGameEngine = nullptr;
}

GameProcess::~GameProcess()
{

}

HRESULT GameProcess::Initialize(HINSTANCE hInstance)
{
	//�ü������ â Ŭ������ �����ϰ� ����Ѵ�. WNDCLASSEX�� â Ŭ������ �Ӽ��� �����ϴ� ����ü�̴�. 
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),												//����ü�� ũ�⸦ ��Ÿ����. 
		CS_CLASSDC,														//â Ŭ���� ��Ÿ���� �����Ѵ�.
		WndProc,														//â ���ν����� ����Ų��.  â ���ν����� â�� �޽����� ��� ó�������� �����ϴ� �Լ��̴�. 
		0,
		0,
		GetModuleHandle(NULL),											//�ν��Ͻ� �ڵ��� �����´�. NULL�� �� ���� �������� �ν��Ͻ��� �ڵ��� �����´�.
		NULL,
		NULL,
		NULL,
		NULL,
		L"MangoWaffle",													// lpszClassName, L-string, â Ŭ������ �̸��� �����Ѵ�. 
		NULL };

	RegisterClassEx(&wc);												//�� â�� �ý��ۿ� ����ϴ� �Լ��� ȣ��, �� â�� ȣ�� ������ �츮�� �� â Ŭ������ �����Ҽ� �ְ� �ȴ�. 


	///Window â�� �����ϴ� �κ�, CreateWindow�Լ��� �־��� �Ű������� ���� ���ο� â�� �����ϰ�, �� â�� �ڵ��� ��ȯ�Ѵ�.
	mHWnd = CreateWindow(
		wc.lpszClassName,												// �ռ� ����� âŬ������ �̸�. �̸��� ���� CreateWindow�Լ��� � â Ŭ������ ����Ͽ� â�� �������� �˼� �ִ�.
		L"MangoWaffle Graphics",										// ������ â�� ����. Ŭ������ �̸����� �ٸ���.
		WS_POPUP,											// â�� ��Ÿ���� �����Ѵ�. ���� â�� �ִ����� ���� �ѹ� ����
		mWindowPosX,													// ������ ���� ����� x ��ǥ	
		mWindowPosY,													// ������ ���� ����� y ��ǥ	
		mScreenWidth,													// ������ ���� ���� �ػ�	
		mScreenHeight,													// ������ ���� ���� �ػ�	
		NULL,
		NULL,
		wc.hInstance,													//�ռ� GetModuleHandle(NULL) �Լ��� ���� ������ �ν��Ͻ� �ڵ�
		NULL);

	if (!mHWnd)
	{
		std::cout << "m_hWnd - CreateWindow Error\n";
		return E_FAIL;
	}

	// ������ �׷����� �ػ󵵸� �����ϱ� ����
	RECT wr = { mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight };	// Rect����ü�� ����Ͽ� â�� ũ�⸦ �����Ѵ�. set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);							// AdjustWindowRect �Լ��� ȣ���Ͽ� â�� ũ�⸦ ����, adjust the size �Լ��� �־��� ��Ÿ��(WS_OVERLAPPEDWINDOW)�� ���� â�� ũ�⸦ �����Ѵ�.

	ShowWindow(mHWnd, SW_SHOWDEFAULT);											// ShowWindow �Լ��� ȣ���Ͽ� â�� ȭ�鿡 ǥ��, hwnd�� �ռ� CreateWindow �Լ��� ���� ������ â�� �ڵ�, SW_SHOWDEFAULT�� â�� �⺻ ���·� ǥ���ϵ��� ����
	UpdateWindow(mHWnd);														// UpdateWindow �Լ��� ȣ���Ͽ� â�� ��� ������Ʈ

	mpGameEngine = new GameEngine(mHWnd, mScreenWidth, mScreenHeight);
	mpGameEngine->Initialize();
	return S_OK;
}

void GameProcess::Loop()
{
	while (true)
	{
		if (PeekMessage(&mMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (mMsg.message == WM_QUIT) break;


			DispatchMessage(&mMsg);
		}

		//���ӿ����� ������ ����. 
		mpGameEngine->GameLoop();
	}

}

void GameProcess::Finalize()
{
	delete mpGameEngine;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//WndProc�Լ��� â�� ���� �ǵ�鸸 �ְ� �ް�.
LRESULT CALLBACK GameProcess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{

	case WM_SIZE:																//�������� ũ�Ⱑ ����� �� �߻��ϴ� �޼��� 
		// Reset and resize swapchain											// �� �޼����� ó���Ͽ� ����ü���� �缳���Ѵ�.
		return 0;
	case WM_SYSCOMMAND:															//�ý��� ����� �߻��� �� �߻��ϴ� �޽���
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_KEYDOWN:
		if (wParam == 27)
		{
			PostQuitMessage(0);
		}
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}
