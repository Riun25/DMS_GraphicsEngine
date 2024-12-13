#include "ParticleManager.h"
#include "RendererHelper.h"
#include "GraphicsCommon.h"
#include "ModelInfo.h"
namespace particleEnum
{
	enum ParticleState
	{
		NotUse,
		Use,
	};
}

using namespace Dears;
ParticleManager::ParticleManager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, unsigned int _scale)
{
	mpDevice = _pDevice;
	mpDeviceContext = _pDeviceContext;
	mpParticleObjectpool = new ObjectPool<CSParticleData>(1024);
	mpParticleObjectpool->ChangeState(0, mpParticleObjectpool->GetSize() - 1, static_cast<unsigned int>(particleEnum::NotUse));
}

void ParticleManager::Initialize()
{
	CreateParticleStructedBufferandUAVandSRV();
	CreateStagingBuffer();
}

void ParticleManager::CreateParticleStructedBufferandUAVandSRV()
{
	CSParticleData tempParticleData;
	mpParticleStructedBuffer = RendererHelper::CreateStructuredBuffer(mpDevice, tempParticleData, mpParticleObjectpool->GetSize());
	mpParticleUAV = RendererHelper::CreateUnorderedAccessView(mpDevice, mpParticleStructedBuffer, mpParticleObjectpool->GetSize());
	mpParticleSRV = RendererHelper::CreateShaderResourceView(mpDevice, mpParticleStructedBuffer, mpParticleObjectpool->GetSize());

	unsigned int tempIndex;
	mpParticleIndexBuffer = RendererHelper::CreateStructuredBuffer(mpDevice, tempIndex, mpParticleObjectpool->GetSize());
	mpParticleIndexUAV = RendererHelper::CreateUnorderedAccessView(mpDevice, mpParticleIndexBuffer, mpParticleObjectpool->GetSize());
	mpParticleIndexSRV = RendererHelper::CreateShaderResourceView(mpDevice, mpParticleIndexBuffer, mpParticleObjectpool->GetSize());
	
	ParticleVertexInput tempVertexContantdata;
	mpParticleVertexinfoBuffer = RendererHelper::CreateStructuredBuffer(mpDevice, tempVertexContantdata, mpParticleObjectpool->GetSize());
	mpParticleVertexInfoUAV = RendererHelper::CreateUnorderedAccessView(mpDevice, mpParticleVertexinfoBuffer, mpParticleObjectpool->GetSize());
	mpParticleVertexInfoSRV = RendererHelper::CreateShaderResourceView(mpDevice, mpParticleVertexinfoBuffer, mpParticleObjectpool->GetSize());

}

void ParticleManager::CreateStagingBuffer()
{
	mpIndexStagingBuffer = RendererHelper::CreateStagingBuffer(mpDevice, mpParticleObjectpool->GetSize(),
															sizeof(unsigned int), mpParticleObjectpool->GetAvailableVec()->data());
	CSParticleData tempParticleData;
	mpParticleInfoStagingBuffer = RendererHelper::CreateStagingBuffer(mpDevice, mpParticleObjectpool->GetSize(),
													sizeof(tempParticleData), mpParticleObjectpool->GetObjectPool()->data());
}

void ParticleManager::Update()
{
	vector<CSParticleData>* temp = mpParticleObjectpool->GetObjectPool();

	RendererHelper::UpdateDataOnGPU(mpDeviceContext, mpParticleStructedBuffer, mpParticleInfoStagingBuffer,
		mpParticleObjectpool->GetObjectVecSize(), mpParticleObjectpool->GetObjectPool()->data());

	RendererHelper::UpdateDataOnGPU(mpDeviceContext, mpParticleIndexBuffer, mpIndexStagingBuffer,
		mpParticleObjectpool->GetAvailableVecMemorySize(), mpParticleObjectpool->GetAvailableVec()->data());

	// UAV�� ��ǻƮ ���̴��� ���ε�
	mpDeviceContext->CSSetUnorderedAccessViews(0, 1, mpParticleUAV.GetAddressOf(), nullptr);			//Compute Shader���� Particle ��ġ ����
	mpDeviceContext->CSSetUnorderedAccessViews(1, 1, mpParticleIndexUAV.GetAddressOf(), nullptr);		//Compute Shader���� Particle ��ġ ����
	mpDeviceContext->CSSetUnorderedAccessViews(2, 1, mpParticleVertexInfoUAV.GetAddressOf(), nullptr);		//Compute Shader���� Particle ��ġ ����
	//m_pDeviceContext->CSSetShaderResources(0, 1, m_pParticleSRV.GetAddressOf());

	// Dispatch ȣ���Ͽ� ��ǻƮ ���̴� ����
	const UINT threadsPerGroup = 256;
	const UINT totalParticles = 1024;
	UINT dispatchCount = (totalParticles + threadsPerGroup - 1) / threadsPerGroup;
	mpDeviceContext->CSSetShader(Dears::Graphics::gpParticleComputeShader.Get(), nullptr, 0);		//[������ ��ǻƮ ���̴�, HLSLŬ���� �ν��Ͻ��� ����� ��� �̸� �����ϴ� ������ �迭, �迭�� ���Ե� Ŭ���� �ν��Ͻ��� ����]
	mpDeviceContext->Dispatch(dispatchCount, 1, 1);

	// UAV ����
	ComputeShaderBarrier();

	//�ε��������� ������ ������¡ ���۷� �����Ѵ�.
	mpDeviceContext->CopyResource(mpIndexStagingBuffer.Get(), mpParticleIndexBuffer.Get());
	RendererHelper::CopyFromStagingBuffer(mpDeviceContext, mpIndexStagingBuffer, mpParticleObjectpool->GetAvailableVecMemorySize(), mpParticleObjectpool->GetAvailableVec()->data());

	mpDeviceContext->CopyResource(mpParticleInfoStagingBuffer.Get(), mpParticleStructedBuffer.Get());
	RendererHelper::CopyFromStagingBuffer(mpDeviceContext, mpParticleInfoStagingBuffer, mpParticleObjectpool->GetObjectVecSize(), mpParticleObjectpool->GetObjectPool()->data());


}

