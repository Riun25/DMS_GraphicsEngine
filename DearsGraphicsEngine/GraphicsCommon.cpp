#include "GraphicsCommon.h"


namespace Dears {

	namespace Graphics {

		// Sampler States
		ComPtr<ID3D11SamplerState> gpLinearWrapSS;
		ComPtr<ID3D11SamplerState> gpLinearClampSS;
		ComPtr<ID3D11SamplerState> gpShadowPointSS;
		ComPtr<ID3D11SamplerState> gpShadowCompareSS;
		vector<ID3D11SamplerState*> gSampleStates;

		// Rasterizer States
		ComPtr<ID3D11RasterizerState> gpSolidRS;
		ComPtr<ID3D11RasterizerState> gpWireRS;

		// Depth Stencil States
		ComPtr<ID3D11DepthStencilState> gpDrawDSS;       // 일반적으로 그리기
		ComPtr<ID3D11DepthStencilState> particleDSS;   // 일반적으로 그리기 + 뒷면도 렌더


		ComPtr<ID3D11DepthStencilState> punchDSS;	   //물체의 마스크의 스탠실을 변경
		ComPtr<ID3D11DepthStencilState> drawEdgeDSS;   //tempDSS에서 변경된 stencil을 바탕으로 외곽선을 그림   

		// Shaders
		ComPtr<ID3D11VertexShader> gpBasicVS;
		ComPtr<ID3D11VertexShader> gpAnimeVS;
		ComPtr<ID3D11VertexShader> gpEquimentVS;
		ComPtr<ID3D11VertexShader> gpSkyboxVS;
		ComPtr<ID3D11VertexShader> gpDepthOnlyVS;
		ComPtr<ID3D11VertexShader> gpDepthAniOnlyVS;
		ComPtr<ID3D11VertexShader> gpDepthEquipOnlyVS;
		ComPtr<ID3D11VertexShader> gpInstanceBasicVS;
		ComPtr<ID3D11VertexShader> edgeVS;
		ComPtr<ID3D11VertexShader> testVS;
		ComPtr<ID3D11VertexShader> waterVS;
		ComPtr<ID3D11VertexShader> particleVS;
		ComPtr<ID3D11VertexShader> samplerVS;
		ComPtr<ID3D11VertexShader> postProcessingVS;


		ComPtr<ID3D11PixelShader> gpBasicPS;
		ComPtr<ID3D11PixelShader> gpSkyboxPS;
		ComPtr<ID3D11PixelShader> depthOnlyPS;
		ComPtr<ID3D11PixelShader> edgePS;
		ComPtr<ID3D11PixelShader> testPS;
		ComPtr<ID3D11PixelShader> particlePS;
		ComPtr<ID3D11PixelShader> samplerPS;
		ComPtr<ID3D11PixelShader> postProcessingPS;

		//ComputeShader
		ComPtr<ID3D11ComputeShader> gpParticleComputeShader;


		// Input Layouts
		ComPtr<ID3D11InputLayout> gpBasicIL;
		ComPtr<ID3D11InputLayout> gpAnimeIL;
		ComPtr<ID3D11InputLayout> equimentIL;
		ComPtr<ID3D11InputLayout> gpSkyboxIL;
		ComPtr<ID3D11InputLayout> gpDepthOnlyIL;
		ComPtr<ID3D11InputLayout> gpDepthAniOnlyIL;
		ComPtr<ID3D11InputLayout> gpDepthEquipOnlyIL;

		ComPtr<ID3D11InputLayout> gpInstanceBasicIL;
		ComPtr<ID3D11InputLayout> edgeIL;
		ComPtr<ID3D11InputLayout> testIL;
		ComPtr<ID3D11InputLayout> waterIL;
		
		//Instancing Layouts
		ComPtr<ID3D11InputLayout> basicInstanceIL;
		ComPtr<ID3D11InputLayout> particleIL;

		//postprocessing Layouts
		ComPtr<ID3D11InputLayout> samplerIL;
		ComPtr<ID3D11InputLayout> postProcessingIL;

