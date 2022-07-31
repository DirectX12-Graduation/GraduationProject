﻿#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Scene.h"
#include "Collision.h"
#include "ParticleShader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nGraphicsSrvRootParameters, int nComputeUavRootParameters, int nComputeSrvRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextures[i] = NULL;
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextureUploadBuffers[i] = NULL;

		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pd3dUavGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dUavGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnResourceTypes[i] = -1;

		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnResourceTypes[i] = DXGI_FORMAT_UNKNOWN;
		m_pnBufferElements = new int[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnBufferElements[i] = 0;
		m_pnBufferStrides = new int[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnBufferStrides[i] = 0;
	}

	m_nGraphicsSrvRootParameters = nGraphicsSrvRootParameters;
	if (m_nGraphicsSrvRootParameters > 0)
	{
		m_pnGraphicsSrvRootParameterIndices = new int[m_nGraphicsSrvRootParameters];
		for (int i = 0; i < m_nGraphicsSrvRootParameters; i++) m_pnGraphicsSrvRootParameterIndices[i] = -1;
		m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nGraphicsSrvRootParameters];
		for (int i = 0; i < m_nGraphicsSrvRootParameters; i++) m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles[i].ptr = NULL;
	}

	m_nComputeUavRootParameters = nComputeUavRootParameters;
	if (m_nComputeUavRootParameters > 0)
	{
		m_pnComputeUavRootParameterIndices = new int[m_nComputeUavRootParameters];
		for (int i = 0; i < m_nComputeUavRootParameters; i++) m_pnComputeUavRootParameterIndices[i] = -1;
		m_pd3dComputeRootParameterUavGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nComputeUavRootParameters];
		for (int i = 0; i < m_nComputeUavRootParameters; i++) m_pd3dComputeRootParameterUavGpuDescriptorHandles[i].ptr = NULL;
	}

	m_nComputeSrvRootParameters = nComputeSrvRootParameters;
	if (m_nComputeSrvRootParameters > 0)
	{
		m_pnComputeSrvRootParameterIndices = new int[m_nComputeSrvRootParameters];
		for (int i = 0; i < m_nComputeSrvRootParameters; i++) m_pnComputeSrvRootParameterIndices[i] = -1;
		m_pd3dComputeRootParameterSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nComputeSrvRootParameters];
		for (int i = 0; i < m_nComputeSrvRootParameters; i++) m_pd3dComputeRootParameterSrvGpuDescriptorHandles[i].ptr = NULL;
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;

	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;
	if (m_pnBufferStrides) delete[] m_pnBufferStrides;

	if (m_pnGraphicsSrvRootParameterIndices) delete[] m_pnGraphicsSrvRootParameterIndices;
	if (m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles) delete[] m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles;

	if (m_pnComputeUavRootParameterIndices) delete[] m_pnComputeUavRootParameterIndices;
	if (m_pd3dComputeRootParameterUavGpuDescriptorHandles) delete[] m_pd3dComputeRootParameterUavGpuDescriptorHandles;

	if (m_pnComputeSrvRootParameterIndices) delete[] m_pnComputeSrvRootParameterIndices;
	if (m_pd3dComputeRootParameterSrvGpuDescriptorHandles) delete[] m_pd3dComputeRootParameterSrvGpuDescriptorHandles;

	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;
	if (m_pd3dUavGpuDescriptorHandles) delete[] m_pd3dUavGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetSrvGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetUavGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dUavGpuDescriptorHandle)
{
	m_pd3dUavGpuDescriptorHandles[nIndex] = d3dUavGpuDescriptorHandle;
}

void CTexture::SetGraphicsSrvRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex)
{
	m_pnGraphicsSrvRootParameterIndices[nIndex] = nRootParameterIndex;
	m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles[nIndex] = m_pd3dSrvGpuDescriptorHandles[nGpuHandleIndex];
}

void CTexture::SetComputeUavRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex)
{
	m_pnComputeUavRootParameterIndices[nIndex] = nRootParameterIndex;
	m_pd3dComputeRootParameterUavGpuDescriptorHandles[nIndex] = m_pd3dUavGpuDescriptorHandles[nGpuHandleIndex];
}

void CTexture::SetComputeSrvRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex)
{
	m_pnComputeSrvRootParameterIndices[nIndex] = nRootParameterIndex;
	m_pd3dComputeRootParameterSrvGpuDescriptorHandles[nIndex] = m_pd3dSrvGpuDescriptorHandles[nGpuHandleIndex];
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateComputeShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nComputeSrvRootParameters; i++)
	{
		if ((m_pnComputeSrvRootParameterIndices[i] != -1) && (m_pd3dComputeRootParameterSrvGpuDescriptorHandles[i].ptr != NULL))
			pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeSrvRootParameterIndices[i], m_pd3dComputeRootParameterSrvGpuDescriptorHandles[i]);
	}

	for (int i = 0; i < m_nComputeUavRootParameters; i++)
	{
		if ((m_pnComputeUavRootParameterIndices[i] != -1) && (m_pd3dComputeRootParameterUavGpuDescriptorHandles[i].ptr != NULL))
			pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeUavRootParameterIndices[i], m_pd3dComputeRootParameterUavGpuDescriptorHandles[i]);
	}
}

