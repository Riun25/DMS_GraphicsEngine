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
	//모델의 컨스턴트 데이터 게임엔진에서 자료구조화 시켜 쓸것.
	CommonConstantBufferData mCConstantBuffer;
	//조명의 위치를 참고하는 commonConstant
	CommonConstantBufferData mLightCConstantBuffer;

	PsShadowConstantBufferData mPSShadowConstantData;

	float mOpacityValue[4] = { 1.0,1.0,1.0,1.0 };
public:
	//그래픽스 엔진, 물리엔진 등의 객체를 생성한다.
	void Initialize();

	//업데이트를 돈다
	void Update();

	//랜더 관련함수
	void Render();

	void GameLoop();

	void Finalize();

	// 클래스 내에서만 정돈을 위해 만드는 함수
private:
	void InitializeManager();
};