		// Graphics Pipeline States
		PipelineStateObject gBasicGeometryPSO;
		PipelineStateObject gAnimeGeometryPSO;
		PipelineStateObject gEquipmentGeometryPSO;
		PipelineStateObject gDebugGeometryPSO;
		PipelineStateObject gCubeMapGeometryPSO;
		PipelineStateObject gDepthOnlyPSO;
		PipelineStateObject gDepthAniOnlyPSO;
		PipelineStateObject gDepthEquipOnlyPSO;
		PipelineStateObject gOpacityPSO;

		PipelineStateObject gBasicInstancingPSO;
		PipelineStateObject gPunchingPSO;
		PipelineStateObject gEdgePSO;
		PipelineStateObject gWaterPSO;

		PipelineStateObject gTestPSO;
		PipelineStateObject gParticlePSO;

		PipelineStateObject gSamplerPSO;
		PipelineStateObject gPostEffectPSO;

		// Blend States
		ComPtr<ID3D11BlendState> gpOpacityBS;



	} // namespace Graphics

	void Graphics::InitCommonStates(ComPtr<ID3D11Device>& device) {

		InitShaders(device);
		InitSamplers(device);
		InitRasterizerStates(device);
		InitBlendStates(device);
		InitDepthStencilStates(device);
		InitPipelineStates(device);
	}

	void Graphics::InitSamplers(ComPtr<ID3D11Device>& device) {

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&sampDesc, gpLinearWrapSS.GetAddressOf());
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		device->CreateSamplerState(&sampDesc, gpLinearClampSS.GetAddressOf());

		// shadowPointSS
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 100.0f; // 큰 Z값
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		device->CreateSamplerState(&sampDesc, gpShadowPointSS.GetAddressOf());

		// shadowCompareSS, 쉐이더 안에서는 SamplerComparisonState
		// Filter = "_COMPARISON_" 주의
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 100.0f; // 큰 Z값
		sampDesc.Filter =
			D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		device->CreateSamplerState(&sampDesc, gpShadowCompareSS.GetAddressOf());

