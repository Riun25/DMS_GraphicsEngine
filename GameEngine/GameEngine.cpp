#include "GameEngine.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "FileManager.h"
#include "Camera.h"
#include <math.h>

Vector3 dir = { 1.f, -1.f, 1.f };

GameEngine::GameEngine(HWND _hWnd, const int _screenWidth, const int _screenHeight)
{
	mHWnd = _hWnd;
	mScreenWidth = _screenWidth;
	mScreenHeight = _screenHeight;
	mpTimeManager = nullptr;
	mpInputManager = nullptr;
	mpFileManager = new FileManager();
	mpDearsGraphicsEngine = new DearsGraphicsEngine(mHWnd, mScreenWidth, mScreenHeight);
	mpAStar = new AStar;
	mpEasingFunc = new EasingFunc;
}

GameEngine::~GameEngine()
{
	delete mpAStar;
	delete mpDearsGraphicsEngine;
	delete mpFileManager;
}

void GameEngine::Initialize()
{
	InitializeManager();
	mpDearsGraphicsEngine->Initialize();


	///모델 추가의 예시코드
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Character 01.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Armor 05.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Armor 01.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Hat 04.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Wand 01.fbx");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Axe 01.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Bow 01.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Scythe 02.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Shoulder 12.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Hair Female 04.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Hood 02.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Face Female 10.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Mask 01.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Cape 02.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Bow 03.fbx");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Vest 01.FBX");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Shoulder 12.fbx");
	//m_pDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Tree_02.fbx");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Rock_05.fbx");
	mpDearsGraphicsEngine->AddModel("../TestAsset/Test/", "Sung.fbx");
	//m_pDearsGraphicsEngine->AddModel("../TestAsset/", "box.fbx");

	///애니메이션 추가
	//m_pDearsGraphicsEngine->AddAnimation("../TestAsset/", "CatwalkIdle.fbx");
	//m_pDearsGraphicsEngine->AddAnimation("../TestAsset/", "CatwalkWalkForward.fbx");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Cast Spell 02.FBX");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Run Forward Bare Hands In Place.FBX");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Die.FBX");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Idle.FBX");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Mutant Run2.fbx");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Mutant Run.FBX");
	mpDearsGraphicsEngine->AddAnimation("../TestAsset/Test/", "Character@Slash Attack.FBX");

	///2D텍스쳐 추가
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "Red.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "Yellow.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "Blue.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "White.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "BillBoardTest.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "ss.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "Water.png");
	//m_pDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "pngegg.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "Hair-Orange.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "Weapon-Black.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "coco.jpg");
	mpDearsGraphicsEngine->AddDDSTexture("../TestAsset/Test/", "saintpeters.dds");
	mpDearsGraphicsEngine->AddDDSTexture("../TestAsset/Test/", "Atrium_diffuseIBL.dds");
	mpDearsGraphicsEngine->AddDDSTexture("../TestAsset/Test/", "Atrium_specularIBL.dds");

	mpDearsGraphicsEngine->Add2DTexture("../TestAsset/Test/", "startButton.png");
	mpDearsGraphicsEngine->Add2DTexture("../TestAsset/Test/", "pngegg.png");		//빌보드에 쓸 것은 2D Texture로 읽는다.
	//m_pDearsGraphicsEngine->Add2DTexture("../TestAsset/", "Paladin_diffuse.png");

	///폰트추가
	///폰트추가