void ParticleManager::Render()
{
	auto _pso = Dears::Graphics::gParticlePSO;
	mpDeviceContext->VSSetShader(_pso.mpVertexShader.Get(), 0, 0);
	mpDeviceContext->PSSetShader(_pso.mpPixelShader.Get(), 0, 0);
	mpDeviceContext->IASetInputLayout(_pso.mpInputLayout.Get());
	mpDeviceContext->RSSetState(_pso.mpRasterizerState.Get());
	mpDeviceContext->OMSetBlendState(_pso.mpBlendState.Get(), _pso.mBlendFactor, 0xffffffff);		 //0xffffffff - ��� ������ Ȱ��ȭ ��Ű�ڴ�.
	mpDeviceContext->OMSetDepthStencilState(_pso.mpDepthStencilState.Get(), _pso.mStencilRef);
	mpDeviceContext->IASetPrimitiveTopology(_pso.mPrimitiveTopology);


	UINT stride = sizeof(Vertex); // ���� �������� ũ��
	UINT offset = 0;

	mpDeviceContext->IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &stride, &offset); // ���� ���� ����
	mpDeviceContext->IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); // �ε��� ���� ����
	UINT instanceCount = mpParticleObjectpool->GetSize(); // �ν��Ͻ� ����



	mpDeviceContext->VSSetShaderResources(0, 1, mpParticleVertexInfoSRV.GetAddressOf());
	mpDeviceContext->VSSetShaderResources(1, 1, mpParticleIndexSRV.GetAddressOf());
	mpDeviceContext->DrawIndexedInstanced(mNumIndices, instanceCount, 0, 0, 0);


}

/// <summary>
/// ���� ���� ��ǻƮ ���̴��� �׷��� ���̴��� ���ο� ���ҽ��� �Ҵ��� �� ���� UAV�� SRV�� ������ ���� �ʵ��� �ϱ� ���� ������� ��ġ
/// </summary>
void ParticleManager::ComputeShaderBarrier() {

	// ����: BreadcrumbsDirectX-Graphics-Samples (DX12)
	// void CommandContext::InsertUAVBarrier(GpuResource & Resource, bool FlushImmediate)

	// �ִ� ����ϴ� SRV, UAV ������ n��, ������ 6���� �Ѵ�.
	ID3D11ShaderResourceView* nullSRV[6] = {
		0,
	};
	mpDeviceContext->CSSetShaderResources(0, 6, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[6] = {
		0,
	};
	mpDeviceContext->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
}

void ParticleManager::SetVertexBufferAndIndexBuffer(ComPtr<ID3D11Buffer> _vertexBuffer, ComPtr<ID3D11Buffer> _indexBuffer, unsigned int _numIndices)
{
	mpVertexBuffer = _vertexBuffer;
	mpIndexBuffer = _indexBuffer;
	mNumIndices = _numIndices;
}

void ParticleManager::AddParticle(unsigned int _particleNum, CSParticleData& _particleData)
{
	unsigned int particleNum = _particleNum;
	std::vector<unsigned int>* availalveVec = mpParticleObjectpool->GetAvailableVec();
	std::vector<CSParticleData>* ObjectPoolVec = mpParticleObjectpool->GetObjectPool();

	for (unsigned int i = 0; i < mpParticleObjectpool->GetSize()&& particleNum > 0 ; i++)
	{
		if ((*availalveVec)[i] == particleEnum::NotUse)
		{
			(*ObjectPoolVec)[i] = _particleData;
			(*availalveVec)[i] = particleEnum::Use;
			particleNum--;
		}
	}
}