		// 샘플러 순서가 "Common.hlsli"에서와 일관성 있어야 함
		gSampleStates.push_back(gpLinearWrapSS.Get());
		gSampleStates.push_back(gpLinearClampSS.Get());
		gSampleStates.push_back(gpShadowPointSS.Get());
		gSampleStates.push_back(gpShadowCompareSS.Get());
	}

	void Graphics::InitRasterizerStates(ComPtr<ID3D11Device>& device)
	{
		gpSolidRS = RendererHelper::CreateSolidRasterizerState(device);
		gpWireRS = RendererHelper::CreateWireRasterizerState(device);
	}

	void Graphics::InitBlendStates(ComPtr<ID3D11Device>& device)
	{
		///블렌딩 상태를 정의하는 구조체, 이 구조체를 사용하여 블렌딩 방식을 설정하고 이를 GPU에 적용한다.
		D3D11_BLEND_DESC BlendDesc;
		ZeroMemory(&BlendDesc, sizeof(BlendDesc));	//구조테를 0으로 초기화한다.
		BlendDesc.AlphaToCoverageEnable = true; // MSAA(Multi-Sample Anti Aliasing)을 사용할때 알파블렌딩을 사용할 것인가?
		BlendDesc.IndependentBlendEnable = false; //여러개의 렌더 타겟(최대 8개)에 대해 독립적인 블렌딩 설정을 사용할 것인가? false일 경우 첫번째 렌더타겟의 블렌딩 설정이 모든 렌더타겟에 적용된다.

		//렌더 타겟 설정
		BlendDesc.RenderTarget[0].BlendEnable = true;								//블렌딩을 활성화 하는 설정, true로 설정하면 GPU가 색상을 혼합하여 출력

		///FinalRGB = (SrcRGB * SrcBlend) [op] (DstRGB * DestBlend)						//Scr = Source(소스 - 현재 렌더링하려는 픽셀의 색상), Dst = Destination(대상 - 이미 렌더 타겟에 존재하는 픽셀의 색상), [op] - RenderTarget.BlendOp
		BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;			//블렌딩 시에 사용됭 소스 색상의 혼합방식을 설정, D3D11_BLEND_BLEND_FACTOR는 GPU의 블렌딩 펙터를 소스 색상의 곱셈인자로 사용한다. 블렌딩 팩터는 이후에 "OMSetBlendState"를 호출할 때 지정할 수 있다.
		BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;		//블렌딩 시에 사용될 대상 색상(랜더타겟에 이미 존재하는 색상)의 혼합방식을 설정, D3D11_BLEND_INV_BLEND_FACTOR는 블렌딩 펙터의 "보수값(1 - 블렌딩 팩터)을 곱하는 방식으로 블렌딩한다.
		BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;					//소스 색상과 대상 색상 간의 블렌딩 연산을 설정한다.D3D11_BLEND_OP_ADD는 두 색상을 더하는 연산을 수행한다. 최종 색상 = (소스 색상 * SrcBlend) + (대상 색상 * DestBlend)

		///FinalAlpha = (SrcAlpha * SrcBlendAlpha) [op] (DstAlpha* DestBlendAlpha)		//Scr = Source(소스 - 현재 렌더링하려는 픽셀의 알파값), Dst = Destination(대상 - 이미 렌더 타겟에 존재하는 픽셀의 알파값), [op] - RenderTarget.BlendOpAlpha
		BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				// 알파 값에 대해 소스 색상의 혼합 방식을 설정, D3D11_BLEND_ONE - 소스 알파값이 그대로 설정
		BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;				// 알파 값에 대해 소스 색상의 혼합 방식을 설정, D3D11_BLEND_ONE - 대상 알파 값이 그대로 사용
		BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//작은걸로

		// 필요하면 RGBA 각각에 대해서도 조절 가능
		BlendDesc.RenderTarget[0].RenderTargetWriteMask =							//RGBA 채널중 어떤 채널에 값을 쓸지를 설정
			D3D11_COLOR_WRITE_ENABLE_ALL;												//RGBA 모두에 값을 쓰도록 설정하겠다.

		ThrowIfFailed(
			device->CreateBlendState(&BlendDesc, gpOpacityBS.GetAddressOf()));
	}

	/// <summary>
	/// 뎁스 스탠실 테스트의 작동방식을 정의하는 설정
	/// </summary>
	/// <param name="device"></param>
	void Graphics::InitDepthStencilStates(ComPtr<ID3D11Device>& device)
	{
		// m_drawDSS: 기본 DSS------------------------------------------------
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.DepthEnable = true;														 // 깊이 테스트를 수행할 것인가
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;								 //	깊이 버퍼에 기록을 할 것인가? -> 모든 값 기록
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;										 // 깊이 값을 비교할 함수 -> 픽셀의 깊이가 깊이버퍼보다 작을때만 렌더
		dsDesc.StencilEnable = true;// Stencil 불필요									 //	스탠실 테스트를 할것인가? false -> 하지않겠다. 
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;						 // 스탠실 값을 읽고 쓸때 사용할 마스크
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;						 // 스탠실 값을 읽고 쓸때 사용할 마스크

		// 앞면에 대해서 어떻게 작동할지 설정												 // 스탠실은 앞면과 뒷면에 대한 스탠실 테스트를 별도로 설정할 수 있다.
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;							 // 스텐실 테스트가 실패할 경우 스텐실 값을 유지하겠다.
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;					 // 스텐실 테스트는 통과했지만 깊이 테스트가 실패했을 경우
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;							 // 스텐실 테스트와 깊이 테스트 모두 통과했을경우
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;							 // 스텐실 테스트를 항상 성공하는 설절 즉, 스텐실 값과 상관없이 항상 통과
		// 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)								
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;							 //
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;						 //
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;						 //
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;							 //

		ThrowIfFailed(
			device->CreateDepthStencilState(&dsDesc, gpDrawDSS.GetAddressOf()));

		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		ThrowIfFailed(
			device->CreateDepthStencilState(&dsDesc, particleDSS.GetAddressOf()));


		//punchDSS : 그릴려는 곳의 스탠실을 변경하는 DSS-------------------------
		D3D11_DEPTH_STENCIL_DESC punchDsDesc;
		ZeroMemory(&punchDsDesc, sizeof(punchDsDesc));
		punchDsDesc.DepthEnable = true;
		punchDsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		punchDsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		punchDsDesc.StencilEnable = true;
		punchDsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		punchDsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		// 앞면에 대해서 어떻게 작동할지 설정									
		punchDsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		punchDsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		punchDsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		punchDsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		
		// 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)					
		punchDsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		punchDsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		punchDsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		punchDsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

		ThrowIfFailed(
			device->CreateDepthStencilState(&punchDsDesc, punchDSS.GetAddressOf()));

		//drawEdgeDSS : 외곽선을 강조하는 DSS-----------------------------------
		D3D11_DEPTH_STENCIL_DESC edgeDsDesc;
		ZeroMemory(&edgeDsDesc, sizeof(edgeDsDesc));
		edgeDsDesc.DepthEnable = true;														 // 깊이 테스트를 수행할 것인가
		edgeDsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;								 //	깊이 버퍼에 기록을 할 것인가? -> 모든 값 기록
		edgeDsDesc.DepthFunc = D3D11_COMPARISON_LESS;										 // 깊이 값을 비교할 함수 -> 픽셀의 깊이가 깊이버퍼보다 작을때만 렌더
		edgeDsDesc.StencilEnable = true;													 //	스탠실 테스트를 할것인가? 
		edgeDsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;						 // 스탠실 값을 읽고 쓸때 사용할 마스크
		edgeDsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;						 // 스탠실 값을 읽고 쓸때 사용할 마스크

		// 앞면에 대해서 어떻게 작동할지 설정												
		edgeDsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;							
		edgeDsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;					
		edgeDsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		edgeDsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		// 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)								
		edgeDsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;						
		edgeDsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;					
		edgeDsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		edgeDsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;							 //뒷면은 안그린다.

		ThrowIfFailed(
			device->CreateDepthStencilState(&edgeDsDesc, drawEdgeDSS.GetAddressOf()));
	}

	///D3D11_INPUT_PER_VERTEX_DATA의 경우 해당 데이터가 정점단위로 처리됨을 나타낸다. 즉, 각 정점마다 다른 데이터를 제공하는 경우 사용한다.
	///D3D11_INPUT_PER_INSTANCE_DATA의 경우  해당 데이터가 인스턴트 단위로 처리됨을 나타낸다.
	/// 이는 한번의 드로우콜로 동일한 오브젝트를 여러 번 렌더링할 때 각 인스턴트마다 제공되는 데이터를 의미한다.
	/// 예를 들어 동일한 모델을 여러 위치에 그릴 때, 각 인스턴스마다 다른 변환 행렬을 적용할수 있다. 이런 변환 단위는 인스턴스 단위로 제공된다.
	//	typedef
	//		enum D3D11_INPUT_CLASSIFICATION
	//	{
	//		D3D11_INPUT_PER_VERTEX_DATA = 0,
	//		D3D11_INPUT_PER_INSTANCE_DATA = 1
	//	} 	D3D11_INPUT_CLASSIFICATION;

	void Graphics::InitShaders(ComPtr<ID3D11Device>& device)
	{
		// Shaders, InputLayouts
		vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> AnimeInputElements =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},				   //위치
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},				   //노말
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},				   //텍스쳐 좌표
			{"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},		   //본 인덱스
			{"BONEINDICES", 1, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},		   //본 인덱스2
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0},		   //본의 가중치
			{"BONEWEIGHTS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0}		   //본으 가중치2
		};
		//DirectX::BoundingBox& objectBounds

		vector<D3D11_INPUT_ELEMENT_DESC> skyboxInputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		vector<D3D11_INPUT_ELEMENT_DESC> EquipmentInputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		vector<D3D11_INPUT_ELEMENT_DESC> BasicInstanceElements = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
		
		vector< D3D11_INPUT_ELEMENT_DESC> dummyElement =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		std::string VSfilename = "../DearsGraphicsEngine/Shader/VertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, gpBasicVS, gpBasicIL);

		VSfilename = "../DearsGraphicsEngine/Shader/AniVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			AnimeInputElements, gpAnimeVS, gpAnimeIL);

		VSfilename = "../DearsGraphicsEngine/Shader/CubeMapVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			skyboxInputElements, gpSkyboxVS, gpSkyboxIL);

		VSfilename = "../DearsGraphicsEngine/Shader/ShadowVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, gpDepthOnlyVS, gpDepthOnlyIL);

		VSfilename = "../DearsGraphicsEngine/Shader/ShadowAniVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			AnimeInputElements, gpDepthAniOnlyVS, gpDepthAniOnlyIL);

		VSfilename = "../DearsGraphicsEngine/Shader/ShadowEquipmentVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, gpDepthEquipOnlyVS, gpDepthEquipOnlyIL);

		VSfilename = "../DearsGraphicsEngine/Shader/EquipmentVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			EquipmentInputElements, gpEquimentVS, equimentIL);

		VSfilename = "../DearsGraphicsEngine/Shader/InstanceVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			BasicInstanceElements, gpInstanceBasicVS, gpInstanceBasicIL);

		VSfilename = "../DearsGraphicsEngine/Shader/EdgeVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			BasicInstanceElements, edgeVS, edgeIL);

		VSfilename = "../DearsGraphicsEngine/Shader/TestVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, testVS, testIL);


		VSfilename = "../DearsGraphicsEngine/Shader/WaterVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, waterVS, waterIL);

		VSfilename = "../DearsGraphicsEngine/Shader/ParticleVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			dummyElement, particleVS, particleIL);

		VSfilename = "../DearsGraphicsEngine/Shader/SamplerVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, samplerVS, samplerIL);

		VSfilename = "../DearsGraphicsEngine/Shader/PostProcessingVertexShader.hlsl";
		RendererHelper::CreateVertexShaderAndInputLayout(device, VSfilename,
			basicInputElements, postProcessingVS, postProcessingIL);

		//Create PixelSahder
		std::string PSfilename = "../DearsGraphicsEngine/Shader/PixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, gpBasicPS);

		PSfilename = "../DearsGraphicsEngine/Shader/CubeMapPixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, gpSkyboxPS);

		PSfilename = "../DearsGraphicsEngine/Shader/ShadowPixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, depthOnlyPS);

		PSfilename = "../DearsGraphicsEngine/Shader/EdgePixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, edgePS);

		PSfilename = "../DearsGraphicsEngine/Shader/TestPixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, testPS);

		PSfilename = "../DearsGraphicsEngine/Shader/ParticlePixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, particlePS);

		PSfilename = "../DearsGraphicsEngine/Shader/SamplerPixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, samplerPS);

		PSfilename = "../DearsGraphicsEngine/Shader/PostProcessingPixelShader.hlsl";
		RendererHelper::CreatePixelShader(device, PSfilename, postProcessingPS);

		//Create ComputeShader
		std::string CSfilename = "../DearsGraphicsEngine/Shader/TestComputeShader.hlsl";
		RendererHelper::CreateComputeShader(device, CSfilename, gpParticleComputeShader);




		//RendererHelper::CreateGeometryShader(device, L"NormalGS.hlsl", normalGS);
	}

	void Graphics::InitPipelineStates(ComPtr<ID3D11Device>& device) {

		// BasicGeometryPSO;
		gBasicGeometryPSO.mpVertexShader = gpBasicVS;
		gBasicGeometryPSO.mpInputLayout = gpBasicIL;
		gBasicGeometryPSO.mpPixelShader = gpBasicPS;
		gBasicGeometryPSO.mpRasterizerState = gpSolidRS;
		gBasicGeometryPSO.mpDepthStencilState = gpDrawDSS;
		gBasicGeometryPSO.mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gBasicGeometryPSO.mpBlendState = gpOpacityBS;


		// DebugGeometryPSO
		gDebugGeometryPSO = gBasicGeometryPSO;
		gDebugGeometryPSO.mpRasterizerState = gpWireRS;

		// AnimeGeometryPSO
		gAnimeGeometryPSO.mpVertexShader = gpAnimeVS;
		gAnimeGeometryPSO.mpInputLayout = gpAnimeIL;
		gAnimeGeometryPSO.mpPixelShader = gpBasicPS;
		gAnimeGeometryPSO.mpRasterizerState = gpSolidRS;
		gAnimeGeometryPSO.mpDepthStencilState = gpDrawDSS;
		gAnimeGeometryPSO.mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// AnimeGeometryPSO
		gEquipmentGeometryPSO.mpVertexShader = gpEquimentVS;
		gEquipmentGeometryPSO.mpInputLayout = equimentIL;
		gEquipmentGeometryPSO.mpPixelShader = gpBasicPS;
		gEquipmentGeometryPSO.mpRasterizerState = gpSolidRS;
		gEquipmentGeometryPSO.mpDepthStencilState = gpDrawDSS;
		gEquipmentGeometryPSO.mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		gCubeMapGeometryPSO.mpVertexShader = gpSkyboxVS;
		gCubeMapGeometryPSO.mpInputLayout = gpSkyboxIL;
		gCubeMapGeometryPSO.mpPixelShader = gpSkyboxPS;
		gCubeMapGeometryPSO.mpRasterizerState = gpSolidRS;
		gCubeMapGeometryPSO.mpDepthStencilState = gpDrawDSS;
		gCubeMapGeometryPSO.mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// depthOnlyPSO
		gDepthOnlyPSO = gBasicGeometryPSO;
		gDepthOnlyPSO.mpVertexShader = gpDepthOnlyVS;
		gDepthOnlyPSO.mpPixelShader = depthOnlyPS;

		// depthAniOnlyPSO
		gDepthAniOnlyPSO = gAnimeGeometryPSO;
		gDepthAniOnlyPSO.mpVertexShader = gpDepthAniOnlyVS;
		gDepthAniOnlyPSO.mpPixelShader = depthOnlyPS;

		// depthEquipOnlyPSO
		gDepthEquipOnlyPSO = gBasicGeometryPSO;
		gDepthEquipOnlyPSO.mpVertexShader = gpDepthEquipOnlyVS;
		gDepthEquipOnlyPSO.mpPixelShader = depthOnlyPS;

		//OpacityPso
		gOpacityPSO = gBasicGeometryPSO;
		gOpacityPSO.mpBlendState = gpOpacityBS;

		//InstancingPSO
		gBasicInstancingPSO = gBasicGeometryPSO;
		gBasicInstancingPSO.mpVertexShader = gpInstanceBasicVS;
		gBasicInstancingPSO.mpInputLayout = gpInstanceBasicIL;
		
		//PunchingPSO
		gPunchingPSO = gBasicGeometryPSO;
		gPunchingPSO.mpVertexShader = gpDepthOnlyVS;
		gPunchingPSO.mpPixelShader = depthOnlyPS;
		gPunchingPSO.mpDepthStencilState = punchDSS;
		gPunchingPSO.mStencilRef = 1;

		//EdgePSO
		gEdgePSO = gBasicGeometryPSO;
		gEdgePSO.mpVertexShader = edgeVS;
		gEdgePSO.mpPixelShader = edgePS;
		gEdgePSO.mpDepthStencilState = drawEdgeDSS;
		gEdgePSO.mStencilRef = 1;

		//WaterPSO
		gWaterPSO = gBasicGeometryPSO;
		gWaterPSO.mpVertexShader = waterVS;
		gWaterPSO.mpInputLayout = waterIL;

		//TestPSO
		gTestPSO = gBasicGeometryPSO;
		gTestPSO.mpVertexShader = testVS;
		gTestPSO.mpPixelShader = testPS;

		gParticlePSO = gBasicInstancingPSO;
		gParticlePSO.mpVertexShader = particleVS;
		gParticlePSO.mpInputLayout = particleIL;
		gParticlePSO.mpPixelShader = particlePS;
		//ParticlePSO.m_pRasterizerState = wireRS;
		gParticlePSO.mpDepthStencilState = particleDSS;
		
		gSamplerPSO = gBasicGeometryPSO;
		gSamplerPSO.mpVertexShader = samplerVS;
		gSamplerPSO.mpInputLayout = samplerIL;
		gSamplerPSO.mpPixelShader = samplerPS;

		gPostEffectPSO = gBasicGeometryPSO;
		gPostEffectPSO.mpVertexShader = postProcessingVS;
		gPostEffectPSO.mpInputLayout = postProcessingIL;
		gPostEffectPSO.mpPixelShader = postProcessingPS;
	}

}