void CTexture::UpdateGraphicsShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nGraphicsSrvRootParameters; i++)
	{
		if ((m_pnGraphicsSrvRootParameterIndices[i] != -1) && (m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles[i].ptr != NULL)) 
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnGraphicsSrvRootParameterIndices[i], m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles[i]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnGraphicsSrvRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex, bool bIsDDSFile)
{
	if (bIsDDSFile)
		m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	/*else
		m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);*/
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, nWidth, nHeight, 1, 1, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetTexture(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;// -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	case RESOURCE_STRUCTURED_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = m_pnBufferStrides[nIndex];
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

D3D12_UNORDERED_ACCESS_VIEW_DESC CTexture::GetUnorderedAccessViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetTexture(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_UNORDERED_ACCESS_VIEW_DESC d3dUnorderedAccessViewDesc;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dUnorderedAccessViewDesc.Format = d3dResourceDesc.Format;
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		d3dUnorderedAccessViewDesc.Texture2D.MipSlice = 0;
		d3dUnorderedAccessViewDesc.Texture2D.PlaneSlice = 0;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dUnorderedAccessViewDesc.Format = d3dResourceDesc.Format;
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		d3dUnorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
		d3dUnorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dUnorderedAccessViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		d3dUnorderedAccessViewDesc.Texture2DArray.PlaneSlice = 0;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dUnorderedAccessViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		d3dUnorderedAccessViewDesc.Buffer.FirstElement = 0;
		d3dUnorderedAccessViewDesc.Buffer.NumElements = 0;
		d3dUnorderedAccessViewDesc.Buffer.StructureByteStride = 0;
		d3dUnorderedAccessViewDesc.Buffer.CounterOffsetInBytes = 0;
		d3dUnorderedAccessViewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		break;
	}
	return(d3dUnorderedAccessViewDesc);
}

void CTexture::CreateBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_pnBufferStrides[nIndex] = nStride;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, d3dHeapType, d3dResourceStates, &m_ppd3dTextureUploadBuffers[nIndex]);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial(int nTextures)
{
	SetTexture(nTextures);
}


CMaterial::~CMaterial()
{
	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;
	}

	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture* pTexture, UINT nTexture)
{
	//if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}

void CMaterial::SetTexture(int nTexture)
{
	m_nTextures = nTexture;

	m_ppTextures = new CTexture * [m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateGraphicsShaderVariables(pd3dCommandList);
	}
}

void CMaterial::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateGraphicsShaderVariables(pd3dCommandList);

		//if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList,0, 0);
	}
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseShaderVariables();
	}
}

void CMaterial::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

CShader* CMaterial::m_pWireFrameShader = NULL;
CShader* CMaterial::m_pSkinnedAnimationWireFrameShader = NULL;

void CMaterial::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pWireFrameShader = new CWireFrameShader();
	m_pWireFrameShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	m_pWireFrameShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pSkinnedAnimationWireFrameShader = new CSkinnedAnimationWireFrameShader();
	m_pSkinnedAnimationWireFrameShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	m_pSkinnedAnimationWireFrameShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
CGameObject::CGameObject()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::CGameObject(int nMaterials) : CGameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial * [m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials) delete[] m_ppMaterials;
	if (m_pSkinnedAnimationController) delete m_pSkinnedAnimationController;
}

void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this;
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader* pShader)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial(0);
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader* pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetTag(char* tagName)
{
	strcpy_s(m_pstrTag, tagName);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;
	
	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	m_xmf3Scale = XMFLOAT3(x, y, z);
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::SetWireFrameShader()
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = NULL;
	CMaterial* pMaterial = new CMaterial(0);
	pMaterial->SetWireFrameShader();
	SetMaterial(0, pMaterial);
}

void CGameObject::SetSkinnedAnimationWireFrameShader()
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = NULL;
	CMaterial* pMaterial = new CMaterial(0);
	pMaterial->SetSkinnedAnimationWireFrameShader();
	SetMaterial(0, pMaterial);
}

CSkinnedMesh* CGameObject::FindSkinnedMesh(char* pstrSkinnedMeshName)
{
	CSkinnedMesh* pSkinnedMesh = NULL;
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT::Bone_Index_Weight))
	{
		pSkinnedMesh = (CSkinnedMesh*)m_pMesh;
		if (!strcmp(pSkinnedMesh->m_pstrMeshName, pstrSkinnedMeshName)) return(pSkinnedMesh);
	}

	if (m_pSibling) if (pSkinnedMesh = m_pSibling->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);
	if (m_pChild) if (pSkinnedMesh = m_pChild->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);

	return(NULL);
}

void CGameObject::FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT::Bone_Index_Weight)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (CSkinnedMesh*)m_pMesh;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

CGameObject* CGameObject::FindFrame(const char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;

	if (!strcmp(m_pstrFrameName, pstrFrameName)) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::SetActive(char* pstrFrameName, bool bActive)
{
	CGameObject* pFrameObject = FindFrame(pstrFrameName);
	if (pFrameObject) pFrameObject->m_bActive = bActive;
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void CGameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}


XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}

//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}

//게임 객체의 로컬 x-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 y-축 방향으로 이동한다.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance); CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 z-축 방향으로 이동한다. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);
	
	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);
	
	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CGameObject::ReleaseShaderVariables()
{
	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseShaderVariables();
		}
	}
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(Signature::Graphics::object, 16, &xmf4x4World, 0);
	/*
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		if (!strcmp(m_pstrFrameName, "L_shoulder")) xmf4Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &xmf4Color, 16);
	*/
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial)
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed, CCamera* pCamrea)
{
	OnPrepareRender();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}


void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_bActive) return;

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pTexture) m_pTexture->UpdateGraphicsShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera,0);
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}

	if (m_CollisionManager)
		m_CollisionManager->Render(pd3dCommandList, pCamera);

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

