#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

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
		if ((m_pnComputeSrvRootParameterIndices[i] != -1) && (m_pd3dComputeRootParameterSrvGpuDescriptorHandles[i].ptr != NULL)) pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeSrvRootParameterIndices[i], m_pd3dComputeRootParameterSrvGpuDescriptorHandles[i]);
	}

	for (int i = 0; i < m_nComputeUavRootParameters; i++)
	{
		if ((m_pnComputeUavRootParameterIndices[i] != -1) && (m_pd3dComputeRootParameterUavGpuDescriptorHandles[i].ptr != NULL)) pd3dCommandList->SetComputeRootDescriptorTable(m_pnComputeUavRootParameterIndices[i], m_pd3dComputeRootParameterUavGpuDescriptorHandles[i]);
	}
}

void CTexture::UpdateGraphicsShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nGraphicsSrvRootParameters; i++)
	{
		if ((m_pnGraphicsSrvRootParameterIndices[i] != -1) && (m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles[i].ptr != NULL)) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnGraphicsSrvRootParameterIndices[i], m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles[i]);
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

void CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, nWidth, nHeight, 1, 1, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
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

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
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
	ID3D12Resource* pShaderResource = GetResource(nIndex);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateGraphicsShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (!m_pMaterial)
	{
		CMaterial* pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
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
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(Signature::Graphics::object, 16, &xmf4x4World, 0);

	if (m_pMaterial) pd3dCommandList->SetGraphicsRoot32BitConstants(Signature::Graphics::object, 1, &m_pMaterial->m_nReflection, 16);
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed, CCamera* pCamrea)
{
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	
	if (!m_bActive) return;

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
		}
		// 객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다. 
		UpdateShaderVariables(pd3dCommandList);
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateGraphicsShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CGameObject::CalculateBoundingBox()
{
	m_xmBoundingBox = m_ppMeshes[0]->m_xmBoundingBox;
	for (int i = 1; i < m_nMeshes; i++)BoundingBox::CreateMerged(m_xmBoundingBox, m_xmBoundingBox, m_ppMeshes[i]->m_xmBoundingBox);

	m_xmBoundingBox.Transform(m_xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
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

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

//////////////////////////////////////////////////////////////////////////////
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength);

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	long cxBlocks = (nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (nLength - 1) / czQuadsPerBlock;

#ifdef _WITH_VERTICES_AS_SCALING
	nWidth = int(nWidth * xmf3Scale.x);
	nLength = int(nLength * xmf3Scale.z);
	nBlockWidth = int(nBlockWidth * xmf3Scale.x);
	nBlockLength = int(nBlockLength * xmf3Scale.z);
#endif

	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList,
				xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTerrainTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0, 1,0,0);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/AerialTexture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/Detail_Texture_7.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/CloversTexture.dds", RESOURCE_TEXTURE2D, 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Texture/Brick02.dds", RESOURCE_TEXTURE2D, 3);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Alpha/RoadAlphaMap.dds", RESOURCE_TEXTURE2D, 4);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Alpha/CloversAlphaMap.dds", RESOURCE_TEXTURE2D, 5);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

#ifdef _WITH_TERRAIN_TESSELATION
	CTerrainTessellationShader* pTerrainShader = new CTerrainTessellationShader();
#else
	CTerrainShader* pTerrainShader = new CTerrainShader();
#endif
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 1, 6,0);
	pTerrainShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pTerrainTexture, 0, Signature::Graphics::terrain);

	CMaterial* pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);

	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTerrainShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pTerrainShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

float CHeightMapTerrain::GetHeight(float fx, float fz) //World Coordinates
{
	float fHeight = m_pHeightMapImage->GetHeight(fx, fz, m_xmf3Scale);

	return(fHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(6)
{
	CTexturedRectMesh* pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0, 1,0,0);

	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_Front_0.dds", RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_Back_0.dds", RESOURCE_TEXTURE2D, 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_Left_0.dds", RESOURCE_TEXTURE2D, 2);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_Right_0.dds", RESOURCE_TEXTURE2D, 3);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_Top_0.dds", RESOURCE_TEXTURE2D, 4);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/SkyBox_Bottom_0.dds", RESOURCE_TEXTURE2D, 5);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();

	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 1, 6,0);
	pSkyBoxShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObject, ncbElementBytes);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, Signature::Graphics::skybox);
	CMaterial* pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);

	SetMaterial(pSkyBoxMaterial);

	SetCbvGPUDescriptorHandle(pSkyBoxShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial)
			{
				if (m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0, i);
			}
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}