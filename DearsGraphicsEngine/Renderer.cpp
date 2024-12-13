#pragma once
#include "Renderer.h"
#include <directxtk/SimpleMath.h>
using namespace Dears;

Renderer::Renderer(HWND _hWnd, int _startScreenWidth, int _startScreenHeight, int _endScreenWidth, int _endScreenHeight,
				ComPtr<ID3D11Device>& _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	mHWnd = _hWnd;
	mStartScreenWidth = _startScreenWidth;
	mStartScreenHeight = _startScreenHeight;
	mEndScreenWidth = _endScreenWidth;
	mEndScreenHeight = _endScreenHeight;
	mNumQualityLevels = 1;
	mpRenderHelper = nullptr;
	mpResourceManager = nullptr;
	mpDevice = _pDevice;
	mpDeviceContext = _pDeviceContext;

	mScreenWidth = mEndScreenWidth - mStartScreenWidth;
	mScreenHeight = mEndScreenHeight - mStartScreenHeight;
}

Renderer::~Renderer()
{
	delete mpTargetCamera;
	//delete m_pResourceManager;
	delete mpRenderHelper;
}

void Renderer::SetCamera(Camera* _pTargetCamera)
{
	mpTargetCamera = _pTargetCamera;
}

bool Renderer::Initialize(GraphicsResourceManager* _pResourceManager)
{
	if (!InitalizeD3D())
	{
		return false;
	}
	mpResourceManager = _pResourceManager;
	// ���� ���� ����
	mpRenderHelper = new RendererHelper();


	CommonConstantBufferData CCBD;	//mpCommonConstantBuffer�� �ʱ�ȭ ��Ű�� ���� ����
	mpCommonConstantBuffer = RendererHelper::CreateConstantBuffer(mpDevice, CCBD);
	return true;
}

void Renderer::BeginRender()
{
	// �������� ����Ʈ�� �����Ѵ�. (1����, m_pD3dScreenViewport ����)
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // ����� ��
	mpDeviceContext->ClearRenderTargetView(mpRenderTargetView.Get(), clearColor); // renderTargetView�� clearColor�� �� �����.

	float clearColor1[4] = { 0.f, 0.f, 0.f, 0.0f }; // ����� ��
	mpDeviceContext->ClearRenderTargetView(mpTempTargetView.Get(), clearColor1); // renderTargetView�� clearColor�� �� �����.

	//��� ���÷����� �������� ���
	mpDeviceContext->VSSetSamplers(0, UINT(Dears::Graphics::gSampleStates.size()),
		Dears::Graphics::gSampleStates.data());
	mpDeviceContext->PSSetSamplers(0, UINT(Dears::Graphics::gSampleStates.size()),
		Dears::Graphics::gSampleStates.data());

	// ����-���ĽǺ� ����
	mpDeviceContext->ClearDepthStencilView(mpDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mpDeviceContext->ClearDepthStencilView(mpDepthOnlyDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());


	//ť��� ���ҽ��� GPU�� �̸� �������´�.
	SetCommonShaderResourceToGPU();
}

void Renderer::SetCommonShaderResource(ComPtr<ID3D11ShaderResourceView> _diffuseTexture, ComPtr<ID3D11ShaderResourceView> _specularTexture)
{
	mpCubeMapDiffuseResourceView = _diffuseTexture;
	mpCubeMapSpecularResourceView = _specularTexture;
}

void Renderer::SetCommonConstant(ComPtr<ID3D11Buffer>& commonConstsBuffer)
{
	// ���̴��� �ϰ��� ���� register(b1)***
	mpDeviceContext->VSSetConstantBuffers(1, 1, commonConstsBuffer.GetAddressOf());
	mpDeviceContext->PSSetConstantBuffers(1, 1, commonConstsBuffer.GetAddressOf());
}

bool Renderer::SetCommonShaderResourceToGPU()
{
	if (!mpCubeMapDiffuseResourceView || !mpCubeMapSpecularResourceView)
	{
		DEBUG_LOG("ERROR - SetCommonShaderResource()");
		return false;
	}
	ID3D11ShaderResourceView* pixelResources[2] =
	{
		mpCubeMapDiffuseResourceView.Get(),
		mpCubeMapSpecularResourceView.Get(),
	};
	mpDeviceContext->PSSetShaderResources(10, 2, pixelResources);
	return true;
}

void Renderer::Render(ModelBuffer* _modelbuffer)
{

	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	mpDeviceContext->VSSetConstantBuffers(0, 1, _modelbuffer->pVSConstantBuffer.GetAddressOf());
	mpDeviceContext->VSSetConstantBuffers(2, 1, _modelbuffer->pBoneConstantBuffer.GetAddressOf());
	mpDeviceContext->VSSetConstantBuffers(3, 1, _modelbuffer->pTargetBoneConstantBuffer.GetAddressOf());
	mpDeviceContext->VSSetConstantBuffers(5, 1, _modelbuffer->pVSWaterConstantBuffer.GetAddressOf());

	mpDeviceContext->PSSetConstantBuffers(0, 1, _modelbuffer->pPSConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* pixelResources[1] = 
	{
		_modelbuffer->pDiffuseTexture.Get(),
	};
	mpDeviceContext->PSSetShaderResources(0, 1, pixelResources);
	
	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);
	
	//m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), NULL);
	//m_pDeviceContext->DrawIndexed(_modelbuffer->mNumIndices, 0, 0);

	//m_pDeviceContext->RSSetViewports(1, &m_pD3dScreenViewport);
	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());

}

