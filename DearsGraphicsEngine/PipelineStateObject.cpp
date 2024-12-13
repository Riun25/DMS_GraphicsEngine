#include "PipelineStateObject.h"

///-------------------------------------------------------이 밑으로 경고뜸. 나중에 확인해볼것.
// operator=가 여러 번 정의되었기 때문이다. 
// 헤더 파일에 정의된 함수가 여러 번 포함될 수 있기 때문에, 
// 함수 정의를 소스 파일로 분리하여 중복 정의를 방지하는 것이 중요하다. -by Riun

void PipelineStateObject::operator=(const PipelineStateObject& _pso)
{
	mpVertexShader = _pso.mpVertexShader;
	mpPixelShader = _pso.mpPixelShader;
	mpHullShader = _pso.mpHullShader;
	mpDomainShader = _pso.mpDomainShader;
	mpGeometryShader = _pso.mpGeometryShader;
	mpInputLayout = _pso.mpInputLayout;

	mpBlendState = _pso.mpBlendState;
	mpDepthStencilState = _pso.mpDepthStencilState;
	mpRasterizerState = _pso.mpRasterizerState;
	mpSamplerState = _pso.mpSamplerState;

	int blendFactorsize = sizeof(mBlendFactor) / sizeof(mBlendFactor[0]);
	for (int i = 0; i < blendFactorsize; i++)
	{
		mBlendFactor[i] = _pso.mBlendFactor[i];
	}

	mPrimitiveTopology = _pso.mPrimitiveTopology;
}

void PipelineStateObject::SetBlendFactor(const float blendFactor[4])
{
	int blendFactorsize = sizeof(mBlendFactor) / sizeof(mBlendFactor[0]);
	for (int i = 0; i < blendFactorsize; i++)
	{
		mBlendFactor[i] = blendFactor[i];
	}
}