// 	m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "B.ttf", 40.0f, true);
// 	m_pDearsGraphicsEngine->AddFont("../TestAsset/Test/", "L.ttf", 20.0f, true);
// 	//m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "L_100.ttf", 10.f, true);
//	m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "M.ttf", 100.f, true);
// 	m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "NotoSansKR-Regular(24).ttf", 24.f, true);
// 	m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "NotoSansKR-Regular(32).ttf", 32.f, true);
// 	m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "NotoSansKR-Regular(36).ttf", 36.f, true);
// 	m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "NotoSansKR-Regular(40).ttf", 40.f, true);
	mpDearsGraphicsEngine->AddFont("../../Resources/Font/", "NotoSansKR-Regular(48).ttf", 48.f, true);
	//m_pDearsGraphicsEngine->AddFont("../../Resources/Font/", "NotoSansKR-Regular(60).ttf", 60.f, true);


	mpDearsGraphicsEngine->FontSetFinish();	//폰트 세팅을 완료

	///3D텍스쳐의 추가
	//m_pDearsGraphicsEngine->Add3DTexture("../TestAsset/", "Ch03_1001_Diffuse.png");
	//m_pDearsGraphicsEngine->Add3DTexture("../TestAsset/", "Ch03_1001_Normal.png");
	mpDearsGraphicsEngine->Add3DTexture("../TestAsset/Test/", "M-Cos-16-Blue.png");

	//모델 삭제의 에시코드
	//m_pDearsGraphicsEngine->Erase_Animation("CatwalkIdle.fbx");
	//m_pDearsGraphicsEngine->Erase_VertexBuffer("Ch03");

	///그래픽스에서 사용할 카메라의 세팅
	mpCommonCamera = new Camera(mScreenWidth, mScreenHeight);
	mpCommonCamera->SetSpeed(20.0f); // 카메라 이동속도를 조정함
	mpCommonCamera->SetEyePos(Vector3(0.f,0.f, -30.f));
	//tempCamera->SetDirection(Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
	//tempCamera->SetAircraftAxes(0.0f, 0.0f, 0.0f);
	mpCommonCamera->ProjectionSettings(70.f, 0.1f, 10000.0f);
	mpCommonCamera->SetAircraftAxes(0,0,0);
	mpDearsGraphicsEngine->SetCamera(mpCommonCamera);

	///그림자맵에서 사용할 카메라의 세팅
	mpLightCamera = new Camera(mScreenWidth, mScreenHeight);
	mpLightCamera->SetSpeed(20.0f);
	mpLightCamera->SetEyePos(Vector3(-50.f, 50.0f, -50.f));		  //이후 light의 위치를 업데이트
	mpLightCamera->SetDirection(Vector3(1.0f, -1.0f, 1.0f));
	mpLightCamera->ProjectionSettings(70.f, 10.f, 1000.0f);
	mpLightCamera->SetAircraftAxes(0, 0, 0);

	tempObject1 = new tempObject(mpDearsGraphicsEngine);
	tempObject1->Initialize();
	tempObject1->CreateVSConstantBuffer();
	tempObject1->CreateVSBoneConstantBuffer();
	tempObject1->CreatePSConstantBuffer();
	///tempObject1->SetModelBuffer("Character 01", "M-Cos-16-Blue.png","Character 01.FBX", "Character@Slash Attack.FBX");
	tempObject1->SetVIBuffer("Character 01"); 
	tempObject1->SetDiffuseTexture("M-Cos-16-Blue.png");
	tempObject1->SetModelInfo("Character 01.FBX");
	tempObject1->SetAnimation("Character@Slash Attack.FBX");
	tempObject1->SetObjectScl(Matrix::CreateScale(0.1));

	tempObject2 = new tempObject(mpDearsGraphicsEngine);
	tempObject2->Initialize();
	tempObject2->CreateVSConstantBuffer();
	//tempObject2->CreateVSTargetBoneConstantBuffer();
	tempObject2->CreatePSConstantBuffer();
	///tempObject2->SetModelBuffer("Sung" , "ss.png", "Sung.fbx");
	tempObject2->SetVIBuffer("Axe 01");
	tempObject2->SetModelInfo("Axe 01.fbx");
	tempObject2->SetDiffuseTexture("ss.png");
	tempObject2->SetTargetBoneIndex(mpDearsGraphicsEngine->Get_TargetModelBoneIndex("Character 01", "RigRPalm"));
	tempObject2->GetObjectTargetBoneMatrix(tempObject1->mpVSBoneConstantBufferData);
	//tempObject2->SetObjectPos(m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRPalm", 0.1f));
	//tempObject2->SetObjectPos(Matrix::CreateTranslation({0,-20,0}));

	tempObject3 = new tempObject(mpDearsGraphicsEngine);
	tempObject3->Initialize();
	tempObject3->CreateVSConstantBuffer();
	tempObject3->CreateVSBoneConstantBuffer();
	tempObject3->CreatePSConstantBuffer();



	tempObject3->SetVIBuffer("BillBoardSquare");
	tempObject3->SetDiffuseTexture("pngegg.png");
	tempObject3->SetObjectScl(Matrix::CreateScale(5));
	tempObject3->SetObjectPos(Matrix::CreateTranslation({ 10,3,-10 }));

	tempObject4 = new tempObject(mpDearsGraphicsEngine);
	tempObject4->Initialize();
	tempObject4->CreateVSConstantBuffer();
	tempObject4->CreateVSTargetBoneConstantBuffer();
	tempObject4->CreatePSConstantBuffer();

	tempObject4->SetVIBuffer("Hat 04");
	tempObject4->SetModelInfo("Hat 04.FBX");
	tempObject4->SetDiffuseTexture("ss.png");
	tempObject4->SetTargetBoneIndex(mpDearsGraphicsEngine->Get_TargetModelBoneIndex("Character 01", "RigHead"));
	tempObject4->GetObjectTargetBoneMatrix(tempObject1->mpVSBoneConstantBufferData);
	tempObject4->SetObjectRot(Matrix::CreateRotationX(1.570));
	//tempObject4->SetObjectPos(m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigHead", 0.1f));
	tempObject4->SetObjectScl(Matrix::CreateScale(0.1));
	tempObject4->SetObjectPos(Matrix::CreateTranslation({ 0,-35, -10 }));

	//tempObject4->SetObjectPos(Matrix::CreateTranslation({ 10,0,0 }));



	tempObject5 = new tempObject(mpDearsGraphicsEngine);
	tempObject5->Initialize();
	tempObject5->CreateVSConstantBuffer();
	tempObject5->CreatePSConstantBuffer();
	///tempObject5->SetModelBuffer("MySphere", "ss.png");
	tempObject5->SetVIBuffer("MySquare");
	tempObject5->SetDiffuseTexture("White.png");
	tempObject5->SetObjectScl(Matrix::CreateScale(5, 5, 5));
	tempObject5->SetObjectPos(Matrix::CreateTranslation({ 0,0,0 }));

	tempObject5->CreateVSEdgeConstantBuffer();
	tempObject5->mVSEdgeConstantBufferData.EdgeScaleMatrix = Matrix::CreateScale(1.01); //외곽선의 스케일(두께), 1+a이어야 한다.
	tempObject5->mPSEdgeConstantBufferData.color = { 1,1,1 };		//외곽선의 색깔
	tempObject5->CreatePSEdgeConstantBuffer();


	tempObject6 = new tempObject(mpDearsGraphicsEngine);
	tempObject6->Initialize();
	tempObject6->CreateVSConstantBuffer();
	tempObject6->CreatePSConstantBuffer();
	tempObject6->CreateVSInstanceConstantBuffer();
	tempObject6->SetVIBuffer("MyBox");
	tempObject6->SetDiffuseTexture("Water.png");
	tempObject6->SetObjectScl(Matrix::CreateScale(5,5,5));
	tempObject6->SetObjectPos(Matrix::CreateTranslation({ 5,-20, 0 }));

	tempObject6->CreateVSWaterConstantBuffer();
	tempObject6->mVSWaterConstantBufferData.time = 0.0f;
	tempObject6->mVSWaterConstantBufferData.speed = 0.1f;


	tempObject7 = new tempObject(mpDearsGraphicsEngine);
	tempObject7->Initialize();
	tempObject7->CreateVSConstantBuffer();
	tempObject7->CreatePSConstantBuffer();
	//tempObject6->SetVIBuffer("MySquare");
	tempObject7->SetVIBuffer("MySquare");
	tempObject7->SetDiffuseTexture("ss.png");
	tempObject7->SetObjectScl(Matrix::CreateScale(200));
	tempObject7->SetObjectPos(Matrix::CreateTranslation({ 0,-25, 0 }));

	tempObject9 = new tempObject(mpDearsGraphicsEngine);
	tempObject9->Initialize();
	tempObject9->CreateVSConstantBuffer();
	tempObject9->CreatePSConstantBuffer();
	//tempObject9->SetModelBuffer("CubeMap", "Atrium_specularIBL.dds" );
	tempObject9->SetVIBuffer("CubeMap");
	tempObject9->SetObjectScl(Matrix::CreateScale(1));
	tempObject9->SetObjectPos(Matrix::CreateTranslation({ 0,0,0 }));


	///라이트의 세팅----------------------------
	mpDearsGraphicsEngine->LightInitialize(&mCConstantBuffer, 3);
	Vector3 dir = { 1.f, -1.f, 1.f };
	dir.Normalize();
	
	//auto e = lightCamera->mViewDir;

	//lightCamera->SetDirection(dir, {0,1,0});
	mpDearsGraphicsEngine->SetDirLight(&mCConstantBuffer, 0,1.f, dir);
	mCConstantBuffer.light[0].position = mpLightCamera->GetmViewPos();
	//m_pDearsGraphicsEngine->SetPointLight(&tempCCConstantBuffer, 1, 5.0f, 1.0f, 10.0f, Vector3(0.0f, -3.0f, -4.0f), Vector3(0.0f, 1.0f, 0.0f));
	//m_pDearsGraphicsEngine->SetSpotLight(&tempCCConstantBuffer, 2, 20.0f, 10.f, 70.0f, Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 45.0f, -0.0f), 10.0f, Vector3(1.0f, 1.0f, 1.0f));
	mpLightCamera->SetDirection(dir);



	mpLightCamera->SetEyePos(mCConstantBuffer.light[0].position);

	mUIPoint = { 1720, 400 };
	mUIPoint = { 1720, 700 };

	mpDearsGraphicsEngine->Set_CubeMap("Atrium_diffuseIBL.dds", "Atrium_specularIBL.dds");

}

