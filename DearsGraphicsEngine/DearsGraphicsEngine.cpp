#if DEBUG
#incldue<iostream>
#endif
#include "DearsGraphicsEngine.h"
#include "GraphicsCommon.h"


DearsGraphicsEngine::DearsGraphicsEngine(HWND _hWnd, int screenWidth, int screenHeight)
{
	mHWnd = _hWnd;
	mScreenHeight = screenHeight;
	mScreenWidth = screenWidth;
	mpResourceManager = nullptr;
	mpRenderer = nullptr;
	mpDearsImGui = nullptr;
	mpTargetCamera = nullptr;
	mpAnimationHelper = nullptr;
	mpLightHelper = nullptr;
}

DearsGraphicsEngine::~DearsGraphicsEngine()
{

	delete mpDearsImGui;
	delete mpRenderer;
	delete mpResourceManager;
	delete mpTargetCamera;
	delete mpAnimationHelper;

}
//��������� �ʱ�ȭ�� ��Ų��.
void DearsGraphicsEngine::Initialize()
{
	//�켱 ����̽� ������ �����Ѵ�. 	//  ���� ü��, �� ��Ʈ, ���� ���ٽ� , ���� Ÿ�� �� �������� Renderer���� InitalizeD3D�Լ� �ȿ����Ѵ�.

	RendererHelper::CreateDevice(mpDevice, mpDeviceContext);
	
	//������ ����
	mpRenderer = new Renderer(mHWnd, 0, 0, mScreenWidth, mScreenHeight, mpDevice, mpDeviceContext);
	mpRenderer->Initialize(mpResourceManager);

	//���ҽ� �Ŵ��� ����
	mpResourceManager = new GraphicsResourceManager(mpDevice, mpDeviceContext);

	//�ִϸ��̼� ����Ŭ���� ����
	mpAnimationHelper = new AnimationHelper();

	//ImGUI ����
	mpDearsImGui = new DearsImGui(mHWnd, mpDevice, mpDeviceContext, mScreenWidth, mScreenHeight, mpResourceManager);
	

	//����Ʈ ���ۻ���
	mpLightHelper = new LightHelper;

 	mpDebugModelBuffer = new ModelBuffer;
	AddModel("../TestAsset/", "Debug_Box.fbx");
	AddModel("../TestAsset/", "Debug_Capsule.fbx");
	AddModel("../TestAsset/", "Debug_Sphere.fbx");

 	VSConstantBufferData temp;
 	mpDebugModelBuffer->pVSConstantBuffer = CreateConstantBuffer(temp);

	Dears::Graphics::InitCommonStates(mpDevice);

	MeshData cubeMeshData = GeometryGenerator::MakeBox(1000);
	std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());
	mpResourceManager->AddModel(cubeMeshData, "CubeMap");

	MeshData SphereMeshData = GeometryGenerator::MakeSphere(1.f, 300, 300);
	mpResourceManager->AddModel(SphereMeshData, "MySphere");

	MeshData SquareMeshData = GeometryGenerator::MakeSquare(1);
	mpResourceManager->AddModel(SquareMeshData, "MySquare");

	MeshData BillBoardSquareMeshData = GeometryGenerator::BillboradSquare(1);
	mpResourceManager->AddModel(BillBoardSquareMeshData, "BillBoardSquare");

	MeshData boxMeshData = GeometryGenerator::MakeBox(1);
	mpResourceManager->AddModel(boxMeshData, "MyBox");

	mpParticleManager = new ParticleManager(mpDevice, mpDeviceContext, MAX_PARTICLE);
	mpParticleManager->Initialize();
	mpParticleManager->SetVertexBufferAndIndexBuffer(mpResourceManager->Get_VertexBuffer("BillBoardSquare"), 
														mpResourceManager->Get_IndexBuffer("BillBoardSquare"),
														mpResourceManager->Get_NumIndex("BillBoardSquare"));
	mpPostProcessingBuffer = new ModelBuffer;
	mpPostProcessingBuffer->pVertexBuffer = Get_VertexBuffer("BillBoardSquare");
	mpPostProcessingBuffer->pIndexBuffer = Get_IndexBuffer("BillBoardSquare");
	mpPostProcessingBuffer->numIndices = Get_NumIndex("BillBoardSquare");
}

void DearsGraphicsEngine::Update()
{
	mpParticleManager->Update();
}

