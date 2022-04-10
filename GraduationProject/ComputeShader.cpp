#include "stdafx.h"
#include "ComputeShader.h"

CComputeShader::CComputeShader()
{
}

CComputeShader::~CComputeShader()
{
	if (m_pTexture) delete m_pTexture;
}

D3D12_SHADER_BYTECODE CComputeShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

void CComputeShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	if (pd3dRootSignature)
	{
		m_pd3dComputeRootSignature = pd3dRootSignature;
		pd3dRootSignature->AddRef();
	}

	ID3DBlob* pd3dComputeShaderBlob = NULL;

	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	D3D12_CACHED_PIPELINE_STATE d3dCachedPipelineState = { };
	D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.CS = CreateComputeShader(&pd3dComputeShaderBlob);
	d3dPipelineStateDesc.NodeMask = 0;
	d3dPipelineStateDesc.CachedPSO = d3dCachedPipelineState;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateComputePipelineState(
		&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();

	m_cxThreadGroups = cxThreadGroups;
	m_cyThreadGroups = cyThreadGroups;
	m_czThreadGroups = czThreadGroups;
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepare(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->Dispatch(m_cxThreadGroups, m_cyThreadGroups, m_czThreadGroups);
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	OnPrepare(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->Dispatch(cxThreadGroups, cyThreadGroups, czThreadGroups);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE CVertComputeShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VertBlurCS", "cs_5_1", ppd3dShaderBlob));
}

void CVertComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	CComputeShader::Dispatch(pd3dCommandList, cxThreadGroups, cyThreadGroups, czThreadGroups);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE CHorzComputeShader::CreateComputeShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "HorzBlurCS", "cs_5_1", ppd3dShaderBlob));
}

void CHorzComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups)
{
	CComputeShader::Dispatch(pd3dCommandList, cxThreadGroups, cyThreadGroups, czThreadGroups);
}