void Renderer::Render_CubeMap(ModelBuffer* _modelbuffer)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	// ť�����
	mpDeviceContext->IASetVertexBuffers(
		0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride,
		&offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	mpDeviceContext->VSSetConstantBuffers(
		0, 1, _modelbuffer->pVSConstantBuffer.GetAddressOf());
	ID3D11ShaderResourceView* views[2] = {
	_modelbuffer->pCubeMapDiffuseResourceView.Get(),
	_modelbuffer->pCubeMapSpecularResourceView.Get()
	};
	mpDeviceContext->PSSetShaderResources(0, 2, views);
	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);

}

void Renderer::EndRender()
{
	HRESULT hr = (mpSwapChain->Present(0, 0));
	if (FAILED(hr))
	{
		LPWSTR output;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&output, 0,
			NULL);
		MessageBox(NULL, output, L"EndRender Error", MB_OK);
	}
}

void Renderer::Finalize()
{

}

bool Renderer::InitalizeD3D()
{
	// ���� ü�� ����
	RendererHelper::CreateSwapChain(mpDevice, mHWnd, mNumQualityLevels, mEndScreenWidth, mEndScreenHeight, mpSwapChain);

	// ���� ���� Ÿ�� �� ����
	RendererHelper::CreateRenderTargetView(mpDevice, mpSwapChain, mpRenderTargetView, mpBackBuffer);
	HRESULT hr = mpDevice->CreateShaderResourceView(mpBackBuffer.Get(), nullptr, mpMainRenderTargetSRV.GetAddressOf());

	// �⺻ �� ��Ʈ ����
	RendererHelper::SetViewPort(mpDeviceContext, mStartScreenWidth, mStartScreenHeight, mEndScreenWidth, mEndScreenHeight, mD3dScreenViewport);
	// ����� �� ��Ʈ
	RendererHelper::SetViewPort(mpDeviceContext, 0, 0, 1920 *0.5, 1080 * 0.5, mD3dtempViewport);
	
	// ���� ���ٽ� �� ����
	RendererHelper::CreateDepthStencilBuffer(mpDevice, mNumQualityLevels, mEndScreenWidth, mEndScreenHeight, mpDepthStencilView);

	//����� ���� Ÿ��
	RendererHelper::CreateRenderTargetView(mpDevice, mpSwapChain, mpTempTargetView, mpTempBuffer, 1, 1920 * 0.5, 1080 * 0.5);
	hr = mpDevice->CreateShaderResourceView(mpTempBuffer.Get(), nullptr, mpTempTargetSRV.GetAddressOf());

	RendererHelper::CreateDepthOnlyBuffer(mpDevice, mScreenWidth, mScreenHeight, mpDepthOnlyDSV, mpDepthOnlySRV);
	return true;
}

void Renderer::SetPipelineState(const PipelineStateObject& _pso)
{
	mpDeviceContext->VSSetShader(_pso.mpVertexShader.Get(), 0, 0);
	mpDeviceContext->PSSetShader(_pso.mpPixelShader.Get(), 0, 0);
	mpDeviceContext->IASetInputLayout(_pso.mpInputLayout.Get());
	mpDeviceContext->RSSetState(_pso.mpRasterizerState.Get());
	mpDeviceContext->OMSetBlendState(_pso.mpBlendState.Get(), _pso.mBlendFactor, 0xffffffff);		 //0xffffffff - ��� ������ Ȱ��ȭ ��Ű�ڴ�.
	mpDeviceContext->OMSetDepthStencilState(_pso.mpDepthStencilState.Get(), _pso.mStencilRef);
	mpDeviceContext->IASetPrimitiveTopology(_pso.mPrimitiveTopology);
	//m_pDeviceContext->PSSetSamplers(0, 1, _pso.m_pSamplerState.GetAddressOf());
}

