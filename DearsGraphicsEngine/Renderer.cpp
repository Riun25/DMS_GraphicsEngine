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
	// 랜더 헬퍼 생성
	mpRenderHelper = new RendererHelper();


	CommonConstantBufferData CCBD;	//mpCommonConstantBuffer만 초기화 시키고 버릴 변수
	mpCommonConstantBuffer = RendererHelper::CreateConstantBuffer(mpDevice, CCBD);
	return true;
}

void Renderer::BeginRender()
{
	// 렌더링할 뷰포트를 설정한다. (1번에, m_pD3dScreenViewport 설정)
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // 사용할 색
	mpDeviceContext->ClearRenderTargetView(mpRenderTargetView.Get(), clearColor); // renderTargetView를 clearColor로 싹 지운다.

	float clearColor1[4] = { 0.f, 0.f, 0.f, 0.0f }; // 사용할 색
	mpDeviceContext->ClearRenderTargetView(mpTempTargetView.Get(), clearColor1); // renderTargetView를 clearColor로 싹 지운다.

	//모든 샘플러들을 공통으로 사용
	mpDeviceContext->VSSetSamplers(0, UINT(Dears::Graphics::gSampleStates.size()),
		Dears::Graphics::gSampleStates.data());
	mpDeviceContext->PSSetSamplers(0, UINT(Dears::Graphics::gSampleStates.size()),
		Dears::Graphics::gSampleStates.data());

	// 깊이-스탠실뷰 리셋
	mpDeviceContext->ClearDepthStencilView(mpDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mpDeviceContext->ClearDepthStencilView(mpDepthOnlyDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mpDeviceContext->OMSetRenderTargets(1, mpRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());


	//큐브맵 리소스를 GPU로 미리 보내놓는다.
	SetCommonShaderResourceToGPU();
}

void Renderer::SetCommonShaderResource(ComPtr<ID3D11ShaderResourceView> _diffuseTexture, ComPtr<ID3D11ShaderResourceView> _specularTexture)
{
	mpCubeMapDiffuseResourceView = _diffuseTexture;
	mpCubeMapSpecularResourceView = _specularTexture;
}

void Renderer::SetCommonConstant(ComPtr<ID3D11Buffer>& commonConstsBuffer)
{
	// 쉐이더와 일관성 유지 register(b1)***
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

	// 버텍스/인덱스 버퍼 설정
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
	// 큐브매핑
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
	// 스왑 체인 생성
	RendererHelper::CreateSwapChain(mpDevice, mHWnd, mNumQualityLevels, mEndScreenWidth, mEndScreenHeight, mpSwapChain);

	// 메인 렌더 타겟 뷰 생성
	RendererHelper::CreateRenderTargetView(mpDevice, mpSwapChain, mpRenderTargetView, mpBackBuffer);
	HRESULT hr = mpDevice->CreateShaderResourceView(mpBackBuffer.Get(), nullptr, mpMainRenderTargetSRV.GetAddressOf());

	// 기본 뷰 포트 세팅
	RendererHelper::SetViewPort(mpDeviceContext, mStartScreenWidth, mStartScreenHeight, mEndScreenWidth, mEndScreenHeight, mD3dScreenViewport);
	// 실험용 뷰 포트
	RendererHelper::SetViewPort(mpDeviceContext, 0, 0, 1920 *0.5, 1080 * 0.5, mD3dtempViewport);
	
	// 깊이 스텐실 뷰 생성
	RendererHelper::CreateDepthStencilBuffer(mpDevice, mNumQualityLevels, mEndScreenWidth, mEndScreenHeight, mpDepthStencilView);

	//실험용 렌더 타겟
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
	mpDeviceContext->OMSetBlendState(_pso.mpBlendState.Get(), _pso.mBlendFactor, 0xffffffff);		 //0xffffffff - 모든 샘플을 활성화 시키겠다.
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

	// 쉐이더와 일관성 유지 register(b1)***
	mpDeviceContext->VSSetConstantBuffers(1, 1, mpCommonConstantBuffer.GetAddressOf());
	mpDeviceContext->PSSetConstantBuffers(1, 1, mpCommonConstantBuffer.GetAddressOf());
}

void Renderer::RenderDepthMap(ModelBuffer* _modelbuffer)
{
	SetPipelineState(Dears::Graphics::gDepthOnlyPSO);
//	m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);
	
	//화면에 띄어보는, -> 대강 확인할수 있는
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthOnlyDSV.Get());
	
	mpDeviceContext->OMSetRenderTargets(0,NULL, mpDepthOnlyDSV.Get());
	//다른 렌더타겟에 그리는
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), m_depthOnlyDSV.Get());


	// 버텍스/인덱스 버퍼 설정
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

	mpDeviceContext->PSSetShaderResources(15, 1, mpDepthOnlySRV.GetAddressOf());	  //(텍스쳐의 번호(인덱스), SRV의 개수, 포인터(주소값))
}