UINT ReadUnsignedIntegerFromFile(FILE* pInFile)
{
	UINT nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(UINT), 1, pInFile);
	return(nValue);
}

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

int ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	int nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

CGameObject* CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader, int* pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = ::ReadIntegerFromFile(pInFile);

	CGameObject* pGameObject = new CGameObject();
	::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Transform>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(XMFLOAT4X4), 1, pInFile);

			nReads = (UINT)::fread(&pGameObject->m_xmf3Scale, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&pGameObject->m_xmf3Rotation, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&pGameObject->m_xmf3Translation, sizeof(XMFLOAT3), 1, pInFile);

		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CModelMesh* pMesh = new CModelMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			//pGameObject->m_xmBoundingBox = pMesh->m_xmBoundingBox;
			//pGameObject->MakeCollider(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

			pGameObject->SetMesh(pMesh);

			/**/pGameObject->SetWireFrameShader();
		}
		else if (!strcmp(pstrToken, "<SkinDeformations>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			CSkinnedMesh* pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinDeformationsFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pSkinnedMesh);

			/**/pGameObject->SetSkinnedAnimationWireFrameShader();
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:"))
					{
						CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader, pnSkinnedMeshes);
						if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
						TCHAR pstrDebug[256] = { 0 };
						_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
						OutputDebugString(pstrDebug);
#endif
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void CGameObject::LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets = new CAnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fStartTime = ::ReadFloatFromFile(pInFile);
			float fEndTime = ::ReadFloatFromFile(pInFile);

			pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet] = new CAnimationSet(fStartTime, fEndTime, pstrToken);
			CAnimationSet* pAnimationSet = pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet];

			::ReadStringFromFile(pInFile, pstrToken);
			if (!strcmp(pstrToken, "<AnimationLayers>:"))
			{
				pAnimationSet->m_nAnimationLayers = ::ReadIntegerFromFile(pInFile);
				pAnimationSet->m_pAnimationLayers = new CAnimationLayer[pAnimationSet->m_nAnimationLayers];

				for (int i = 0; i < pAnimationSet->m_nAnimationLayers; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<AnimationLayer>:"))
					{
						int nAnimationLayer = ::ReadIntegerFromFile(pInFile);
						CAnimationLayer* pAnimationLayer = &pAnimationSet->m_pAnimationLayers[nAnimationLayer];

						pAnimationLayer->m_nAnimatedBoneFrames = ::ReadIntegerFromFile(pInFile);

						pAnimationLayer->m_ppAnimatedBoneFrameCaches = new CGameObject * [pAnimationLayer->m_nAnimatedBoneFrames];
						pAnimationLayer->m_ppAnimationCurves = new CAnimationCurve * [pAnimationLayer->m_nAnimatedBoneFrames][9];

						pAnimationLayer->m_fWeight = ::ReadFloatFromFile(pInFile);

						for (int j = 0; j < pAnimationLayer->m_nAnimatedBoneFrames; j++)
						{
							::ReadStringFromFile(pInFile, pstrToken);
							if (!strcmp(pstrToken, "<AnimationCurve>:"))
							{
								int nCurveNode = ::ReadIntegerFromFile(pInFile); //j

								for (int k = 0; k < 9; k++) pAnimationLayer->m_ppAnimationCurves[j][k] = NULL;

								::ReadStringFromFile(pInFile, pstrToken);
								pAnimationLayer->m_ppAnimatedBoneFrameCaches[j] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);

								for (; ; )
								{
									::ReadStringFromFile(pInFile, pstrToken);
									if (!strcmp(pstrToken, "<TX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 0, pInFile);
									else if (!strcmp(pstrToken, "<TY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 1, pInFile);
									else if (!strcmp(pstrToken, "<TZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 2, pInFile);
									else if (!strcmp(pstrToken, "<RX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 3, pInFile);
									else if (!strcmp(pstrToken, "<RY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 4, pInFile);
									else if (!strcmp(pstrToken, "<RZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 5, pInFile);
									else if (!strcmp(pstrToken, "<SX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 6, pInFile);
									else if (!strcmp(pstrToken, "<SY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 7, pInFile);
									else if (!strcmp(pstrToken, "<SZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 8, pInFile);
									else if (!strcmp(pstrToken, "</AnimationCurve>"))
									{
										break;
									}
								}
							}
						}
						::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayer>
					}
				}
				::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayers>
			}
			::ReadStringFromFile(pInFile, pstrToken); //</AnimationSet>
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

CLoadedModelInfo* CGameObject::LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfo* pLoadedModel = new CLoadedModelInfo();
	pLoadedModel->m_pModelRootObject = new CGameObject();
	strcpy_s(pLoadedModel->m_pModelRootObject->m_pstrFrameName, "RootNode");

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>"))
			{
				for (; ; )
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:"))
					{
						CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
						if (pChild) pLoadedModel->m_pModelRootObject->SetChild(pChild);
					}
					else if (!strcmp(pstrToken, "</Hierarchy>"))
					{
						break;
					}
				}
			}
			else if (!strcmp(pstrToken, "<Animation>"))
			{
				CGameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
				pLoadedModel->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pLoadedModel->m_pModelRootObject, NULL);
#endif

	::fclose(pInFile);

	return(pLoadedModel);
}

void CGameObject::SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	string root = "../Assets/Model/Bounding/";
	string tail = ".txt";

	string tag = GetTag();
	string filename = "../Assets/Model/Bounding/" + tag + ".txt";
	CCollisionManager* manager = new CCollisionManager(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this , filename);
	SetCollisionManager(manager);
	UpdateTransform(nullptr);
}


/////////////////////////////////////////////////////////////////////////////////

CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed, CCamera* pCamrea)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

//////////////////////////////////////////////////////////////////////////////
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	CHeightMapGridMesh* pMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTerrainTexture = new CTexture(5, RESOURCE_TEXTURE2D, 0,1,0,0);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Terrain/Base_Texture.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/ground_base.dds", 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/ground_dry.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/ground_dirt.dds", 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/G_dry_B_dirt.dds", 4);

	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainTexture, Signature::Graphics::terrain_textures, false);

	CMaterial* pTerrainMaterial = new CMaterial(1);
	pTerrainMaterial->SetTexture(pTerrainTexture, 0);
	pTerrainMaterial->SetShader(pTerrainShader);

	SetMaterial(0, pTerrainMaterial);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(6)
{
	CSkyBoxMesh* pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 2.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1,0,0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_0.dds",0);

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, Signature::Graphics::skybox, false);

	CMaterial* pSkyBoxMaterial = new CMaterial(1);
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0,pSkyBoxMaterial);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAngrybotObject::CAngrybotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks)
{
	CLoadedModelInfo* pAngrybotModel = pModel;
	if (!pAngrybotModel) pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/Angrybot.bin", NULL);

	SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pAngrybotModel);

	strcpy_s(m_pstrFrameName, "Angrybot");

	Rotate(-90.0f, 0.0f, 0.0f);
	SetScale(0.2f, 0.2f, 0.2f);
}

CAngrybotObject::~CAngrybotObject()
{
}

////////////////////////////////////////////////////

CCoverObject::CCoverObject(int nMeshes) : CGameObject(nMeshes)
{
}

CCoverObject::~CCoverObject()
{
}

void CCoverObject::Animate(float fTimeElapsed, CCamera* pCamrea)
{
}

//

CMovingCoverObject::CMovingCoverObject(int nMeshes) : CCoverObject(nMeshes)
{
}

CMovingCoverObject::~CMovingCoverObject()
{
}

void CMovingCoverObject::SetPoints(XMFLOAT3 xmf3Center)
{
	m_xmf3Point1 = XMFLOAT3(xmf3Center.x, xmf3Center.y, xmf3Center.z - 200.0f);
	m_xmf3Point2 = XMFLOAT3(xmf3Center.x, xmf3Center.y, xmf3Center.z + 200.0f);
}

void CMovingCoverObject::Animate(float fTimeElapsed, CCamera* pCamrea)
{
	XMFLOAT3 xmf3Position = GetPosition();

	if (xmf3Position.z < m_xmf3Point1.z) {
		m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	}
	if (xmf3Position.z > m_xmf3Point2.z) {
		m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, -1.0f);
	}

	xmf3Position = Vector3::Add(xmf3Position, m_xmf3Direction, m_fSpeed * fTimeElapsed);
	SetPosition(xmf3Position);
}

//

CInteractiveCoverObject::CInteractiveCoverObject(int nMeshes) : CCoverObject(nMeshes)
{
	m_fInteractTime = 0.0f;
}

CInteractiveCoverObject::~CInteractiveCoverObject()
{
}

void CInteractiveCoverObject::Animate(float fTimeElapsed, CCamera* pCamrea)
{
	m_fInteractTime -= fTimeElapsed;
}

//////////////////////////////////////////

CCannonballObject::CCannonballObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1)
{
	CLoadedModelInfo* pCannonballModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/cannon balls.bin", NULL);
	SetChild(pCannonballModel->m_pModelRootObject, true);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Rock01.dds", 0);

	CIlluminatedShader* pShader = new CIlluminatedShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	CScene::CreateShaderResourceViews(pd3dDevice, pTexture, Signature::Graphics::texture,true);
	
	CMaterial* pSMaterial = new CMaterial(1);
	pSMaterial->SetTexture(pTexture);
	SetMaterial(0, pSMaterial);

	SetShader(pShader);

	SetTag("cannon_ball");
	SetObjectCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	SetDamage(100);

	if (pCannonballModel) delete pCannonballModel;
}

CCannonballObject::~CCannonballObject()
{
}

void CCannonballObject::Animate(float fTimeElapsed, CCamera* pCamera)
{
	if (m_bIsFired) {
		XMFLOAT3 xmf3Gravity = XMFLOAT3(0.0f, -10.0f * fTimeElapsed * 0.5, 0.0f);
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Gravity);
		//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(xmf3Gravity, fTimeElapsed * 0.5, false));
		XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed * 100, false);
		SetPosition(Vector3::Add(GetPosition(), xmf3Velocity));
	}

	// 포탄이 바닥에 충돌, 없어지면
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3Position = GetPosition();
	float fHeight = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z);
	if (xmf3Position.y <= fHeight) {
		SetFire(false);
		SetActive(false);
	}
}

bool CCannonballObject::IsReadyToFire()
{
	return m_bIsFired ? false : true;
}

void CCannonballObject::SetValues(XMFLOAT3 origin, XMFLOAT3 velocity)
{
	SetPosition(origin);
	m_xmf3Velocity = velocity;
}

//

CCannonObject::CCannonObject() : CGameObject(1)
{
}

CCannonObject::~CCannonObject()
{
}

void CCannonObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//XMFLOAT4X4 xmf4x4TempWolrd;
	//xmf4x4TempWolrd = m_xmf4x4ToParent = m_xmf4x4World;

	//m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationY(110.0f), m_xmf4x4ToParent);
	UpdateTransform(NULL);

	CGameObject::Render(pd3dCommandList, pCamera);
	if (m_pCannonball) {
		m_pCannonball->Render(pd3dCommandList, pCamera);
	}

	//m_xmf4x4World = m_xmf4x4ToParent = xmf4x4TempWolrd;
}

void CCannonObject::Animate(float fTimeElapsed, CCamera* pCamera)
{
	if (m_pCannonball) {
		m_pCannonball->Animate(fTimeElapsed, pCamera);
	}
}

void CCannonObject::RotateCannon(XMFLOAT3* pxmf3Axis, float fAngle)
{
	CGameObject* pBarrel = m_pChild->FindFrame("Cube_001");	// 포신
	pBarrel->Rotate(pxmf3Axis, fAngle);
}

void CCannonObject::FireCannonBall(XMFLOAT3 Origin, XMFLOAT3 Velocity)
{
	if (m_pCannonball->IsReadyToFire()) {
		m_pCannonball->SetValues(Origin, Velocity);
		m_pCannonball->SetFire(true);
		m_pCannonball->SetActive(true);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMonsterObject::CMonsterObject()
{
}

CMonsterObject::~CMonsterObject()
{
}

void CMonsterObject::FindTarget(CGameObject* pObject)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3TargetPosition = pObject->GetPosition();

	float distance = Vector3::Distance(xmf3Position, xmf3TargetPosition);

	if (distance < m_fDetectionRange)
		m_pTargetObject = pObject;
	else
		m_pTargetObject = NULL;

	if (m_pTargetObject && m_pNavMesh) {
		if (!m_curCell || !m_pNavMesh->PointInCell(m_curCell, xmf3Position)) {
			m_curCell = m_pNavMesh->FindCell(xmf3Position);
		}

		CCell* tarCell = m_pTargetObject->GetCurCell();
		if (!tarCell || !m_pNavMesh->PointInCell(tarCell, xmf3TargetPosition)) {
			m_pTargetObject->SetCurCell(m_pNavMesh->FindCell(xmf3TargetPosition));
			if (m_curCell && tarCell)
				if (!m_bStraight)
					MakePath();
		}

		if (m_lPath.size() == 0) {
			if (!m_curCell) return;
			if (!tarCell) return;
			if (m_curCell == tarCell) return;
			if (!m_bStraight)
				MakePath();
		}
	}
}

void CMonsterObject::CheckStraightToTarget(vector<CGameObject*> pObjects)
{
	if (!m_pTargetObject) return;
	m_bStraight = true;
	XMFLOAT3 TPos = m_pTargetObject->GetPosition();
	XMFLOAT3 MPos = GetPosition();
	XMFLOAT3 dir = Vector3::Subtract(TPos, MPos);
	dir = Vector3::Normalize(dir);
	float dist = Vector3::Length(dir);

	for (auto& obj : pObjects) {
		CCollisionManager* col = obj->GetCollisionManager();
		col->UpdateCollisions();
		BoundingBox ObjBox = col->GetBoundingBox();
		bool result = false;

		// ObjBox와 선분 MPos부터 TPos까지 충돌
		XMVECTOR origin = XMLoadFloat3(&MPos);
		XMVECTOR direction = XMLoadFloat3(&dir);
		result = ObjBox.Intersects(origin, direction, dist);

		if (result) {
			m_bStraight = false;
			break;
		}
	}
}

float CMonsterObject::ChaseTarget(float fTimeElapsed, bool bMove)
{
	if (m_pTargetObject == NULL) return 0;

	XMFLOAT3 targetPosition = m_pTargetObject->GetPosition();

	if (bMove && m_pNavMesh) {
		int TargetCellIdx = -1;
		if (m_lPath.size() > 0) {
			TargetCellIdx = m_lPath.front();
			m_lPath.pop_front();
		}

		if (m_bStraight) {
			targetPosition = m_pTargetObject->GetPosition();
		}
		else {
			if (TargetCellIdx != -1) {
				if (m_pNavMesh) {
					targetPosition = m_pNavMesh->GetCell(TargetCellIdx).center;
				}
			}

			CCell* tarCell = m_pTargetObject->GetCurCell();
			if (m_curCell == tarCell)
				targetPosition = m_pTargetObject->GetPosition();
		}
	}

	XMFLOAT3 monsterPosition = GetPosition();

	targetPosition.y = 0;
	monsterPosition.y = 0;

	XMFLOAT3 xmf3Direction = Vector3::Subtract(targetPosition, monsterPosition);

	float fYaw = 0.0f;
	float fAngle = 0.0f, fScalarTriple = 0.0f, nSign = 0.0f;

	XMFLOAT3 monsterLook = GetLook();

	fScalarTriple = Vector3::DotProduct(GetUp(), Vector3::CrossProduct(xmf3Direction, monsterLook));
	nSign = fScalarTriple < 0.0f ? 1.0f : -1.0f;
	fAngle = Vector3::Angle(xmf3Direction, monsterLook) * nSign;

	fYaw = fAngle * fTimeElapsed;
	
	Rotate(0.0f, fYaw, 0.0f);

	// 전진
	float distance = Vector3::Distance(monsterPosition, targetPosition);
	if (bMove) {
		if (distance > 200.0f)
			MoveForward(100.0f * fTimeElapsed);
	}
	return distance;
}

void CMonsterObject::MakePath()
{
	m_lPath.clear();
	m_lPath = m_pNavMesh->MakePath(m_curCell, m_pTargetObject->GetPosition());
}

void CMonsterObject::AttackTarget()
{
	if (m_fHp <= 0) return;

	int curNum = m_pSkinnedAnimationController->GetCurrentTrackNum();

	int randomNum = rand() % 2;
	int trackNum = randomNum ? track_name::attack1 : track_name::attack2;

	if (curNum == track_name::idle1 || curNum == track_name::idle2 || curNum == track_name::walk) {
		m_pSkinnedAnimationController->SwitchAnimationState(trackNum);
		m_pSkinnedAnimationController->SetAttackEnable(true);
	}
}

void CMonsterObject::MonsterDead()
{
	int curNum = m_pSkinnedAnimationController->GetCurrentTrackNum();

	if (curNum == track_name::death1 || curNum == track_name::death2) return;

	m_pSkinnedAnimationController->SwitchAnimationState(m_pSkinnedAnimationController->GetDeadNum());
	m_pSkinnedAnimationController->SetAttackEnable(false);
}

bool CMonsterObject::DecreaseHp(float val)
{
	m_fHp -= val;
	if (m_fHp <= 0)
	{
		MonsterDead();
		return true;
	}

	return false;
}

void CMonsterObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateTransform(NULL);

	CGameObject::Render(pd3dCommandList, pCamera);
}

void CMonsterObject::Animate(float fTimeElapsed, CCamera* pCamera)
{
	int curTrackNum = m_pSkinnedAnimationController->GetCurrentTrackNum();
	if (GetHp() > 0) {
		if (GetTarget() != NULL) {
			if (curTrackNum != track_name::attack1 &&
				curTrackNum != track_name::attack2) {
				ChaseTarget(fTimeElapsed);
			}
			if (curTrackNum == track_name::idle1 ||
				curTrackNum == track_name::idle2) {
				m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk);
			}
		}
		else {
			if (curTrackNum == track_name::walk) {
				m_pSkinnedAnimationController->SwitchAnimationState(track_name::idle1);
			}
		}
	}
	CGameObject::Animate(fTimeElapsed, pCamera);
}

bool CMonsterObject::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'Y':
			int trackNum = m_pSkinnedAnimationController->GetCurrentTrackNum();
			trackNum++;
			if (trackNum == track_name::length) trackNum = 0;
			m_pSkinnedAnimationController->SwitchAnimationState(trackNum);
			break;
		}
	}
	return(false);
}

