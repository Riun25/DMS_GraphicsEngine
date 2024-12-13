#pragma once
#include "DearsGraphicsEngine.h"
struct ModelBuffer;
struct VSConstantBufferData;
struct VSBoneConstantBufferData;
struct PSConstantBufferData;

class tempObject
{
public:
	tempObject() = delete;
	tempObject(DearsGraphicsEngine* _pGrapicsEngine);
	~tempObject();

private:
	DearsGraphicsEngine* mpGraphicsEngine;

public:
	/// <summary>
	/// model, view, projection, invTranspose
	/// </summary>
	VSConstantBufferData mpVSConstantBufferData;

	/// <summary>
	/// Matrix bone[MAX_BONES]
	/// </summary>
	VSBoneConstantBufferData mpVSBoneConstantBufferData;

	/// <summary>
	/// Matrix targrtBoneMatrix;
	/// </summary>
	VSTargetBoneConstantBufferData mpTargetBoneConstantBufferData;
	
	/// <summary>
	/// Matrix world[MAX_INSTANT]
	/// Matrix invWorld[MAX_INSTANT]
	/// </summary>
	VSInstantConstantBufferData mpInstantConstantBufferData;

	/// <summary>
	/// eyeworld, maxLights, material, light[MaxLight]
	/// </summary>
	PSConstantBufferData mPSConstantBuffer;

	//아래 두개는 지금 안쓰지만 후에 쓸일이 있을까봐 지우지 못했다. 
	VSShadowConstantBufferData mVSShadowConstantBufferData;
	PsShadowConstantBufferData mPSShadowConstantBufferData;

	VSEdgeConstantBufferData mVSEdgeConstantBufferData;
	PSEdgeConstantBufferData mPSEdgeConstantBufferData;
	
	//물쉐이딩에 필요한 정보 UV좌표계를 윔직이게 하기위한.
	VSWaterConstantBufferData mVSWaterConstantBufferData;

	bool mIsVSconstant;
	bool mIsVSBoneConstant;
	bool mIsVSTargetBoneConstant;
	bool mIsVSInstanceConstant;
	bool mIsPSconstant;
	bool mIsVSShadowConstant;
	bool mIsPSShadowConstant = false;

	bool mIsVSEdgeConstant;
	bool mIsPSEdgeConstant;

	bool mIsVSWaterConstant = false;


	int mTargetBoneIndex;			//장비의 경우 타겟본의 인덱스를 알고 있어야한다. -> 똑같은 계산을 두번 안해도 된다.


public:
	ModelBuffer* mpModelBuffer;

	Matrix objectPos;
	Matrix objectRot;
	Matrix objectScl;

public:
	
	void CreateVSConstantBuffer() 
	{
		mpModelBuffer->pVSConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mpVSConstantBufferData);
		mIsVSconstant = true;
	};

	void CreateVSBoneConstantBuffer() 
	{
		mpModelBuffer->pBoneConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mpVSBoneConstantBufferData); mIsVSBoneConstant = true;
	};

	void CreateVSTargetBoneConstantBuffer() 
	{
		mpModelBuffer->pTargetBoneConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mpTargetBoneConstantBufferData); mIsVSTargetBoneConstant = true;
	};

	void CreatePSConstantBuffer() 
	{
		mpModelBuffer->pPSConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mPSConstantBuffer);  mIsPSconstant = true;
	};

	
	//난 이녀석을 업데이트 시킬 생각은 일단 지금없다. 
	void CreateVSInstanceConstantBuffer()
	{
		for (int i = 0; i < MAX_INSTANCE; i++)
		{
			mpInstantConstantBufferData.world[i] = Matrix::CreateTranslation({ static_cast<float>(i),0,0 }).Transpose();
			mpInstantConstantBufferData.world[i].Invert();
			mpModelBuffer->numInstances++;
		}
		mpModelBuffer->pInstanceConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mpInstantConstantBufferData); mIsVSInstanceConstant = true;
	}

	void CreateVSEdgeConstantBuffer()
	{
		mpModelBuffer->pVSEdgeConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mpVSConstantBufferData);
		mIsVSEdgeConstant = true;
	}
	
	void CreatePSEdgeConstantBuffer()
	{
		mpModelBuffer->pPSEdgeConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mPSEdgeConstantBufferData);
		mIsPSEdgeConstant = true;
	}

	void CreateVSWaterConstantBuffer()
	{
		mpModelBuffer->pVSWaterConstantBuffer = mpGraphicsEngine->CreateConstantBuffer(mVSWaterConstantBufferData);
		mIsVSWaterConstant = true;
	
	}


	void Initialize();
	void Update();
	//void SetModelBuffer(std::string _VIBname, std::string _dTname, std::string _MIname = std::string(), std::string _Aname = std::string());
	void SetVIBuffer(std::string _bufferName);
	void SetDiffuseTexture(std::string _TextureName);
	void SetNormalMapTexture(std::string _TextureName);
	void SetCubeMapTexture(std::string _DiffuseTextureName = "", std::string _SpecularTextureName = "");

	void SetAnimation(std::string _AnimationName);
	void SetNextAnimation(std::string _NextAnimationname);
	void SetModelInfo(std::string _ModelName);
	void SetTargetBoneIndex(int _targetBoneIndex);

	void UpdateAnimationTime(float _deltaTime);
	void UpdatePSConstantBufferData(PSConstantBufferData& _PSConstantdata);

	void UpdateVSShadowConstantBufferData(Matrix _shadowView, Matrix _shadowProjection);

	void SetObjectPos(Matrix _pos);
	void SetObjectRot(Matrix _Rot);
	void SetObjectScl(Matrix _Scl);

	void GetObjectTargetBoneMatrix(std::string _targetModel, std::string _targetBoneName);
	void GetObjectTargetBoneMatrix(VSBoneConstantBufferData _targetModelBoneConstantBuffer);
	ModelBuffer* GetModelBuffer();
};