void Renderer::RenderAniDepthMap(ModelBuffer* _modelbuffer)
{
	SetPipelineState(Dears::Graphics::gDepthAniOnlyPSO);
	//	m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	//화면에 띄어보는, -> 대강 확인할수 있는
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthOnlyDSV.Get());

	mpDeviceContext->OMSetRenderTargets(0, NULL, mpDepthOnlyDSV.Get());
	//다른 렌더타겟에 그리는
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), m_depthOnlyDSV.Get());


	// 버텍스/인덱스 버퍼 설정
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

	mpDeviceContext->PSSetShaderResources(15, 1, mpDepthOnlySRV.GetAddressOf());	  //(텍스쳐의 번호(인덱스), SRV의 개수, 포인터(주소값))
}

void Renderer::RenderEquipDepthMap(ModelBuffer* _modelbuffer)
{
	SetPipelineState(Dears::Graphics::gDepthEquipOnlyPSO);
	//	m_pDeviceContext->RSSetViewports(1, &m_pD3dtempViewport);
	mpDeviceContext->RSSetViewports(1, &mD3dScreenViewport);

	//화면에 띄어보는, -> 대강 확인할수 있는
	//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthOnlyDSV.Get());

	mpDeviceContext->OMSetRenderTargets(0, NULL, mpDepthOnlyDSV.Get());
	//다른 렌더타겟에 그리는
	//m_pDeviceContext->OMSetRenderTargets(1, m_pTempRednerTargetview.GetAddressOf(), m_depthOnlyDSV.Get());


	// 버텍스/인덱스 버퍼 설정
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

	mpDeviceContext->PSSetShaderResources(15, 1, mpDepthOnlySRV.GetAddressOf());	  //(텍스쳐의 번호(인덱스), SRV의 개수, 포인터(주소값))
}

void Renderer::RenderEdge(ModelBuffer* _modelbuffer)
{
	// 버텍스/인덱스 버퍼 설정
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
	// 버텍스/인덱스 버퍼 설정
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
	// 버텍스/인덱스 버퍼 설정
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

//인스턴스 모델의 경우 vertexConstantBuffer외에도 instanceConstantBuffer도 GPU로 보내주어야한다.
void Renderer::Rend_InstancedModels(ModelBuffer* _modelBuffers)
{
		/// 인스턴스 모델의 경우 VertexConstantBuffer 외에도 InstanceConstantBuffer도 GPU로 보내주어야 한다.

	// 1. 버텍스/인덱스 버퍼 설정
		UINT stride = sizeof(Vertex); // 정점 데이터의 크기
		UINT offset = 0;
		mpDeviceContext->IASetVertexBuffers(0, 1, _modelBuffers->pVertexBuffer.GetAddressOf(), &stride, &offset); // 정점 버퍼 설정
		mpDeviceContext->IASetIndexBuffer(_modelBuffers->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); // 인덱스 버퍼 설정

		// 3. 인스턴스 상수 버퍼 추가
		// 인스턴스 데이터가 들어있는 상수 버퍼를 설정해야 함
		mpDeviceContext->VSSetConstantBuffers(0, 1, _modelBuffers->pInstanceConstantBuffer.GetAddressOf()); // Instance Constant Buffer (인스턴스 버퍼)

		// 4. 픽셀 셰이더 상수 버퍼
		mpDeviceContext->PSSetConstantBuffers(0, 1, _modelBuffers->pPSConstantBuffer.GetAddressOf());

		// 5. 셰이더 리소스 (텍스처)
		ID3D11ShaderResourceView* pixelResources[1] =
		{
			_modelBuffers->pDiffuseTexture.Get(),
		};
		mpDeviceContext->PSSetShaderResources(0, 1, pixelResources); // 텍스처 설정

		// 6. 인스턴스 드로우 호출
		// 기존 DrawIndexed 대신 DrawIndexedInstanced를 사용하여 인스턴스를 한 번에 여러 개 렌더링
		UINT instanceCount = _modelBuffers->numInstances; // 인스턴스 개수
		mpDeviceContext->DrawIndexedInstanced(_modelBuffers->numIndices, instanceCount, 0, 0, 0);

		// 7. 뷰포트 및 렌더 타겟 설정
		//m_pDeviceContext->RSSetViewports(1, &m_pD3dScreenViewport);
		//m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), mpDepthStencilView.Get());

}

