#pragma once
#include<wrl.h>
#include<d3d11.h>
#include <unordered_map>
#include<string>

#include "BufferData.h"
#include "ObjectPool.h"

using Microsoft::WRL::ComPtr;

class ParticleManager
{
public:
	ParticleManager() = delete;
	ParticleManager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, unsigned int _scale);
private:
	ComPtr<ID3D11Device> mpDevice;											//����̽�
	ComPtr<ID3D11DeviceContext> mpDeviceContext;							//����̽� ���ؽ�Ʈ
	//ComPtr<ID3D11DeviceContext1> mpDeviceContext;						//����̽� ���ؽ�Ʈ
public:
	ObjectPool<CSParticleData>* mpParticleObjectpool;

	ComPtr<ID3D11Buffer> mpParticleStructedBuffer;				//��ƼŬ�� ����� structedBuffer
	ComPtr<ID3D11UnorderedAccessView> mpParticleUAV;			//��ƼŬ�� ����� UnorderedAccessView
	ComPtr<ID3D11ShaderResourceView> mpParticleSRV;			//��ƼŬ�� ����� UnorderedAccessView

	ComPtr<ID3D11Buffer> mpParticleIndexBuffer;				//��ƼŬ�� ����� IndexStructedBuffer
	ComPtr<ID3D11UnorderedAccessView> mpParticleIndexUAV;		//��ƼŬ�� ����� UnorderedAccessView
	ComPtr<ID3D11ShaderResourceView> mpParticleIndexSRV;		//��ƼŬ�� ����� UnorderedAccessView

	
	ComPtr<ID3D11Buffer> mpParticleVertexinfoBuffer;
	ComPtr<ID3D11UnorderedAccessView> mpParticleVertexInfoUAV;
	ComPtr<ID3D11ShaderResourceView> mpParticleVertexInfoSRV;

	//������¡ ���۵�
	ComPtr<ID3D11Buffer> mpParticleInfoStagingBuffer;
	ComPtr<ID3D11Buffer> mpIndexStagingBuffer;

	ComPtr<ID3D11Buffer> mpVertexBuffer;
	ComPtr<ID3D11Buffer> mpIndexBuffer;
	unsigned int mNumIndices = 0;

public:
	void Initialize();
	void CreateParticleStructedBufferandUAVandSRV();
	void CreateStagingBuffer();
	void Update();
	void Render();
	void ComputeShaderBarrier();
	void SetVertexBufferAndIndexBuffer(ComPtr<ID3D11Buffer> _vertexBuffer, ComPtr<ID3D11Buffer> _indexBuffer, unsigned int _numIndices);

public:
	void AddParticle(unsigned int _particleNum, CSParticleData& _particleData);
};