void GameEngine::Update()
{
	mpTimeManager->Tick(); // Tick이 있어야 DeltaTime이 업데이트 된다.
	mpTimeManager->DeltaTime();

	mpInputManager->Update();
	mpCommonCamera->OnMouseMove(static_cast<int>(mpInputManager->GetMousePos().x), static_cast<int>(mpInputManager->GetMousePos().y));
	mpLightCamera->OnMouseMove(static_cast<int>(mpInputManager->GetMousePos().x), static_cast<int>(mpInputManager->GetMousePos().y));
	mCConstantBuffer.light[0].direction = mpLightCamera->mViewDir;

	if (mpInputManager->GetKeyState(KEY::T) == KEY_STATE::TAP)
	{
		if (mpCommonCamera->mIsFirstPersonMode)
		{
			mpCommonCamera->mIsFirstPersonMode = false;
		}
		else
		{
			mpCommonCamera->mIsFirstPersonMode = true;
		}
	}

	if (mpInputManager->GetKeyState(KEY::Y) == KEY_STATE::TAP)
	{
		if (mpLightCamera->mIsFirstPersonMode)
		{
			mpLightCamera->mIsFirstPersonMode = false;
		}
		else
		{
			mpLightCamera->mIsFirstPersonMode = true;
		}
	}
	if (mpInputManager->GetKeyState(KEY::UP) == KEY_STATE::HOLD)
	{
		mpCommonCamera->MoveForward(mpTimeManager->DeltaTime());
	}
	if (mpInputManager->GetKeyState(KEY::DOWN) == KEY_STATE::HOLD)
	{
		mpCommonCamera->MoveForward(-mpTimeManager->DeltaTime());
	}
	if (mpInputManager->GetKeyState(KEY::LEFT) == KEY_STATE::HOLD)
	{
		mpCommonCamera->MoveRight(-mpTimeManager->DeltaTime());
	}
	if (mpInputManager->GetKeyState(KEY::RIGHT) == KEY_STATE::HOLD)
	{
		mpCommonCamera->MoveRight(mpTimeManager->DeltaTime());
	}
	if (mpInputManager->GetKeyState(KEY::N) == KEY_STATE::HOLD)
	{
		mpCommonCamera->MoveUp(mpTimeManager->DeltaTime());
	}
	if (mpInputManager->GetKeyState(KEY::M) == KEY_STATE::HOLD)
	{
		mpCommonCamera->MoveUp(-mpTimeManager->DeltaTime());
	}
//애니메이션
	static Vector3 tempVec = { 0,0,0 };
		if (mpInputManager->GetKeyState(KEY::W) == KEY_STATE::HOLD)
		{
			tempVec.z += 0.1f;
		}
		if (mpInputManager->GetKeyState(KEY::A) == KEY_STATE::HOLD)
		{
			tempVec.x -= 0.1f;


		}
		if (mpInputManager->GetKeyState(KEY::S) == KEY_STATE::HOLD)
		{
			tempVec.z -= 0.1f;

		}
		if (mpInputManager->GetKeyState(KEY::D) == KEY_STATE::HOLD)
		{
			tempVec.x += 0.1f;

		}
		if (mpInputManager->GetKeyState(KEY::F) == KEY_STATE::HOLD)
		{
			//tempObject1->SetNextAnimation("Character@Cast Spell 02.FBX");
			CSParticleData temp;
			temp.position = tempVec;
			//temp.position = { 0, 20.f, 0 };
			temp.playTime = 0;

			temp.color = { 1,1,1 };
			temp.lifeTime = 2;

			temp.direction = { 0,0,0 };
			temp.velocity = 20.0f;

			temp.Rotation = { 0, 0, 0 };
			temp.opacity = 0.5;

			temp.Scale = { 1,1,1 };
			temp.deltaOpacity = -0.2f;

			temp.deltaRotation = { 0 ,0, 0 };
			temp.SclRandomFactor = 0;

			temp.deltaScale = { 0.f, 0.f, 0.f };

			temp.PosRandomFactor = { 0, 1, 0 };
			temp.RotRandomFactor = { 0, 0, 0 };
			temp.dirRandomFactor = { 0, 0, 0 };

			temp.deltaColor = { 0, 0, 0 };

			temp.gravity = 0;
			temp.colorRandomFactor = { 0, 0, 0 };

			mpDearsGraphicsEngine->mpParticleManager->AddParticle(1, temp);

			//
		}

///빛의 이동
// 	if (m_pInputManager->GetKeyState(KEY::A) == KEY_STATE::HOLD)
// 	{
// 		lightCamera->MoveRight(-m_pTimeManager->DeltaTime());
// 	}
// 	if (m_pInputManager->GetKeyState(KEY::D) == KEY_STATE::HOLD)
// 	{
// 		lightCamera->MoveRight(m_pTimeManager->DeltaTime());
// 	}
// 	if (m_pInputManager->GetKeyState(KEY::W) == KEY_STATE::HOLD)
// 	{
// 		lightCamera->MoveForward(m_pTimeManager->DeltaTime());
// 	}
// 	if (m_pInputManager->GetKeyState(KEY::S) == KEY_STATE::HOLD)
// 	{
// 		lightCamera->MoveForward(-m_pTimeManager->DeltaTime());
// 	}
// 	if (m_pInputManager->GetKeyState(KEY::Q) == KEY_STATE::HOLD)
// 	{
// 		lightCamera->MoveUp(-m_pTimeManager->DeltaTime());
// 	}
// 	if (m_pInputManager->GetKeyState(KEY::E) == KEY_STATE::HOLD)
// 	{
// 		lightCamera->MoveUp(m_pTimeManager->DeltaTime());
// 	}
// 	tempCCConstantBuffer.light[0].position = lightCamera->GetmViewPos();
// 장비교체 에시코드
// 	if (m_pInputManager->GetKeyState(KEY::_3) == KEY_STATE::TAP)
// 	{
// 		tempObject4->SetModelBuffer("Hood 02", "Hood 02", "Hood 02", "Weapon-Black.png");
// 		tempObject5->SetModelBuffer("Mask 01", "Mask 01", "Mask 01", "Weapon-Black.png");
// 		tempObject6->SetModelBuffer("Cape 02", "Cape 02", "Cape 02", "Weapon-Black.png");
// 	}

	mpDearsGraphicsEngine->LightUpdate(&mCConstantBuffer);
	mpDearsGraphicsEngine->LightUpdate(&mLightCConstantBuffer);
	static int tempTime = 0.0f;

	mpDearsGraphicsEngine->SetCamera(mpLightCamera);
	mCConstantBuffer.light[0].viewProj = (mpLightCamera->GetViewRow() * mpLightCamera->GetProjRow()).Transpose();
	//std::cout<< lightCamera->mViewDir.x << " / " << lightCamera->mViewDir.y << " / " << lightCamera->mViewDir.z << std::endl;
	//m_pDearsGraphicsEngine->UpdateCommonConstantBuffer(tempCCConstantBuffer);
	//m_pDearsGraphicsEngine->UpdateCommonConstantBuffer(tempLightCConstantBuffer);

	tempObject1->UpdateAnimationTime(mpTimeManager->DeltaTime() * 1.f);
	tempObject1->Update();

//	tempObject2->GetObjectTargetBoneMatrix(tempObject1->mpVSBoneConstantBufferData);
	//tempObject2->SetObjectPos(m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRPalm", 0.1f));
	tempObject2->SetObjectPos(mpDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", tempObject2->mTargetBoneIndex, 0.1f));

	tempObject2->Update();

	tempObject3->Update();


	tempObject4->GetObjectTargetBoneMatrix(tempObject1->mpVSBoneConstantBufferData);
	//tempObject4->SetObjectPos(m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigHead", 0.1f));
	tempObject4->Update();

	//tempObject5->UpdatePSConstantBufferData(tempCCConstantBuffer);
	tempObject5->Update();
 //	Matrix tempShadowView = m_pDearsGraphicsEngine->CreateShadowViewMatrix(tempCCConstantBuffer.light[0]);
 //	Matrix tempShadowProjection = m_pDearsGraphicsEngine->CreateShadowProjectionMatrix(tempCCConstantBuffer.light[0], 0, 1, 0, 0);
//	tempObject5->UpdateVSShadowConstantBufferData(tempShadowView, tempShadowProjection);
	
	///UV좌표를 증가시켜주기 위해
	tempObject6->mVSWaterConstantBufferData.time += mpTimeManager->DeltaTime();
	tempObject6->Update();

	tempObject7->Update();

	tempObject9->Update();

	static float temptime = 0;
	if (mpInputManager->GetKeyState(KEY::_4) == KEY_STATE::HOLD)
	{
		temptime += mpTimeManager->DeltaTime();
		mpEasingFunc->EasingFuncMap["easeInOutBack"](1600, 300, temptime, &mUIPoint.x);
		mpEasingFunc->EasingFuncMap["easeOutBounce"](1600, 300, temptime, &mUIPoint2.x);
		std::cout << "1 - " << mUIPoint.x << std::endl;
		std::cout << "2 - " << mUIPoint2.x << std::endl;
	}
	if (mpInputManager->GetKeyState(KEY::_5) == KEY_STATE::HOLD)
	{
		temptime = 0;
		mUIPoint.x = 1600;
		mUIPoint2.x = 1600;
	}

	if (mpInputManager->GetKeyState(KEY::_9) == KEY_STATE::HOLD)
	{
		for (int i = 0; i < 4; i++)
		{
			mOpacityValue[i] -= 0.01f;
		}
		mpDearsGraphicsEngine->SetOpacityFactor(mOpacityValue);	//투명도를 몇으로 할것인지..
	}

	if (mpInputManager->GetKeyState(KEY::_0) == KEY_STATE::HOLD)
	{
		for (int i = 0; i < 4; i++)
		{
			mOpacityValue[i] += 0.01f;
		}
		mpDearsGraphicsEngine->SetOpacityFactor(mOpacityValue);
	}
	mpDearsGraphicsEngine->Update();
}

void GameEngine::Render()
{
	if (mpInputManager->GetKeyState(KEY::Z) == KEY_STATE::TAP)
	{
		mpCommonCamera->SetPerspective();
	}
	if (mpInputManager->GetKeyState(KEY::X) == KEY_STATE::TAP)
	{
		mpCommonCamera->SetOrthgraphic(0.1f);
	}


	mpDearsGraphicsEngine->BeginRender();

	mpDearsGraphicsEngine->UIDrawImageStart();												//그림을 그리기전 호출
	mpDearsGraphicsEngine->UIDrawImage(Vector2(1720, 20), Vector2(200, 200), "coco.jpg", Vector4(0.2f, 0.2f, 0.2f, 0.8f));	//coco.jpg가 리소스 컨테이너 안에 이미 존재해야함
	mpDearsGraphicsEngine->UIDrawImage(Vector2(0, 0), Vector2(200, 200), "coco.jpg");	//coco.jpg가 리소스 컨테이너 안에 이미 존재해야함
	mpDearsGraphicsEngine->UIDrawImage(mUIPoint2, Vector2(200, 100), "startButton.png");	//coco.jpg가 리소스 컨테이너 안에 이미 존재해야함
	mpDearsGraphicsEngine->UIDrawImageFin();													//그림을 다 그리고 호출
	//m_pDearsGraphicsEngine->UIFreeRectFilled(Vector2(100, 100), Vector2(200, 100), Vector2(300, 500), Vector2(10, 500), Vector4(1.0f, 1.0f, 0.0f, 1.0f));		//사각형을 그린다.
	//m_pDearsGraphicsEngine->UIFreeRectwithBorder(Vector2(500, 100), Vector2(600, 100), Vector2(700, 500), Vector2(400, 500), Vector4(1.0f, 1.0f, 0.0f, 1.0f), 10.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));		//사각형을 그린다.
	//m_pDearsGraphicsEngine->UIFreeRect(Vector2(100, 100), Vector2(200, 100), Vector2(300, 500), Vector2(10, 500), Vector4(0.0f, 1.0f, 0.0f, 1.0f), 10.0f);		//사각형을 그린다.
	//m_pDearsGraphicsEngine->UIDrawLine(Vector2(110, 110), Vector2(160, 160), Vector4(1.0f, 0.0f, 0.0f, 1.0f));	//라인을 그린다.
	//m_pDearsGraphicsEngine->UIDrawCir(Vector2(150, 150), 20.0f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));				//원을 그린다.

	ImGuiIO* io = &ImGui::GetIO(); // 창을 초기화하거나 설정하는데 사용
	mpDearsGraphicsEngine->UIStartFontID("NotoSansKR-Regular(48).ttf");	//쓰려는 폰트 적용
	ImGui::SetWindowFontScale(0.5f);  // 폰트 크기를 1.5배로 키움
	mpDearsGraphicsEngine->UIDrawText(Vector2(150, 600), u8"1~4 장비\nQWER 애니메이션 보간", Vector4(1.0f));
	
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 100), u8"FPS : %.1f", Vector4(1.0f), mpTimeManager->FPS());
	
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 120), u8"마우스 x좌표: %.2f \n마우스 y좌표: %.2f", Vector4(1.0f), mpInputManager->GetMousePos().x, mpInputManager->GetMousePos().y);
	
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 170), u8"카메라 좌표: %.2f, %.2f, %.2f", Vector4(0.5f), mpCommonCamera->GetmViewPos().x, mpCommonCamera->GetmViewPos().y, mpCommonCamera->GetmViewPos().z);
	
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 190), u8"카메라 axis(RPY): %.1f, %.1f, %.1f", Vector4(1.0f), DirectX::XMConvertToDegrees(mpCommonCamera->mRoll), (mpCommonCamera->mPitch), (mpCommonCamera->mYaw));
	
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 210), u8"light 카메라 좌표: %.2f, %.2f, %.2f", Vector4(1.0f), mpLightCamera->GetmViewPos().x, mpLightCamera->GetmViewPos().y, mpLightCamera->GetmViewPos().z);
	Matrix e = mCConstantBuffer.light[0].viewProj;
	mpDearsGraphicsEngine->UIDrawImage(mUIPoint, Vector2(200, 100), "startButton.png");	//coco.jpg가 리소스 컨테이너 안에 이미 존재해야함

	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 230), u8"light viewProj: %.2f, %.2f, %.2f, %.2f", Vector4(1.0f), e._11, e._12, e._13, e._14);
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 250), u8"light viewProj: %.2f, %.2f, %.2f, %.2f", Vector4(1.0f), e._21, e._22, e._23, e._24);
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 270), u8"light viewProj: %.2f, %.2f, %.2f, %.2f", Vector4(1.0f), e._31, e._32, e._33, e._34);
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 290), u8"light viewProj: %.2f, %.2f, %.2f, %.2f", Vector4(1.0f), e._41, e._42, e._43, e._44);
	Vector3 e1 = mpCommonCamera->mViewDir;
	mpDearsGraphicsEngine->UIDrawTextWithNum(Vector2(0, 310), u8"mViewDir: %.2f, %.2f, %.2f", Vector4(1.0f),e1.x, e1.y, e1.z);
	mpDearsGraphicsEngine->UIFinFontID();



	//m_pDearsGraphicsEngine->UIStartFontID("B.ttf");	//쓰려는 폰트 적용
	//m_pDearsGraphicsEngine->UIDrawText(Vector2(100, 900), u8"국립박물관문화재단클래식B.ttf / 40.0f", Vector4(0.1f, 0.3f, 0.0f, 1.0f));
	//m_pDearsGraphicsEngine->UIFinFontID();			//다 썻을경우 호출

	//m_pDearsGraphicsEngine->mpRenderer->RenderDepthMap(tempObject1->GetModelBuffer());
	//m_pDearsGraphicsEngine->mpRenderer->RenderDepthMap(tempObject2->GetModelBuffer());
	///m_pDearsGraphicsEngine->SetCamera(lightCamera); 라이트 카메라 Set, CCC업데이트는 위에서 해준다.

	//혹시 메인 카메라에서 보고 싶을경우
 	mpDearsGraphicsEngine->SetCamera(mpLightCamera);
 	mpDearsGraphicsEngine->UpdateCommonConstantBuffer(mLightCConstantBuffer);
	mpDearsGraphicsEngine->RenderDepthMap(tempObject7->GetModelBuffer());
	mpDearsGraphicsEngine->RenderDepthMap(tempObject5->GetModelBuffer());
 	mpDearsGraphicsEngine->RenderDepthMap(tempObject6->GetModelBuffer());
 	mpDearsGraphicsEngine->RenderAniDepthMap(tempObject1->GetModelBuffer());
 	mpDearsGraphicsEngine->RenderDepthMap(tempObject2->GetModelBuffer());
 	mpDearsGraphicsEngine->RenderDepthMap(tempObject3->GetModelBuffer());
 	mpDearsGraphicsEngine->RenderEquipDepthMap(tempObject4->GetModelBuffer());

	mpDearsGraphicsEngine->SetCamera(mpCommonCamera);
	mpDearsGraphicsEngine->UpdateCommonConstantBuffer(mCConstantBuffer);

	//m_pDearsGraphicsEngine->Rend_InstancedModels(tempObject6->GetModelBuffer());
 	mpDearsGraphicsEngine->Rend_CubeMap(tempObject9->GetModelBuffer());
 	mpDearsGraphicsEngine->Rend_AnimateModel(tempObject1->GetModelBuffer());		//애니메이션 모델을 랜더한다.
 	//m_pDearsGraphicsEngine->Rend_Model(tempObject1->GetModelBuffer());		//애니메이션 모델을 랜더한다.
 	mpDearsGraphicsEngine->Rend_Model(tempObject2->GetModelBuffer());

	mpDearsGraphicsEngine->Rend_BillBoard(tempObject3->GetModelBuffer());		//애니메이션 모델을 랜더한다.
	//m_pDearsGraphicsEngine->Rend_EquipmentModel(tempObject4->GetModelBuffer());


 	//m_pDearsGraphicsEngine->Rend_Model(tempObject5->GetModelBuffer());
 	mpDearsGraphicsEngine->Rend_Water(tempObject6->GetModelBuffer());
 	mpDearsGraphicsEngine->Rend_Model(tempObject7->GetModelBuffer());