void Renderer::LightUpdate(CommonConstantBufferData* _psBufferData)
{
	
}

void Renderer::UpdateCommonConstantBuffer(CommonConstantBufferData& _CommonBufferData)
{
	RendererHelper::UpdateBuffer(mpDeviceContext, _CommonBufferData, mpCommonConstantBuffer);

	// ���̴��� �ϰ��� ���� register(b1)***
	mpDeviceContext->VSSetConstantBuffers(1, 1, mpCommonConstantBuffer.GetAddressOf());
	mpDeviceContext->PSSetConstantBuffers(1, 1, mpCommonConstantBuffer.GetAddressOf());
}

void Renderer::RenderDepthMap(ModelBuffer* _modelbuffer)
{
	SetPipelineState(Dears::Graphics::gDepthOnlyPSO);
//	m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);
	
	//ȭ�鿡 ����, -> �밭 Ȯ���Ҽ� �ִ�
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthOnlyDSV.Get());
	
	mpDeviceContext->OMSetRenderTargets(0,NULL, mpDepthOnlyDSV.Get());
	//�ٸ� ����Ÿ�ٿ� �׸���
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), m_depthOnlyDSV.Get());


	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	mpDeviceContext->VSSetConstantBuffers(0, 1, _modelbuffer->pVSConstantBuffer.GetAddressOf());

	mpDeviceContext->PSSetConstantBuffers(0, 1, _modelbuffer->pPSConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* pixelResources[1] =
	{
		_modelbuffer->pDiffuseTexture.Get(),
	};
	mpDeviceContext->PSSetShaderResources(0, 1, pixelResources);


	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);


	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());
	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());

	mpDeviceContext->PSSetShaderResources(15, 1, mpDepthOnlySRV.GetAddressOf());	  //(�ؽ����� ��ȣ(�ε���), SRV�� ����, ������(�ּҰ�))
}

void Renderer::RenderAniDepthMap(ModelBuffer* _modelbuffer)
{
	SetPipelineState(Dears::Graphics::gDepthAniOnlyPSO);
	//	m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	//ȭ�鿡 ����, -> �밭 Ȯ���Ҽ� �ִ�
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthOnlyDSV.Get());

	mpDeviceContext->OMSetRenderTargets(0, NULL, mpDepthOnlyDSV.Get());
	//�ٸ� ����Ÿ�ٿ� �׸���
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), m_depthOnlyDSV.Get());


	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	mpDeviceContext->VSSetConstantBuffers(0, 1, _modelbuffer->pVSConstantBuffer.GetAddressOf());
	mpDeviceContext->VSSetConstantBuffers(2, 1, _modelbuffer->pBoneConstantBuffer.GetAddressOf());

	mpDeviceContext->PSSetConstantBuffers(0, 1, _modelbuffer->pPSConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* pixelResources[1] =
	{
		_modelbuffer->pDiffuseTexture.Get(),
	};
	mpDeviceContext->PSSetShaderResources(0, 1, pixelResources);


	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);


	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);
	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());

	mpDeviceContext->PSSetShaderResources(15, 1, mpDepthOnlySRV.GetAddressOf());	  //(�ؽ����� ��ȣ(�ε���), SRV�� ����, ������(�ּҰ�))
}

void Renderer::RenderEquipDepthMap(ModelBuffer* _modelbuffer)
{
	SetPipelineState(Dears::Graphics::gDepthEquipOnlyPSO);
	//	m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	//ȭ�鿡 ����, -> �밭 Ȯ���Ҽ� �ִ�
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthOnlyDSV.Get());

	mpDeviceContext->OMSetRenderTargets(0, NULL, mpDepthOnlyDSV.Get());
	//�ٸ� ����Ÿ�ٿ� �׸���
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), m_depthOnlyDSV.Get());


	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	mpDeviceContext->VSSetConstantBuffers(0, 1, _modelbuffer->pVSConstantBuffer.GetAddressOf());
	mpDeviceContext->VSSetConstantBuffers(3, 1, _modelbuffer->pTargetBoneConstantBuffer.GetAddressOf());

	mpDeviceContext->PSSetConstantBuffers(0, 1, _modelbuffer->pPSConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* pixelResources[1] =
	{
		_modelbuffer->pDiffuseTexture.Get(),
	};
	mpDeviceContext->PSSetShaderResources(0, 1, pixelResources);


	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);


	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());
	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());

	mpDeviceContext->PSSetShaderResources(15, 1, mpDepthOnlySRV.GetAddressOf());	  //(�ؽ����� ��ȣ(�ε���), SRV�� ����, ������(�ּҰ�))
}

