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

	//�Ʒ� �ΰ��� ���� �Ⱦ����� �Ŀ� ������ ������� ������ ���ߴ�. 
	VSShadowConstantBufferData mVSShadowConstantBufferData;
	PsShadowConstantBufferData mPSShadowConstantBufferData;

	VSEdgeConstantBufferData mVSEdgeConstantBufferData;
	PSEdgeConstantBufferData mPSEdgeConstantBufferData;
	
	//�����̵��� �ʿ��� ���� UV��ǥ�踦 �����̰� �ϱ�����.
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


	int mTargetBoneIndex;			//����� ��� Ÿ�ٺ��� �ε����� �˰� �־���Ѵ�. -> �Ȱ��� ����� �ι� ���ص� �ȴ�.


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

	
	//�� �̳༮�� ������Ʈ ��ų ������ �ϴ� ���ݾ���. 
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