void DearsGraphicsEngine::BeginRender()
{
	mpRenderer->BeginRender();
	UIBegineRender();
	UICanvasSet(Vector2(0, 0), Vector2(static_cast<float>(mScreenWidth), static_cast<float>(mScreenHeight)));
 	SetUICurrentWindow();
}


void DearsGraphicsEngine::EndRender()
{

	RenderImGui();
	EndRenderImGui();
	mpRenderer->EndRender();
}

void DearsGraphicsEngine::Finalize()
{

}

void DearsGraphicsEngine::RendParticle()
{
	mpParticleManager->Render();
}

void DearsGraphicsEngine::RendPostProcessing()
{
	mpRenderer->SetPipelineState(Dears::Graphics::gSamplerPSO);
	for (int i = 0; i < 10; i++)
	{
	mpRenderer->RenderSampler(mpPostProcessingBuffer);

	mpRenderer->SetPipelineState(Dears::Graphics::gPostEffectPSO);
	mpRenderer->RenderPostProcessing(mpPostProcessingBuffer);
	}
}

int DearsGraphicsEngine::GetScreenWidth() const
{
	return mScreenWidth;
}

int DearsGraphicsEngine::GetScreenHeight() const
{
	return mScreenHeight;
}

float DearsGraphicsEngine::GetAspectRatio()
{
	return 0;
}


void DearsGraphicsEngine::AddModel(std::string _basePath, std::string _fileName)
{
	mpResourceManager->AddModel(_basePath, _fileName);
}

void DearsGraphicsEngine::AddAnimation(std::string _basePath, std::string _fileName)
{
	mpResourceManager->AddAnimation(_basePath, _fileName);

}

void DearsGraphicsEngine::Add3DTexture(std::string _basePath, std::string _fileName)
{
	mpResourceManager->Add3DTexture(_basePath, _fileName);

}

