#pragma once
#include <windows.h>
#include "DearsGraphicsEngine.h"
#include "AStar.h"
#include "tempObject.h"
#include "EasingFunc.h"
#include "ObjectPool.h"


class TimeManager;
class InputManager;
class FileManager;
struct ModelBuffer;
struct VSConstantBufferData;
struct VSBoneConstantBufferData;
struct PSConstantBufferData;
class Camera;
class AStar;

class GameEngine
{
public:
	GameEngine(HWND _hWnd, const int _screenWidth, const int _screenHeight);
	~GameEngine();

private:
	HWND mHWnd;
	int mScreenWidth;
	int mScreenHeight;

	TimeManager* mpTimeManager;
	InputManager* mpInputManager;
	FileManager* mpFileManager;
	DearsGraphicsEngine* mpDearsGraphicsEngine;
	EasingFunc* mpEasingFunc;

	Camera* mpCommonCamera;
	Camera* mpLightCamera;

	tempObject* tempObject1;
	tempObject* tempObject2;
	tempObject* tempObject3;
	tempObject* tempObject4;
	tempObject* tempObject5;
	tempObject* tempObject6;
	tempObject* tempObject7;
	tempObject* tempObject8;
	tempObject* tempObject9;

	// AStar
	AStar* mpAStar;
	std::vector<Vector2> mRawPath;
	Vector3 mNextPos;
	size_t mPathIndex = 0;

	//UI tweening
	Vector2 mUIPoint;
	Vector2 mUIPoint2;
public:
	//���� ������Ʈ ������ ���ӿ������� �ڷᱸ��ȭ ���� ����.
	CommonConstantBufferData mCConstantBuffer;
	//������ ��ġ�� �����ϴ� commonConstant
	CommonConstantBufferData mLightCConstantBuffer;

	PsShadowConstantBufferData mPSShadowConstantData;

	float mOpacityValue[4] = { 1.0,1.0,1.0,1.0 };
public:
	//�׷��Ƚ� ����, �������� ���� ��ü�� �����Ѵ�.
	void Initialize();

	//������Ʈ�� ����
	void Update();

	//���� �����Լ�
	void Render();

	void GameLoop();

	void Finalize();

	// Ŭ���� �������� ������ ���� ����� �Լ�
private:
	void InitializeManager();
};