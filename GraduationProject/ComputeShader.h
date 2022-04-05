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
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	CTexture* m_pTexture = NULL;
protected:
	UINT							m_cxThreadGroups = 0;
	UINT							m_cyThreadGroups = 0;
	UINT							m_czThreadGroups = 0;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CVertComputeShader : public CComputeShader
{
public:
	CVertComputeShader();
	virtual ~CVertComputeShader();

public:
	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

class CHorzComputeShader : public CComputeShader
{
public:
	CHorzComputeShader();
	virtual ~CHorzComputeShader();

public:
	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, UINT cxThreadGroups, UINT cyThreadGroups, UINT czThreadGroups);
};