void DearsGraphicsEngine::Add2DTexture(std::string _basePath, std::string _fileName)
{
	mpResourceManager->Add2DTexture(_basePath, _fileName);

}
void DearsGraphicsEngine::AddDDSTexture(std::string _basePath, std::string _fileName)
{
	mpResourceManager->AddDDSTexture(_basePath, _fileName);

}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::Get_VertexBuffer(std::string _modelName)
{
	return mpResourceManager->Get_VertexBuffer(_modelName);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::Get_IndexBuffer(std::string _modelName)
{
	return mpResourceManager->Get_IndexBuffer(_modelName);
}

unsigned int DearsGraphicsEngine::Get_NumIndex(std::string _modelName)
{
	return mpResourceManager->Get_NumIndex(_modelName);
}

Model* DearsGraphicsEngine::Get_ModelInfo(std::string _modelName)
{
	return mpResourceManager->Get_ModelInfo(_modelName);
}

Animation* DearsGraphicsEngine::Get_Animation(std::string _animeName)
{
	return mpResourceManager->Get_Animation(_animeName);
}

ComPtr<ID3D11ShaderResourceView> DearsGraphicsEngine::Get_Textures(std::string _textureName)
{
	return mpResourceManager->Get_Textures(_textureName);
}

ImFont* DearsGraphicsEngine::Get_Font(std::string _fontName)
{
	return mpResourceManager->Get_Font(_fontName);
}

int DearsGraphicsEngine::Get_TargetModelBoneIndex(std::string _modelName, std::string _boneName)
{
	return mpResourceManager->Get_TargetModelBoneIndex(_modelName, _boneName);
}

Matrix DearsGraphicsEngine::GetTargetBoneMatrix(std::string _targetModel, std::string _targetBoneName)
{
	return mpResourceManager->Get_TargetBoneMatrix(_targetModel, _targetBoneName);
}

Matrix DearsGraphicsEngine::GetTargetBoneAboveMatrix(std::string _targetModel, std::string _targetBoneName, float _scale /*= 1.f*/)
{
	return mpResourceManager->Get_TargetBoneAboveMatrix(_targetModel, _targetBoneName)* Matrix::CreateScale(_scale);
}

Matrix DearsGraphicsEngine::GetTargetBoneAboveMatrix(std::string _targetModel, int _index, float _scale /*= 1.f*/)
{
	return mpResourceManager->Get_TargetBoneAboveMatrix(_targetModel, _index) * Matrix::CreateScale(_scale);
}

AABB DearsGraphicsEngine::Get_AABB(std::string __targetModel)
{
	return mpResourceManager->Get_ModelInfo(__targetModel)->mpMeshData->mAABB;
}

void DearsGraphicsEngine::Erase_VertexBuffer(std::string _modelName)
{
	mpResourceManager->Erase_VertexBuffer(_modelName);
}

void DearsGraphicsEngine::Erase_IndexBuffer(std::string _modelName)
{
	mpResourceManager->Erase_IndexBuffer(_modelName);
}

void DearsGraphicsEngine::Erase_NumIndex(std::string _modelName)
{
	mpResourceManager->Erase_NumIndex(_modelName);
}

void DearsGraphicsEngine::Erase_ModelInfo(std::string _modelName)
{
	mpResourceManager->Erase_ModelInfo(_modelName);
}

void DearsGraphicsEngine::Erase_Animation(std::string _animName)
{
	mpResourceManager->Erase_Animation(_animName);
}

void DearsGraphicsEngine::Erase_Textures(std::string _textureName)
{
	mpResourceManager->Erase_Textures(_textureName);
}

void DearsGraphicsEngine::Erase_Font(std::string _fontName)
{
	mpResourceManager->Erase_Font(_fontName);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(VSConstantBufferData& _VsConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _VsConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(VSBoneConstantBufferData& _VsBoneConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _VsBoneConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(VSTargetBoneConstantBufferData& _VsTargetBoneConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _VsTargetBoneConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(PSConstantBufferData& _PsConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _PsConstantBufferData);
}


ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(VSShadowConstantBufferData& _VsShadowConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _VsShadowConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(PsShadowConstantBufferData& _PsShadowConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _PsShadowConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(CommonConstantBufferData& _CommonConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _CommonConstantBufferData);

}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(VSInstantConstantBufferData& _VSInstantConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _VSInstantConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateConstantBuffer(PSEdgeConstantBufferData& _pPSEdgeConstantBufferData)
{
	return RendererHelper::CreateConstantBuffer(mpDevice, _pPSEdgeConstantBufferData);
}

ComPtr<ID3D11Buffer> DearsGraphicsEngine::CreateStructuredBuffer(CSParticleData& _TestCSParticleData, unsigned int _count)
{
	return RendererHelper::CreateStructuredBuffer(mpDevice, _TestCSParticleData, _count);
}

void DearsGraphicsEngine::UpdateConstantBuffer(ModelBuffer* _pModelBuffer, VSConstantBufferData& _VsConstantBufferData)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _VsConstantBufferData, _pModelBuffer->pVSConstantBuffer);
}

void DearsGraphicsEngine::UpdateConstantBuffer(ModelBuffer* _pModelBuffer, PSConstantBufferData& _PsConstantBufferData)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _PsConstantBufferData, _pModelBuffer->pPSConstantBuffer);
}

void DearsGraphicsEngine::UpdateVSEdgeConstantBuffer(ModelBuffer* _pModelBuffer, VSEdgeConstantBufferData& _pPSEdgeConstantBuffer)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _pPSEdgeConstantBuffer, _pModelBuffer->pVSEdgeConstantBuffer);

}

void DearsGraphicsEngine::UpdatePSEdgeConstantBuffer(ModelBuffer* _pModelBuffer, PSEdgeConstantBufferData& _pPSEdgeConstantBuffer)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _pPSEdgeConstantBuffer, _pModelBuffer->pPSEdgeConstantBuffer);
}

void DearsGraphicsEngine::UpdateVSWaterConstantBuffer(ModelBuffer* _pModelBuffer, VSWaterConstantBufferData& _pVSWaterConstantBuffer)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _pVSWaterConstantBuffer, _pModelBuffer->pVSWaterConstantBuffer);

}

void DearsGraphicsEngine::UpdateCommonConstantBuffer(CommonConstantBufferData& _CommonBufferData)
{
 	_CommonBufferData.view = mpTargetCamera->GetViewRow().Transpose();		// ���� ��ȯ
 	_CommonBufferData.proj = mpTargetCamera->GetProjRow().Transpose();
 	_CommonBufferData.viewProj = (mpTargetCamera->GetViewRow() * mpTargetCamera->GetProjRow()).Transpose();

	_CommonBufferData.invView = _CommonBufferData.view.Invert();
 	_CommonBufferData.invProj = _CommonBufferData.proj.Invert();
	
	//�׸��� �������� ���
 	_CommonBufferData.invViewProj = _CommonBufferData.viewProj.Invert();

	_CommonBufferData.eyeWorld = mpTargetCamera->GetmViewPos();

	mpRenderer->UpdateCommonConstantBuffer(_CommonBufferData);
	
}

