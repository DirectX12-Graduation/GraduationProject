#pragma once
#include "Shader.h"

class CComputeShader : public CShader
{
public:
	CComputeShader();
	virtual ~CComputeShader();

public:
	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups);

	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups);

protected:
	UINT		m_cxThreadGroups = 0;
	UINT		m_cyThreadGroups = 0;
	UINT		m_czThreadGroups = 0;
	CTexture*	m_pTexture = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CVertComputeShader : public CComputeShader
{
public:
	CVertComputeShader() {};
	virtual ~CVertComputeShader() {};

public:
	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups);
};

class CHorzComputeShader : public CComputeShader
{
public:
	CHorzComputeShader() {};
	virtual ~CHorzComputeShader() {};

public:
	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups);
};