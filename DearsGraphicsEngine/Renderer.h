#pragma once
#include<windows.h>
#include<wrl.h>
#include<d3d11.h>
#include <directxtk/SimpleMath.h>
#include "ModelBuffer.h"
#include "GraphicsResourceManager.h"
#include "RendererHelper.h"
#include "Camera.h"
#include "PipelineStateObject.h "
#include "GraphicsCommon.h"

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Quaternion;

///�������� �׷��Ƚ� ������ ������ ���������� ����ϴ� Ŭ����.
//Graphics �������� �����Ѵ�.
class Renderer
{
public:
	//������ ȣ�� �� �������ڵ�� �����ȼ�, ���ȼ��� �޴´�.
	Renderer(HWND _hWnd, int _startScreenWidth, int _startScreenHeight, int _endScreenWidth, int _endScreenHeight,
			ComPtr<ID3D11Device>& _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	~Renderer();
private:
	HWND mHWnd;				//�ڵ� ������
	int mStartScreenWidth;		//ȭ���� ����(���� �ȼ�)
	int mStartScreenHeight;		//ȭ���� ����(���� �ȼ�)
	int mEndScreenWidth;		//ȭ���� ��(���� �ȼ�)
	int mEndScreenHeight;		//ȭ���� ��(���� �ȼ�)

	int mScreenWidth;
	int mScreenHeight;

	///D3D���� �� �⺻���� ������
public:
	ComPtr<ID3D11Device> mpDevice;										//����̽�
	ComPtr<ID3D11DeviceContext> mpDeviceContext;						//����̽� ���ؽ�Ʈ


	ComPtr<IDXGISwapChain> mpSwapChain;									//����ü��

	ComPtr<ID3D11Texture2D> mpBackBuffer;								//�����, ���η���Ÿ��
	ComPtr<ID3D11ShaderResourceView> mpMainRenderTargetSRV;				//���η���Ÿ��(����)�� SRV
	ComPtr<ID3D11RenderTargetView> mpRenderTargetView;					//������ Ÿ�ٺ�  //���۵� ������ �� 2�� �̻� �ʿ�

	ComPtr<ID3D11Texture2D> mpTempBuffer;								//����� ����
	ComPtr<ID3D11ShaderResourceView> mpTempTargetSRV;					//����� ������ SRV
	ComPtr<ID3D11RenderTargetView> mpTempTargetView;					//����� ������ ����Ÿ��

	D3D11_VIEWPORT mD3dScreenViewport;								// �� ��Ʈ ������ �����ϴ� ����ü
	D3D11_VIEWPORT mD3dtempViewport;									// ����� �� ��Ʈ ����
	
	ComPtr<ID3D11DepthStencilView> mpDepthStencilView;					// ���� ���� ���ٽ� ��
	
	UINT mNumQualityLevels;

	ComPtr<ID3D11ShaderResourceView> mpCubeMapDiffuseResourceView;
	ComPtr<ID3D11ShaderResourceView> mpCubeMapSpecularResourceView;

	
public:
	GraphicsResourceManager* mpResourceManager;
	RendererHelper* mpRenderHelper;
	Camera* mpTargetCamera;

	// Depth buffer ����
	//ComPtr<ID3D11RenderTargetView> mpResolvedRTV;					//�����ʿ��� ����ϴ� ô�ϴ� ������ Ÿ�ٺ�
	//ComPtr<ID3D11Texture2D> mpDepthOnlyBuffer;  // No MSAA
	ComPtr<ID3D11DepthStencilView> mpDepthOnlyDSV; // No MSAA
	//ComPtr<ID3D11DepthStencilView> mpDepthStencilView; // use MSAA
	ComPtr<ID3D11ShaderResourceView> mpDepthOnlySRV;

	ComPtr<ID3D11Buffer> mpCommonConstantBuffer;


public:
	void SetCamera(Camera* _pTargetCamera);
public:
	bool Initialize(GraphicsResourceManager* _pResourceManager);
	void BeginRender();

	void SetCommonShaderResource(ComPtr<ID3D11ShaderResourceView> _diffuseTexture,
									ComPtr<ID3D11ShaderResourceView> _specularTexture);

	//���� ���̴� ���ҽ��� GPU�� Set���ش�
	bool SetCommonShaderResourceToGPU();


	//���� ������Ʈ�� Set���ش�.(Render ȣ������ �ʼ�)common
	void SetCommonConstant(ComPtr<ID3D11Buffer>& commonConstsBuffer);
	void Render(ModelBuffer* _modelbuffer);
	void Render_CubeMap(ModelBuffer* _modelbuffer);
	void EndRender();
	void Finalize();
	void SetPipelineState(const PipelineStateObject& _pso);

	void LightUpdate(CommonConstantBufferData* _psBufferData);
	void UpdateCommonConstantBuffer(CommonConstantBufferData& _CommonBufferData);

	void RenderDepthMap(ModelBuffer* _modelbuffer);
	void RenderAniDepthMap(ModelBuffer* _modelbuffer);
	void RenderEquipDepthMap(ModelBuffer* _modelbuffer);

	void RenderEdge(ModelBuffer* _modelbuffer);
	void RenderSampler(ModelBuffer* _modelbuffer);
	void RenderPostProcessing(ModelBuffer* _modelbuffer);

	void Rend_InstancedModels(ModelBuffer* _modelBuffers);

private:
	bool InitalizeD3D();

public:
	//mpDevice�� ������ �ӽ��Լ�
	ComPtr<ID3D11Device> GetDevice() const { return mpDevice; }
};