void DearsGraphicsEngine::UpdateShadowConstantBuffer(ModelBuffer* _pModelBuffer, VSShadowConstantBufferData& _VsShadowConstantBufferData)
{
//	m_pResourceManager->UpdateBuffer(_VsShadowConstantBufferData, _pModelBuffer->m_pShadowConstantBuffer);
}

void DearsGraphicsEngine::UpdateShadowConstantBuffer(ModelBuffer* _pModelBuffer, PsShadowConstantBufferData& _PsShadowConstantBufferData)
{
//	m_pResourceManager->UpdateBuffer(_PsShadowConstantBufferData, _pModelBuffer->m_pShadowPSConstantBuffer);
}

void DearsGraphicsEngine::Set_CubeMap(std::string diffuseTextureName, std::string specularTextureName)
{
	mpRenderer->SetCommonShaderResource
	(
		mpResourceManager->Get_Textures(diffuseTextureName),
		mpResourceManager->Get_Textures(specularTextureName)
	);
}

///
void DearsGraphicsEngine::UpdateBoneConstantBuffer(ModelBuffer* _pModelBuffer, VSBoneConstantBufferData& _VsBoneConstantBufferData)
{
	mpAnimationHelper->UpdateBoneConstant(
		_pModelBuffer->pTargetModel,
		_pModelBuffer->pTargetAnimation,
		_VsBoneConstantBufferData,
		_pModelBuffer->animationPlaytime
		);
	RendererHelper::UpdateBuffer(mpDeviceContext, _VsBoneConstantBufferData, _pModelBuffer->pBoneConstantBuffer);
}
///
bool DearsGraphicsEngine::UpdateTransitionBoneConstantBuffer(ModelBuffer* _pModelBuffer, VSBoneConstantBufferData& _VsBoneConstantBufferData)
{
	if (mpAnimationHelper->UpdateBoneConstant(
		_pModelBuffer->pTargetModel,
		_pModelBuffer->pTargetAnimation,
		_pModelBuffer->pNextTargetAnimation,
		_VsBoneConstantBufferData,
		_pModelBuffer->animationPlaytime,
		_pModelBuffer->nextAnimationPlaytime)
		)
	{
		
		RendererHelper::UpdateBuffer(mpDeviceContext, _VsBoneConstantBufferData, _pModelBuffer->pBoneConstantBuffer);
		return true;
	}
	else
	{
	return false;
	}
}

void DearsGraphicsEngine::UpdateTargetBoneConstantBuffer(ModelBuffer* _pModelBuffer, VSTargetBoneConstantBufferData& _VsTargetBoneConstantBufferData)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _VsTargetBoneConstantBufferData, _pModelBuffer->pTargetBoneConstantBuffer);
}

void DearsGraphicsEngine::SetPipelineState(PipelineStateObject& _pso)
{
	mpRenderer->SetPipelineState(_pso);
}

void DearsGraphicsEngine::Rend_AnimateModel(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gAnimeGeometryPSO);
	mpRenderer->Render(_modelBuffer);
}

void DearsGraphicsEngine::Rend_Model(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gBasicGeometryPSO);
	mpRenderer->Render(_modelBuffer);
}

void DearsGraphicsEngine::Rend_EquipmentModel(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gEquipmentGeometryPSO);
	mpRenderer->Render(_modelBuffer);
}

void DearsGraphicsEngine::SetOpacityFactor(float blendFactor[4])
{
	for (int i = 0; i < 4; i++)
	{
	Dears::Graphics::gOpacityPSO.mBlendFactor[i] = blendFactor[i];
	}
}

void DearsGraphicsEngine::Rend_OpacitiyModel(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gOpacityPSO);
	mpRenderer->Render(_modelBuffer);
}

void DearsGraphicsEngine::Rend_EdgeModel(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gPunchingPSO);
	mpRenderer->Render(_modelBuffer);
	SetPipelineState(Dears::Graphics::gEdgePSO);
	mpRenderer->RenderEdge(_modelBuffer);

}

void DearsGraphicsEngine::Rend_Water(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gWaterPSO);
	mpRenderer->Render(_modelBuffer);
}

