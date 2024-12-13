#include "tempObject.h"

tempObject::tempObject(DearsGraphicsEngine* _pGrapicsEngine)
{
	mpGraphicsEngine = _pGrapicsEngine;
	mpModelBuffer = nullptr;
	mTargetBoneIndex = -1;
	objectPos = Matrix();
	objectRot = Matrix();
	objectScl = Matrix();

	mIsVSconstant = false;
	mIsVSBoneConstant = false;
	mIsVSTargetBoneConstant = false;
	mIsVSInstanceConstant = false;
	mIsPSconstant = false;
	mIsVSShadowConstant = false;

	mIsVSEdgeConstant = false;
	mIsPSEdgeConstant = false;
}

tempObject::~tempObject()
{ 

}

void tempObject::Initialize()
{
	if (mpModelBuffer)
	{
		delete mpModelBuffer;
	}
	mpModelBuffer = new ModelBuffer();
}

void tempObject::Update()
{
	mpVSConstantBufferData.world =
		(
			objectScl * objectRot * objectPos
			).Transpose();
	mpVSConstantBufferData.invWorld = mpVSConstantBufferData.world.Transpose().Invert();

	if (mIsVSconstant)
	{
		mpGraphicsEngine->UpdateConstantBuffer(mpModelBuffer, mpVSConstantBufferData);
	}
	if (mIsVSBoneConstant)
	{
		if (mpModelBuffer->pTargetAnimation)
		{
			if (mpModelBuffer->pNextTargetAnimation)
			{
				if (!mpGraphicsEngine->UpdateTransitionBoneConstantBuffer(mpModelBuffer, mpVSBoneConstantBufferData))			  ///false가 반환되었는가? -> 다음 애니메이션을 현재의 애니메이션으로 교체한다.
				{
					mpModelBuffer->animationPlaytime = mpModelBuffer->nextAnimationPlaytime;
					mpModelBuffer->pTargetAnimation = mpModelBuffer->pNextTargetAnimation;
					mpModelBuffer->nextAnimationPlaytime = 0;
					mpModelBuffer->pNextTargetAnimation = nullptr;
				}
			}
			else
			{
				mpGraphicsEngine->UpdateBoneConstantBuffer(mpModelBuffer, mpVSBoneConstantBufferData);
			}
		}
	}

	if (mIsVSTargetBoneConstant)
	{
		mpGraphicsEngine->UpdateTargetBoneConstantBuffer(mpModelBuffer, mpTargetBoneConstantBufferData);
	}
	if (mIsVSShadowConstant)
	{
		mpGraphicsEngine->UpdateShadowConstantBuffer(mpModelBuffer, mVSShadowConstantBufferData);
	}
	if (mIsPSconstant)
	{
		mpGraphicsEngine->UpdateConstantBuffer(mpModelBuffer, mPSConstantBuffer);
	}
	if (mIsPSShadowConstant)
	{
		mpGraphicsEngine->UpdateShadowConstantBuffer(mpModelBuffer, mPSShadowConstantBufferData);
	}
	if (mIsVSEdgeConstant)
	{
		mpGraphicsEngine->UpdateVSEdgeConstantBuffer(mpModelBuffer, mVSEdgeConstantBufferData);

	}
	if (mIsPSEdgeConstant)
	{
		mpGraphicsEngine->UpdatePSEdgeConstantBuffer(mpModelBuffer, mPSEdgeConstantBufferData);

	}
	if (mIsVSWaterConstant)
	{
		mpGraphicsEngine->UpdateVSWaterConstantBuffer(mpModelBuffer, mVSWaterConstantBufferData);

	}
}


// / <summary>
// / 리소스 매니저를 통해 모델 버퍼를 한번에 세팅하는 함수
// / </summary>
// / <param name="_VBname">VertexBuffer명</param>
// / <param name="_IBname">Index Buffer명</param>
// / <param name="_NIname">NumIndex명</param>
// / <param name="_Tname">Texture명</param>
// / <param name="_MIname">정보를 받아올 TargetModel명</param>
// / <param name="_Aname">Animation명</param>
// void tempObject::SetModelBuffer(std::string _VIname, std::string _dTname, std::string _MIname, std::string _Aname)
// {
// 	mpModelBuffer->m_pVertexBuffer = mpGraphicsEngine->Get_VertexBuffer(_VIname);
// 	mpModelBuffer->m_pIndexBuffer = mpGraphicsEngine->Get_IndexBuffer(_VIname);
// 	mpModelBuffer->mNumIndices = mpGraphicsEngine->Get_NumIndex(_VIname);
// 
// 	mpModelBuffer->m_diffusetexture = mpGraphicsEngine->Get_Textures(_dTname);
// 	if (!_MIname.empty()) mpModelBuffer->mpTargetModel = mpGraphicsEngine->Get_ModelInfo(_MIname);
// 	if (!_Aname.empty()) mpModelBuffer->mpTargetAnimation = mpGraphicsEngine->Get_Animation(_Aname);
// }