// 	m_pDearsGraphicsEngine->SetPipelineState(Dears::Graphics::PunchingPSO);
// 	tempObject5->SetObjectScl(Matrix::CreateScale(5, 15, 5));
// 	tempObject5->Update();
// 	m_pDearsGraphicsEngine->mpRenderer->Render(tempObject5->GetModelBuffer());
// 
// 	m_pDearsGraphicsEngine->SetPipelineState(Dears::Graphics::EdgePSO);
// 	tempObject5->SetObjectScl(Matrix::CreateScale(5.5, 15.5, 5.5));
// 	tempObject5->Update();
//	m_pDearsGraphicsEngine->mpRenderer->Render(tempObject5->GetModelBuffer());
	mpDearsGraphicsEngine->Rend_EdgeModel(tempObject5->GetModelBuffer());

	//m_pDearsGraphicsEngine->Rend_Shadow(tempObject5->GetModelBuffer());
	// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempObject2->mpModelBuffer->mpTargetModel->mMeshData->mAABB,
// 		Matrix::CreateScale({ 1,1,1 }),
// 		Matrix::CreateRotationX(0) * Matrix::CreateRotationX(0) * Matrix::CreateRotationX(0),
// 		m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRPalm", 0.1f));

	//m_pDearsGraphicsEngine->Rend_DebugBox(tempObject2->mpModelBuffer->mpTargetModel->mMeshData->mAABB, tempObject2->ObjectScl, tempObject2->ObjectRot);

	Vector3 _size = { 0.1f,0.1f,0.1f };
	//m_pDearsGraphicsEngine->Rend_DebugBox({30,10,50 }, { 0,0,0 }, {0,0,0 }, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRPalm", 0.1f));

 	Matrix tempScl = Matrix::CreateScale({ 3,1,5 });
 	Matrix tempRot = Matrix::CreateRotationX(0) * Matrix::CreateRotationX(0) * Matrix::CreateRotationX(0);
 	Matrix tempPos = Matrix::CreateTranslation({ 0,0,0 });
	//m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos);
	mpDearsGraphicsEngine->Rend_OpacitiyModel(tempObject5->GetModelBuffer());
	mpDearsGraphicsEngine->RendParticle();
	static bool a = true;
	if (mpInputManager->GetKeyState(KEY::_7) == KEY_STATE::HOLD)
	{
		a = true;
	}
	if (mpInputManager->GetKeyState(KEY::_8) == KEY_STATE::HOLD)
	{
		a = false;
	}
	if (a == true)
	{
	mpDearsGraphicsEngine->RendPostProcessing();

	}

	///여기 후처리 필터를 적용할수 있다. DearsGame엔진에서 이러한 부분이 힘들다면.. EndRender안에 넣는 꼼수도 가능하지 않을까?



 	mpDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, Matrix::CreateTranslation({ 0,0,-35 }));
 	mpDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, Matrix::CreateTranslation({ 0,0,0 }));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRThigh", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigPelvis", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLThigh", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRCollarbone", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRibcage", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigHead", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigSpine2", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRUpperarm", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRForearm", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigSpine1", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLFoot", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLToes1", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLCalf", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLPalm", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLCollarbone", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLUpperarm", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLForearm", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigPelvis", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLThigh", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLCalf", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLFoot", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigLToes1", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRThigh", 0.1f));