void DearsGraphicsEngine::RenderDepthMap(ModelBuffer* _modelbuffer)
{
	mpRenderer->RenderDepthMap(_modelbuffer);
}

void DearsGraphicsEngine::RenderAniDepthMap(ModelBuffer* _modelbuffer)
{
	mpRenderer->RenderAniDepthMap(_modelbuffer);
}

void DearsGraphicsEngine::RenderEquipDepthMap(ModelBuffer* _modelbuffer)
{
	mpRenderer->RenderEquipDepthMap(_modelbuffer);
}

void DearsGraphicsEngine::Rend_InstancedModels(ModelBuffer* _modelbuffers)
{
	SetPipelineState(Dears::Graphics::gBasicInstancingPSO);
	///���⼭ �������� ���� �������� �����ϵ���
	mpRenderer->Rend_InstancedModels(_modelbuffers);
}

void DearsGraphicsEngine::Rend_BillBoard(ModelBuffer* _modelbuffers)
{
	SetPipelineState(Dears::Graphics::gTestPSO);
	mpRenderer->Render(_modelbuffers);

}

void DearsGraphicsEngine::Rend_DebugBox(Vector3 _size, Vector3 _rotation, Vector3 _transpose)
{
	SetPipelineState(Dears::Graphics::gDebugGeometryPSO);
	mpDebugModelBuffer->pVertexBuffer = Get_VertexBuffer("Box");
	mpDebugModelBuffer->pIndexBuffer = Get_IndexBuffer("Box");
	mpDebugModelBuffer->numIndices = Get_NumIndex("Box");
	VSConstantBufferData temp;
	temp.world = (Matrix::CreateScale(_size) * Matrix::CreateRotationX(_rotation.x) * Matrix::CreateRotationY(_rotation.y) *
		Matrix::CreateRotationZ(_rotation.z) * Matrix::CreateTranslation(_transpose)).Transpose();
	UpdateConstantBuffer(mpDebugModelBuffer, temp);
	mpRenderer->Render(mpDebugModelBuffer);

}

void DearsGraphicsEngine::Rend_DebugBox(Matrix _size, Matrix _rotation, Matrix _transpose)
{
	SetPipelineState(Dears::Graphics::gDebugGeometryPSO);
	mpDebugModelBuffer->pVertexBuffer = Get_VertexBuffer("Box");
	mpDebugModelBuffer->pIndexBuffer = Get_IndexBuffer("Box");
	mpDebugModelBuffer->numIndices = Get_NumIndex("Box");

	VSConstantBufferData temp;
 	temp.world = (_size * _rotation * _transpose).Transpose();

	UpdateConstantBuffer(mpDebugModelBuffer, temp);

	mpRenderer->Render(mpDebugModelBuffer);
}

void DearsGraphicsEngine::Rend_DebugBox(Matrix _size, Matrix _rotation, Matrix _transpose, Matrix _tempMatrix = Matrix())
{
	SetPipelineState(Dears::Graphics::gDebugGeometryPSO);
	mpDebugModelBuffer->pVertexBuffer = Get_VertexBuffer("Box");
	mpDebugModelBuffer->pIndexBuffer = Get_IndexBuffer("Box");
	mpDebugModelBuffer->numIndices = Get_NumIndex("Box");
	VSConstantBufferData temp;
	temp.world = _size * _rotation * _transpose* _tempMatrix;
	temp.world = temp.world.Transpose();
	
	UpdateConstantBuffer(mpDebugModelBuffer, temp);

	mpRenderer->Render(mpDebugModelBuffer);
}

void DearsGraphicsEngine::Rend_DebugBox(AABB& _AABB, Matrix Scale, Matrix _rotation, Matrix _tempMatrix )
{
	Vector3 _tempSize = { _AABB.mMax.x - _AABB.mMin.x,
		_AABB.mMax.y - _AABB.mMin.y,
		_AABB.mMax.z - _AABB.mMin.z,
	};
	Matrix tempSizeMatrix = Matrix::CreateScale(_tempSize);
	tempSizeMatrix *= Scale;

 	Vector3	centerAABB = {
 	(_AABB.mMax.x + _AABB.mMin.x) / 2.0f,
 	(_AABB.mMax.y + _AABB.mMin.y) / 2.0f,
 	(_AABB.mMax.z + _AABB.mMin.z) / 2.0f
 	};
	//centerAABB *= Scale;
	Matrix tempTranslationMatrix = Matrix::CreateTranslation(centerAABB);
	Rend_DebugBox(tempSizeMatrix, _rotation, tempTranslationMatrix, _tempMatrix);
}