void tempObject::SetVIBuffer(std::string _bufferName)
{
	mpModelBuffer->pVertexBuffer = mpGraphicsEngine->Get_VertexBuffer(_bufferName);
	mpModelBuffer->pIndexBuffer = mpGraphicsEngine->Get_IndexBuffer(_bufferName);
	mpModelBuffer->numIndices = mpGraphicsEngine->Get_NumIndex(_bufferName);
}

void tempObject::SetDiffuseTexture(std::string _TextureName)
{
	mpModelBuffer->pDiffuseTexture = mpGraphicsEngine->Get_Textures(_TextureName);

}

void tempObject::SetNormalMapTexture(std::string _TextureName)
{
	mpModelBuffer->pNormalTexture = mpGraphicsEngine->Get_Textures(_TextureName);

}

void tempObject::SetCubeMapTexture(std::string _DiffuseTextureName, std::string _SpecularTextureName)
{
	mpModelBuffer->pCubeMapDiffuseResourceView = mpGraphicsEngine->Get_Textures(_DiffuseTextureName);

	if (!_SpecularTextureName.empty())
	{
	mpModelBuffer->pCubeMapSpecularResourceView = mpGraphicsEngine->Get_Textures(_SpecularTextureName);
	}
}

void tempObject::SetAnimation(std::string _Aname)
{
	mpModelBuffer->pTargetAnimation = mpGraphicsEngine->Get_Animation(_Aname);
}

void tempObject::SetNextAnimation(std::string _Aname)
{
	Animation* temp = mpGraphicsEngine->Get_Animation(_Aname);
	//if (!mpModelBuffer->mpNextTargetAnimation && mpModelBuffer->mpNextTargetAnimation != temp)
	//{
	mpModelBuffer->pNextTargetAnimation = mpGraphicsEngine->Get_Animation(_Aname);
	mpModelBuffer->nextAnimationPlaytime = 0;
	//}
}

void tempObject::SetModelInfo(std::string _ModelName)
{
	mpModelBuffer->pTargetModel = mpGraphicsEngine->Get_ModelInfo(_ModelName);
}

void tempObject::SetTargetBoneIndex(int _targetBoneIndex)
{
	mTargetBoneIndex = _targetBoneIndex;
}

void tempObject::UpdateAnimationTime(float _deltaTime)
{
	if (mpModelBuffer->pTargetAnimation)
	{
		mpModelBuffer->animationPlaytime += _deltaTime;
	}
	if (mpModelBuffer->pNextTargetAnimation)
	{
		mpModelBuffer->nextAnimationPlaytime += _deltaTime;
	}
}

void tempObject::UpdatePSConstantBufferData(PSConstantBufferData& _PSConstantdata)
{
	mPSConstantBuffer = _PSConstantdata;
}

void tempObject::UpdateVSShadowConstantBufferData(Matrix _shadowView, Matrix _shadowProjection)
{
	mVSShadowConstantBufferData.ShadowView = _shadowView;
	mVSShadowConstantBufferData.ShadowProjection = _shadowProjection;
}

void tempObject::SetObjectPos(Matrix _pos)
{
	objectPos = _pos;
}

void tempObject::SetObjectRot(Matrix _Rot)
{
	objectRot = _Rot;
}

void tempObject::SetObjectScl(Matrix _Scl)
{
	objectScl = _Scl;
}

void tempObject::GetObjectTargetBoneMatrix(std::string _targetModel, std::string _targetBoneName)
{
	mpTargetBoneConstantBufferData.targrtBoneMatrix = mpGraphicsEngine->mpResourceManager->Get_TargetBoneMatrix(_targetModel, _targetBoneName).Transpose();
}

void tempObject::GetObjectTargetBoneMatrix(VSBoneConstantBufferData _targetModelBoneConstantBuffer)
{
	mpTargetBoneConstantBufferData.targrtBoneMatrix = _targetModelBoneConstantBuffer.bone[mTargetBoneIndex];
}

ModelBuffer* tempObject::GetModelBuffer()
{
	return mpModelBuffer;
}