CBossMonster::CBossMonster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes)
{
	CLoadedModelInfo* pBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/new Dragon_new Anim.bin", NULL);
	SetChild(pBossModel->m_pModelRootObject, true);

	CTexture* pAnimationTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pAnimationTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/Dragon.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pAnimationTexture, Signature::Graphics::animation_diffuse, true);
	m_pTexture = pAnimationTexture;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPosition(9470, 300, 27980);
	SetScale(0.01, 0.01, 0.01);
	SetHp(1000);
	SetMaxHp(1000.0f);
	SetDetectionRange(8000);
	SetDamage(30.0f);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetUpdatedContext(pTerrain);

	//
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, track_name::length, pBossModel);

	m_pSkinnedAnimationController->SetCurrentTrackNum(track_name::Idle);
	bool bTrackAnimType[track_name::length] = { true, true, true, true, true, true, true, true, true, true, true, true };
	m_pSkinnedAnimationController->SetAnimationTracks(bTrackAnimType);
	
	bool bAnimType[track_name::length] = { false, false, false, false, false, false, true, false, true, false, false, false };
	m_pSkinnedAnimationController->SetAnimationTypes(bAnimType);
	m_pSkinnedAnimationController->SetIdleNum(track_name::Idle);
	m_pSkinnedAnimationController->SetDeadNum(track_name::Die);
	
	//

	SetTag("boss");
	SetObjectCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (pBossModel) delete pBossModel;
}