void DearsGraphicsEngine::Rend_DebugSphere(Vector3 _size, Vector3 _rotation, Vector3 _transpose)
{
	SetPipelineState(Dears::Graphics::gDebugGeometryPSO);
	mpDebugModelBuffer->pVertexBuffer = Get_VertexBuffer("Sphere");
	mpDebugModelBuffer->pIndexBuffer = Get_IndexBuffer("Sphere");
	mpDebugModelBuffer->numIndices = Get_NumIndex("Sphere");

	VSConstantBufferData temp;
	temp.world = (Matrix::CreateScale(_size) * Matrix::CreateRotationX(_rotation.x) * Matrix::CreateRotationY(_rotation.y) *
		Matrix::CreateRotationZ(_rotation.z) * Matrix::CreateTranslation(_transpose)).Transpose();

	UpdateConstantBuffer(mpDebugModelBuffer, temp);

	mpRenderer->Render(mpDebugModelBuffer);
}

void DearsGraphicsEngine::Rend_DebugCapsule(Vector3 _size, Vector3 _rotation, Vector3 _transpose)
{
	SetPipelineState(Dears::Graphics::gDebugGeometryPSO);
	mpDebugModelBuffer->pVertexBuffer = Get_VertexBuffer("Capsule");
	mpDebugModelBuffer->pIndexBuffer = Get_IndexBuffer("Capsule");
	mpDebugModelBuffer->numIndices = Get_NumIndex("Capsule");

	VSConstantBufferData temp;
	temp.world = (Matrix::CreateScale(_size) * Matrix::CreateRotationX(_rotation.x) * Matrix::CreateRotationY(_rotation.y) *
		Matrix::CreateRotationZ(_rotation.z) * Matrix::CreateTranslation(_transpose)).Transpose();

	UpdateConstantBuffer(mpDebugModelBuffer, temp);

	mpRenderer->Render(mpDebugModelBuffer);
}

void DearsGraphicsEngine::Rend_CubeMap(ModelBuffer* _modelBuffer)
{
	SetPipelineState(Dears::Graphics::gCubeMapGeometryPSO);
	mpRenderer->Render_CubeMap(_modelBuffer);
}

void DearsGraphicsEngine::SetCamera(Camera* _pTargetCamera)
{
	mpTargetCamera=_pTargetCamera;
}

void DearsGraphicsEngine::AddFont(std::string _basePath, std::string _fileName, float _size, bool _isKorean)
{
	mpDearsImGui->UILoadFonts(_basePath, _fileName, _size, _isKorean);
}

void DearsGraphicsEngine::FontSetFinish()
{
	mpDearsImGui->UIBuildFonts();
}

void DearsGraphicsEngine::UIBegineRender()
{
	mpDearsImGui->UIBeginRender();
}

void DearsGraphicsEngine::UICanvasSet(Vector2 _posXY, Vector2 _sizeWH)
{
	mpDearsImGui->UICanvasSet(_posXY, _sizeWH);
}

void DearsGraphicsEngine::UIDrawImageStart()
{
	mpDearsImGui->UIDrawImageStart();
}

void DearsGraphicsEngine::UIDrawImage(Vector2 _posXY, Vector2 _sizeWH, std::string _textureName, Vector4 _rgba /*= Vector4(1.0f, 1.0f, 1.0f, 1.0f)*/)
{
	ComPtr<ID3D11ShaderResourceView> tempSRV = mpResourceManager->Get_Textures(_textureName);
	mpDearsImGui->UIDrawImage(_posXY, _sizeWH, tempSRV, _rgba);
}

void DearsGraphicsEngine::UIDrawImageFin()
{
	mpDearsImGui->UIDrawImageFin();
}

void DearsGraphicsEngine::UIStartFontID(std::string _fontName)
{
	mpDearsImGui->UIStartFontID(mpResourceManager->Get_Font(_fontName));
}

void DearsGraphicsEngine::UIDrawText(Vector2 _pos, std::u8string _text, Vector4 _rgba)
{
	mpDearsImGui->UIDrawText(_pos, mpDearsImGui->ConvertUTF8String(_text), _rgba);
}

void DearsGraphicsEngine::UIFinFontID()
{
	mpDearsImGui->UIEndFontID();
}