void Renderer::RenderEdge(ModelBuffer* _modelbuffer)
{
	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	mpDeviceContext->VSSetConstantBuffers(0, 1, _modelbuffer->pVSConstantBuffer.GetAddressOf());
	mpDeviceContext->VSSetConstantBuffers(4, 1, _modelbuffer->pVSEdgeConstantBuffer.GetAddressOf());
	mpDeviceContext->PSSetConstantBuffers(2, 1, _modelbuffer->pPSEdgeConstantBuffer.GetAddressOf());
	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);


}

void Renderer::RenderSampler(ModelBuffer* _modelbuffer)
{
	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	mpDeviceContext->RSSetViewports(1, &mD3dtempViewport);

	mpDeviceContext->OMSetRenderTargets(1, mpTempTargetView.GetAddressOf(), nullptr);
	mpDeviceContext->PSSetShaderResources(0, 1, mpMainRenderTargetSRV.GetAddressOf());

	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);
 
// 	m_pDeviceContext->RSSetViewports(1, &m_pD3dScreenViewport);
// 	m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
// 	m_pDeviceContext->PSSetShaderResources(0, 1, mpTempTargetSRV.GetAddressOf());
// 	m_pDeviceContext->DrawIndexed(_modelbuffer->mNumIndices, 0, 0);
}

/////
void Renderer::RenderPostProcessing(ModelBuffer* _modelbuffer)
{
	// ���ؽ�/�ε��� ���� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mpDeviceContext->IASetVertexBuffers(0, 1, _modelbuffer->pVertexBuffer.GetAddressOf(), &stride, &offset);
	mpDeviceContext->IASetIndexBuffer(_modelbuffer->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), nullptr);
	mpDeviceContext->PSSetShaderResources(0, 1, mpTempTargetSRV.GetAddressOf());

	mpDeviceContext->DrawIndexed(_modelbuffer->numIndices, 0, 0);

// 	m_pDeviceContext->RSSetViewports(1, &m_pD3dScreenViewport);
// 	m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
// 	m_pDeviceContext->PSSetShaderResources(0, 1, mpTempTargetSRV.GetAddressOf());
// 	m_pDeviceContext->DrawIndexed(_modelbuffer->mNumIndices, 0, 0);

}

//�ν��Ͻ� ���� ��� vertexConstantBuffer�ܿ��� instanceConstantBuffer�� GPU�� �����־���Ѵ�.
void Renderer::Rend_InstancedModels(ModelBuffer* _modelBuffers)
{
		/// �ν��Ͻ� ���� ��� VertexConstantBuffer �ܿ��� InstanceConstantBuffer�� GPU�� �����־�� �Ѵ�.

	// 1. ���ؽ�/�ε��� ���� ����
		UINT stride = sizeof(Vertex); // ���� �������� ũ��
		UINT offset = 0;
		mpDeviceContext->IASetVertexBuffers(0, 1, _modelBuffers->pVertexBuffer.GetAddressOf(), &stride, &offset); // ���� ���� ����
		mpDeviceContext->IASetIndexBuffer(_modelBuffers->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); // �ε��� ���� ����

		// 3. �ν��Ͻ� ��� ���� �߰�
		// �ν��Ͻ� �����Ͱ� ����ִ� ��� ���۸� �����ؾ� ��
		mpDeviceContext->VSSetConstantBuffers(0, 1, _modelBuffers->pInstanceConstantBuffer.GetAddressOf()); // Instance Constant Buffer (�ν��Ͻ� ����)

		// 4. �ȼ� ���̴� ��� ����
		mpDeviceContext->PSSetConstantBuffers(0, 1, _modelBuffers->pPSConstantBuffer.GetAddressOf());

		// 5. ���̴� ���ҽ� (�ؽ�ó)
		ID3D11ShaderResourceView* pixelResources[1] =
		{
			_modelBuffers->pDiffuseTexture.Get(),
		};
		mpDeviceContext->PSSetShaderResources(0, 1, pixelResources); // �ؽ�ó ����

		// 6. �ν��Ͻ� ��ο� ȣ��
		// ���� DrawIndexed ��� DrawIndexedInstanced�� ����Ͽ� �ν��Ͻ��� �� ���� ���� �� ������
		UINT instanceCount = _modelBuffers->numInstances; // �ν��Ͻ� ����
		mpDeviceContext->DrawIndexedInstanced(_modelBuffers->numIndices, instanceCount, 0, 0, 0);

		// 7. ����Ʈ �� ���� Ÿ�� ����
		//m_pDeviceContext->RSSetViewports(1, &m_pD3dScreenViewport);
		//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());

}