// 	m_pDearsGraphicsEngine->Rend_DebugBox(tempScl, tempRot, tempPos, m_pDearsGraphicsEngine->GetTargetBoneAboveMatrix("Character 01.FBX", "RigRCalf", 0.1f));

	//m_pDearsGraphicsEngine->Rend_DebugBox({30,10,20 }, { 0,0,0 }, {0,0,0 }, Matrix::CreateTranslation({ 10,0,0 }));


	//m_pDearsGraphicsEngine->Rend_DebugSphere({ 1,1,1 }, { 0,0,0 }, Vector3(2.0f, 0.0f, 0.0f));
	mpDearsGraphicsEngine->Rend_DebugSphere({ 1.f,1.f,1.f }, { 0,0,0 }, mCConstantBuffer.light[0].position);
// 	m_pDearsGraphicsEngine->Rend_DebugSphere({ 1.f,1.f,1.f }, { 0,0,0 }, tempCCConstantBuffer.light[1].position);
// 	m_pDearsGraphicsEngine->Rend_DebugSphere({ 1.f,1.f,1.f }, { 0,0,0 }, tempCCConstantBuffer.light[2].position);
	//m_pDearsGraphicsEngine->Rend_DebugCapsule({ 2,2,2 }, { 0,0,0 }, { -4,0,0 });
	mpDearsGraphicsEngine->EndRender();
}

void GameEngine::GameLoop()
{
	Update();
	Render();
}

void GameEngine::Finalize()
{

}

/// 정돈용-----------------------------------------------------------------------------------
void GameEngine::InitializeManager()
{
	mpTimeManager = TimeManager::getInstance();
	mpTimeManager->Reset();

	mpInputManager = InputManager::getInstance();
	mpInputManager->SetHwnd(mHWnd);

	mpFileManager->Initialize();
	mpFileManager->SearchOneFile("Michelle", "Michelle.fbx");
}
