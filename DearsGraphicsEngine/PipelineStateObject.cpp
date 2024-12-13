#include "PipelineStateObject.h"

///-------------------------------------------------------�� ������ ����. ���߿� Ȯ���غ���.
// operator=�� ���� �� ���ǵǾ��� �����̴�. 
// ��� ���Ͽ� ���ǵ� �Լ��� ���� �� ���Ե� �� �ֱ� ������, 
// �Լ� ���Ǹ� �ҽ� ���Ϸ� �и��Ͽ� �ߺ� ���Ǹ� �����ϴ� ���� �߿��ϴ�. -by Riun

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