void DearsGraphicsEngine::SetUICurrentWindow()
{
	mpDearsImGui->SetUICurrentWindow();
}

void DearsGraphicsEngine::UIDrawRect(Vector2 _posXY, Vector2 _sizeWH, Vector4 _rgba, float _rounding /*= 0.0f*/, float _thickness)
{
	mpDearsImGui->UIDrawRect(_posXY, _sizeWH, _rgba, _rounding, _thickness);
}

void DearsGraphicsEngine::UIDrawRectFilled(Vector2 _posXY, Vector2 _sizeWH, Vector4 _rgba, float _rounding /*= 0.0f*/)
{
	mpDearsImGui->UIDrawRectFilled(_posXY, _sizeWH, _rgba, _rounding);
}

void DearsGraphicsEngine::UIDrawRectwithBorder(Vector2 _posXY, Vector2 _sizeWH, Vector4 _rgba, float _rounding /*= 0.0f*/, float _thickness)
{
	mpDearsImGui->UIDrawRectwithBorder(_posXY, _sizeWH, _rgba, _rounding, _thickness);
}

void DearsGraphicsEngine::UIFreeRect(Vector2 _posXY1, Vector2 _posXY2, Vector2 _posXY3, Vector2 _posXY4, Vector4 _rgba, float _thickness)
{
	mpDearsImGui->UIFreeRect(_posXY1, _posXY2, _posXY3, _posXY4, _rgba, _thickness);
}

void DearsGraphicsEngine::UIFreeRectFilled(Vector2 _posXY1, Vector2 _posXY2, Vector2 _posXY3, Vector2 _posXY4, Vector4 _rgba)
{
	mpDearsImGui->UIFreeRectFilled(_posXY1, _posXY2, _posXY3, _posXY4, _rgba);
}

void DearsGraphicsEngine::UIFreeRectwithBorder(Vector2 _posXY1, Vector2 _posXY2, Vector2 _posXY3, Vector2 _posXY4, Vector4 _rgba, float _thickness, Vector4 _borderRgba)
{
	mpDearsImGui->UIFreeRectwithBorder(_posXY1, _posXY2, _posXY3, _posXY4, _rgba, _thickness, _borderRgba);
}

void DearsGraphicsEngine::UIDrawLine(Vector2 _sPosXY, Vector2 _ePosXY, Vector4 _rgba)
{
	mpDearsImGui->UIDrawLine(_sPosXY, _ePosXY, _rgba);
}

void DearsGraphicsEngine::UIDrawCir(Vector2 _posXY, float _radius, Vector4 _rgba)
{
	mpDearsImGui->UIDrawCircle(_posXY, _radius, _rgba);
}

void DearsGraphicsEngine::RenderImGui()
{
	mpDearsImGui->UICanvasSetFin();
	mpDearsImGui->UIRender();
}

void DearsGraphicsEngine::EndRenderImGui()
{
	mpDearsImGui->UIEndRender();
}

void DearsGraphicsEngine::LightInitialize(CommonConstantBufferData* _psBufferData, UINT _num)
{
	mpLightHelper->Initialize(_psBufferData, _num);
}

void DearsGraphicsEngine::ChangeLightMaxNum(CommonConstantBufferData* _psBufferData, UINT _num)
{
	mpLightHelper->ChangeLightMaxNum(_psBufferData, _num);
}

void DearsGraphicsEngine::LightUpdate(CommonConstantBufferData* _psBufferData)
{
	//mpLightHelper->Update(_psBufferData);
	//TODO : ���⼭ �׸��� ������Ʈ���� ���ִ°� ���ƺ��δ�.
	mpRenderer->LightUpdate(_psBufferData);



}

void DearsGraphicsEngine::PrintLightInfo(CommonConstantBufferData* _psBufferData)
{
	mpLightHelper->PrintInfo(_psBufferData);
}

void DearsGraphicsEngine::SetLightSettingAll(CommonConstantBufferData* _psBufferData, UINT _index, LightEnum _lightType, float _strength, float _fallOffStart,
											 float _fallOffEnd, Vector3 _dir, Vector3 _pos, float _spotPower, Vector3 _color)
{
	mpLightHelper->SetLightSettingAll(_psBufferData, _index, _lightType, _strength, _fallOffStart, _fallOffEnd, _dir, _pos, _spotPower, _color);
}