CBossMonster::~CBossMonster()
{
}

void CBossMonster::Animate(float fTimeElapsed, CCamera* pCamera)
{
	int curTrackNum = m_pSkinnedAnimationController->GetCurrentTrackNum();
	FlyCoolDown -= fTimeElapsed;
	FlameCoolDown -= fTimeElapsed;
	DefendCoolDown -= fTimeElapsed;
	if (GetHp() > 0) {
		if (GetTarget() != NULL) {
			if (!bNoticed) {
				bNoticed = true;
				m_pSkinnedAnimationController->SwitchAnimationState(track_name::Scream);
			}
			else {
				float distance = ChaseTarget(fTimeElapsed, false);
				int n = rand() % 100;
				while (n >= 0 || n <= 3) {
					if (distance > 1600) {
						if (n == 2 || n == 3) {
							n = rand() % 100;
						}
						else break;
					}
					else {
						if (n == 0 || n == 1) {
							n = rand() % 100;
						}
						else break;
					}
				}

				if (curTrackNum == track_name::Idle || curTrackNum == track_name::FlyIdle) {
					switch (n) {
					case 0:
						DoAttackHand(curTrackNum);
						break;
					case 1:
						DoAttackFlame(curTrackNum);
						break;
					case 2:
						DoAttackMouth(curTrackNum);
						break;
					case 3:
						DoDefend(curTrackNum);
						break;
					case 4:
						DoTakeOff(curTrackNum);
						break;
					case 5:
						DoFlyFlame(curTrackNum);
						break;
					case 6:
						DoLand(curTrackNum);
					}
				}
			}
		}
		else {
			m_pSkinnedAnimationController->SwitchAnimationState(track_name::Idle);
		}
	}
	CGameObject::Animate(fTimeElapsed, pCamera);
}

