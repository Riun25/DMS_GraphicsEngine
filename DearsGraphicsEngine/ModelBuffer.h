#pragma once
#include<wrl.h>
#include<d3d11.h>
#include "ModelInfo.h"
#include "BufferData.h"
using Microsoft::WRL::ComPtr;

// enum class AnimationPlayType
// {
// 	Loop,
// 	Once,
// };

struct ModelBuffer // 1���� �𵨿� ���� ���� ���� ��ü�� ���� ��츦 ����� ����� ����
{
	ComPtr<ID3D11Buffer> pVertexBuffer;					// ���ؽ� ����
	ComPtr<ID3D11Buffer> pIndexBuffer;					// �ε��� ����

	ComPtr<ID3D11Buffer> pVSConstantBuffer;				// ���ؽ� ������Ʈ ����
	ComPtr<ID3D11Buffer> pPSConstantBuffer;				// �ȼ� ������Ʈ ����
	ComPtr<ID3D11Buffer> pBoneConstantBuffer;				// �� ������Ʈ ����	   
	ComPtr<ID3D11Buffer> pTargetBoneConstantBuffer;		// Ÿ�� �� ������Ʈ ����	   

	ComPtr<ID3D11Buffer> pInstanceConstantBuffer;			//�ν��Ͻ��� �Ҷ� �ʿ��� �ν��Ͻ� ������Ʈ ����	   
	unsigned int numInstances = 0;								//�ν��Ͻ� ����

	ComPtr<ID3D11Buffer> pVSEdgeConstantBuffer;			// �ܰ��� ������Ʈ ����
	ComPtr<ID3D11Buffer> pPSEdgeConstantBuffer;			// �ܰ��� ������Ʈ ����

	ComPtr<ID3D11Buffer> pVSWaterConstantBuffer;			//�� ������Ʈ ���� -> �Ŀ� �� ���̵����� ������Ų��.

	ComPtr<ID3D11ShaderResourceView> pDiffuseTexture;		// �ؽ�ó�� ���̴� ���ҽ� �� ����ü
	ComPtr<ID3D11ShaderResourceView> pNormalTexture;
	ComPtr<ID3D11ShaderResourceView> pCubeMapDiffuseResourceView;
	ComPtr<ID3D11ShaderResourceView> pCubeMapSpecularResourceView;
	
	UINT offset = 0;										// �̰Ŵ� Mesh�� �ִ� �� �� ��︮�� �� ������..

	unsigned int numIndices = 0;
												
	Model* pTargetModel;

	Animation* pTargetAnimation = nullptr;						   //� �ִϸ��̼��� �����ų ������
	Animation* pNextTargetAnimation = nullptr;					   //� �ִϸ��̼��� �������� �����ų ������

	double animationPlaytime = 0;
	double nextAnimationPlaytime = 0;

// 	AnimationPlayType mAnimationPlayType = AnimationPlayType::Loop;
// 	AnimationPlayType mNextAnimationPlayType = AnimationPlayType::Loop;

	void operator=(ModelBuffer& _modelBuffer)
	{
		pVertexBuffer = _modelBuffer.pVertexBuffer;
		pIndexBuffer = _modelBuffer.pIndexBuffer;
		pVSConstantBuffer = _modelBuffer.pVSConstantBuffer;
		pPSConstantBuffer = _modelBuffer.pPSConstantBuffer;
		pBoneConstantBuffer = _modelBuffer.pBoneConstantBuffer;
		pDiffuseTexture = _modelBuffer.pDiffuseTexture;
		pNormalTexture = _modelBuffer.pNormalTexture;
		offset = _modelBuffer.offset;
	}
};