void DearsGraphicsEngine::SetDirLight(CommonConstantBufferData* _psBufferData, UINT _index, float _strength, Vector3 _dir, Vector3 _color)
{
	_dir.Normalize();
	mpLightHelper->SetDirLight(_psBufferData, _index, _strength, _dir, _color);
}

void DearsGraphicsEngine::SetPointLight(CommonConstantBufferData* _psBufferData, UINT _index, float _strength, float _fallOffStart, float _fallOffEnd, Vector3 _pos, Vector3 _color)
{
	mpLightHelper->SetPointLight(_psBufferData, _index, _strength, _fallOffStart, _fallOffEnd, _pos, _color);
}

void DearsGraphicsEngine::SetSpotLight(CommonConstantBufferData* _psBufferData, UINT _index, float _strength, float _fallOffStart, float _fallOffEnd, Vector3 _dir, Vector3 _pos, float _spotPower, Vector3 _color)
{
	//_dir.Normalize();
	mpLightHelper->SetSpotLight(_psBufferData, _index, _strength, _fallOffStart, _fallOffEnd, _dir, _pos, _spotPower, _color);
}

void DearsGraphicsEngine::SetLightOff(CommonConstantBufferData* _psBufferData, UINT _index)
{
	mpLightHelper->SetLightOff(_psBufferData, _index);
}

void DearsGraphicsEngine::SetLightStrength(CommonConstantBufferData* _psBufferData, UINT _index, float _strength)
{
	mpLightHelper->SetStrength(_psBufferData, _index, _strength);
}

void DearsGraphicsEngine::SetLightType(CommonConstantBufferData* _psBufferData, UINT _index, LightEnum _lightType)
{
	mpLightHelper->SetLightType(_psBufferData, _index, _lightType);
}

void DearsGraphicsEngine::SetLightDir(CommonConstantBufferData* _psBufferData, UINT _index, Vector3 _dir)
{
	mpLightHelper->SetDirection(_psBufferData, _index, _dir);
}

void DearsGraphicsEngine::SetLightFallOffStart(CommonConstantBufferData* _psBufferData, UINT _index, float _distance)
{
	mpLightHelper->SetFallOffStart(_psBufferData, _index, _distance);
}

void DearsGraphicsEngine::SetLightFallOffEnd(CommonConstantBufferData* _psBufferData, UINT _index, float _length)
{
	mpLightHelper->SetFallOffEnd(_psBufferData, _index, _length);
}

void DearsGraphicsEngine::SetLightPos(CommonConstantBufferData* _psBufferData, UINT _index, Vector3 _pos)
{
	mpLightHelper->SetPosition(_psBufferData, _index, _pos);
}

void DearsGraphicsEngine::SetLightSpotPower(CommonConstantBufferData* _psBufferData, UINT _index, float _power)
{
	mpLightHelper->SetSpotPower(_psBufferData, _index, _power);
}

void DearsGraphicsEngine::SetLightColor(CommonConstantBufferData* _psBufferData, UINT _index, Vector3 _rgb)
{
	mpLightHelper->SetLightColor(_psBufferData, _index, _rgb);
}

void DearsGraphicsEngine::CopyLight(CommonConstantBufferData* _psBufferData, UINT _copy, UINT _origin)
{
	mpLightHelper->CopyLight(_psBufferData, _copy, _origin);
}

Matrix DearsGraphicsEngine::CreateShadowViewMatrix(const Light& light)
{
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

	// ������ �Ʒ����� �ٶ󺸰� �ִ� ���, up ���͸� ������ŵ�ϴ�.
	if (light.direction == up || light.direction == -up)
	{
		up = Vector3(0.0f, 0.0f, 1.0f);
	}

	// LookAt �Լ��� ����� shadowView ��Ʈ������ ����
	
	return Matrix::CreateLookAt(light.position, light.position + light.direction, up);
}

Matrix DearsGraphicsEngine::CreateShadowProjectionMatrix(const Light& light, float nearPlane, float farPlane, float fieldOfView, float aspectRatio)
{
	if (light.lightType == static_cast<int>(LightEnum::DIRECTIONAL_LIGHT))
	{
		// ���� ���� ��Ʈ���� (Orthographic Projection)
		return Matrix::CreateOrthographic(mScreenWidth, mScreenHeight, nearPlane, farPlane);
	}
	else
	{
		// ���� ���� ��Ʈ���� (Perspective Projection)
		return Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);
	}
}