void CBossMonster::DoAttackFlame(int curTrackNum)
{
	if (curTrackNum != track_name::Idle) return;

	if (FlameCoolDown > 0) {
		if (curTrackNum != track_name::attackFlame) return;
	}

	if (curTrackNum != track_name::attackFlame)
		m_pSkinnedAnimationController->SwitchAnimationState(track_name::attackFlame);
	FlameCoolDown = FLAME;
}

void CBossMonster::DoAttackHand(int curTrackNum)
{
	if (curTrackNum != track_name::Idle) return;

	if (curTrackNum != track_name::attackHand)
	{
		m_pSkinnedAnimationController->SwitchAnimationState(track_name::attackHand);
		m_pSkinnedAnimationController->SetAttackEnable(true);
	}
}

void CBossMonster::DoAttackMouth(int curTrackNum)
{
	if (curTrackNum != track_name::Idle) return;

	if (curTrackNum != track_name::attackMouth)
	{
		m_pSkinnedAnimationController->SwitchAnimationState(track_name::attackMouth);
		m_pSkinnedAnimationController->SetAttackEnable(true);
	}
}

void CBossMonster::DoTakeOff(int curTrackNum)
{
	if (curTrackNum != track_name::Idle) return;

	if (FlyCoolDown > 0) {
		if (curTrackNum != track_name::takeOff) return;
	}

	m_pSkinnedAnimationController->SwitchAnimationState(track_name::takeOff);
	m_pSkinnedAnimationController->SetIdleNum(track_name::FlyIdle);
	FlyCoolDown = FLY;
	bFlyAttack = false;
}

void CBossMonster::DoFlyFlame(int curTrackNum)
{
	if (curTrackNum != track_name::FlyIdle) return;

	if (FlameCoolDown > 0) {
		if (curTrackNum != track_name::FlyFlame) return;
	}

	if (curTrackNum != track_name::FlyFlame)
		m_pSkinnedAnimationController->SwitchAnimationState(track_name::FlyFlame);
	FlameCoolDown = FLAME;
	bFlyAttack = true;
}

void CBossMonster::DoLand(int curTrackNum)
{
	if (curTrackNum != track_name::FlyIdle) return;
	if (!bFlyAttack) return;	// fly 상태에서 공격 한 번은 하고 착지하도록

	m_pSkinnedAnimationController->SwitchAnimationState(track_name::Land);
	m_pSkinnedAnimationController->SetIdleNum(track_name::Idle);
}

void CBossMonster::DoDefend(int curTrackNum)
{
	if (curTrackNum != track_name::Idle) return;

	if (DefendCoolDown > 0) {
		if (curTrackNum != track_name::Defend) return;
	}

	if (curTrackNum != track_name::Defend)
		m_pSkinnedAnimationController->SwitchAnimationState(track_name::Defend);
	DefendCoolDown = DEFEND;
}

///////////////////////

CUIObject::CUIObject() : CGameObject(1)
{
	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	CGameObject::Rotate(&m_xmf3RotationAxis, 180.0f);
}

CUIObject::~CUIObject()
{
}

void CUIObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(CB_HP_INFO) + 255) & ~255);
	m_pd3dcbHpInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbHpInfo->Map(0, NULL, (void**)&m_pcbMappedHpInfo);

}

void CUIObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	CGameObject::UpdateShaderVariable(pd3dCommandList, pxmf4x4World);
	
	UpdateHpRatio();
	m_pcbMappedHpInfo->ratioHp = ratioHp;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbHpInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(Signature::Graphics::hp, d3dGpuVirtualAddress);
}

void CUIObject::UpdateHpRatio()
{
	if (m_pTargetObject == NULL) return;
	float hp = m_pTargetObject->GetHp();
	float maxhp = m_pTargetObject->GetMaxHp();

	ratioHp = hp / maxhp;
}

//////////////////////////////////////////////////////////////////////////////////////////

CStreamParticleObject::CStreamParticleObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, float fLifetime, UINT nMaxParticles) : CGameObject(1)
{
	CStreamParticleMesh* pMesh = new CStreamParticleMesh(pd3dDevice, pd3dCommandList, xmf3Position, xmf3Velocity, xmf3Acceleration, xmf3Color, xmf2Size, fLifetime, nMaxParticles);
	SetMesh(pMesh);

	m_size.x = xmf2Size.x;
	m_size.y = xmf2Size.y;

	CTexture* pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Effect/DustEffect.dds", 0);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetTexture(pParticleTexture);

	srand((unsigned)time(NULL));

	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1000];
	for (int i = 0; i < 1000; i++)
	{
		pxmf4RandomValues[i] = XMFLOAT4(RandomValue(0.0f, 1.0f), RandomValue(0.0f, 1.0f), RandomValue(0.0f, 1.0f), RandomValue(0.0f, 1.0f));
	}
	m_pRandowmValueTexture = new CTexture(1, RESOURCE_BUFFER, 0, 1, 0, 0);
	m_pRandowmValueTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1000, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 0);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CStreamParticleShader* pShader = new CStreamParticleShader();
	pShader->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, Signature::Graphics::particle_texture,true);
	CScene::CreateShaderResourceViews(pd3dDevice, m_pRandowmValueTexture, Signature::Graphics::particle_buffer,true,true);

	pMaterial->SetShader(pShader);
	SetMaterial(0,pMaterial);

	SetPosition(xmf3Position);
}

CStreamParticleObject::~CStreamParticleObject()
{
	if (m_pRandowmValueTexture) m_pRandowmValueTexture->Release();

	//if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	//if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	::CloseHandle(m_hFenceEvent);
}

void CStreamParticleObject::ReleaseUploadBuffers()
{
	if (m_pRandowmValueTexture) m_pRandowmValueTexture->ReleaseUploadBuffers();

	CGameObject::ReleaseUploadBuffers();
}

void CStreamParticleObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_ppMaterials)
	{
		if (m_ppMaterials[0]->m_pShader) m_ppMaterials[0]->m_pShader->OnPrepareRender(pd3dCommandList, 0);
		if (m_ppMaterials[0]->m_ppTextures) m_ppMaterials[0]->m_ppTextures[0]->UpdateGraphicsShaderVariables(pd3dCommandList);

		if (m_pRandowmValueTexture) m_pRandowmValueTexture->UpdateGraphicsShaderVariables(pd3dCommandList);
	}

	UpdateShaderVariable(pd3dCommandList,&m_xmf4x4World);

	m_pMesh->OnPreRender(pd3dCommandList,0, 0); //Stream Output
	m_pMesh->Render(pd3dCommandList,0, 0); //Stream Output
	m_pMesh->OnPostRender(pd3dCommandList,0, 0); //Stream Output

	if (m_ppMaterials && m_ppMaterials[0]->m_pShader) m_ppMaterials[0]->m_pShader->OnPrepareRender(pd3dCommandList,1);

	m_pMesh->OnPreRender(pd3dCommandList,0, 1); //Draw
	m_pMesh->Render(pd3dCommandList,0, 1); //Draw
}

void CStreamParticleObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(CB_HP_INFO) + 255) & ~255);
	m_pd3dcbParticleInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbParticleInfo->Map(0, NULL, (void**)&m_pcbMappedParticleInfo);

}

void CStreamParticleObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	CGameObject::UpdateShaderVariable(pd3dCommandList, pxmf4x4World);

	m_pcbMappedParticleInfo->size_x = m_size.x;
	m_pcbMappedParticleInfo->size_y = m_size.y;
	::memcpy(&m_pcbMappedParticleInfo->vec3, &m_xmf3vec, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbParticleInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(Signature::Graphics::particle, d3dGpuVirtualAddress);
}

void CStreamParticleObject::SetVector(XMFLOAT3& vec)
{
	m_xmf3vec = vec;
}

/////////////

CStreamExplosionObject::CStreamExplosionObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, float fLifetime, UINT nMaxParticles)
{
	CStreamParticleMesh* pMesh = new CStreamParticleMesh(pd3dDevice, pd3dCommandList, xmf3Position, xmf3Velocity, xmf3Acceleration, xmf3Color, xmf2Size, fLifetime, nMaxParticles);
	SetMesh(pMesh);

	m_size.x = xmf2Size.x;
	m_size.y = xmf2Size.y;

	CTexture* pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Effect/Fire.dds", 0);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetTexture(pParticleTexture);

	srand((unsigned)time(NULL));

	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1000];
	for (int i = 0; i < 1000; i++)
	{
		pxmf4RandomValues[i] = XMFLOAT4(RandomValue(0.0f,1.0f), RandomValue(0.0f, 1.0f), RandomValue(0.0f, 1.0f), RandomValue(0.0f, 1.0f));
	}
	m_pRandowmValueTexture = new CTexture(1, RESOURCE_BUFFER, 0, 1, 0, 0);
	m_pRandowmValueTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1000, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 0);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);


	CScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, Signature::Graphics::particle_texture, true);
	CScene::CreateShaderResourceViews(pd3dDevice, m_pRandowmValueTexture, Signature::Graphics::particle_buffer, true, true);

	CExplosionStreamParticleShader* pShader = new CExplosionStreamParticleShader();
	pShader->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	pMaterial->SetShader(pShader);
	SetMaterial(0, pMaterial);

	SetPosition(xmf3Position);
}

CStreamExplosionObject::~CStreamExplosionObject()
{
}

////////////////////////////

CMonsterQusetUIObject::CMonsterQusetUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CUIObject()
{
	for (int i = 0; i <= 5; ++i)
	{
		CTexture* qTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
		string str = "../Assets/Image/UI/" + to_string(i) + to_string(5) + ".dds";
		std::wstring widestr = std::wstring(str.begin(), str.end());
		const wchar_t* widecstr = widestr.c_str();

		qTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, (wchar_t*)widecstr, 0);
		CScene::CreateShaderResourceViews(pd3dDevice, qTexture, Signature::Graphics::texture, true);
		
		_textures.push_back(qTexture);
	}

	for (int i = 0; i <= 1; ++i)
	{
		CTexture* qTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
		string str = "../Assets/Image/UI/" + to_string(i) + to_string(1) + ".dds";
		std::wstring widestr = std::wstring(str.begin(), str.end());
		const wchar_t* widecstr = widestr.c_str();

		qTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, (wchar_t*)widecstr, 0);
		CScene::CreateShaderResourceViews(pd3dDevice, qTexture, Signature::Graphics::texture, true);

		_textures.push_back(qTexture);
	}

	CScreenShader* pShader = new CScreenShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);

	CMaterial* Mater = new CMaterial(1);
	Mater->SetTexture(_textures[0]);
	Mater->SetShader(pShader);
	SetMaterial(0, Mater);
}

CMonsterQusetUIObject::~CMonsterQusetUIObject()
{
}

void CMonsterQusetUIObject::setTextureNumber(int n)
{
	if (_textures.size() < n + 1) return;
	m_ppMaterials[0]->SetTexture(_textures[n]);
}

void CMonsterQusetUIObject::MonsterCount()
{
	if (currCount >= 5) return;
	setTextureNumber(++currCount);
}

void CMonsterQusetUIObject::BossCount()
{
	if (currCount == 0) currCount = 6;
	if (currCount > _textures.size()) return;
	if (currCount <= 5) return;
	setTextureNumber(++currCount);
}
