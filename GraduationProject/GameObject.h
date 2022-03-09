﻿#pragma once
#include "Mesh.h"
#include "Camera.h"


#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05
#define RESOURCE_STRUCTURED_BUFFER	0x06

class CShader;

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	UINT m_nMaterial;
};

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nGraphicsSrvRootParameters, int nComputeUavRootParameters, int nComputeSrvRootParameters);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;
	int* m_pnBufferStrides = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dUavGpuDescriptorHandles = NULL;

	int								m_nGraphicsSrvRootParameters = 0;
	int* m_pnGraphicsSrvRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles = NULL;

	int								m_nComputeUavRootParameters = 0;
	int* m_pnComputeUavRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dComputeRootParameterUavGpuDescriptorHandles = NULL;

	int								m_nComputeSrvRootParameters = 0;
	int* m_pnComputeSrvRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dComputeRootParameterSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateComputeShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateGraphicsShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	void CreateBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT dxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);

	int GetGraphicsSrvRootParameters() { return(m_nGraphicsSrvRootParameters); }
	void SetGraphicsSrvRootParameters(UINT nRootParameterStartIndex);
	void SetGraphicsSrvRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex);
	int GetGraphicsSrvRootParameterIndex(int nIndex) { return(m_pnGraphicsSrvRootParameterIndices[nIndex]); }

	void SetSrvGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }

	int GetComputeSrvRootParameters() { return(m_nComputeSrvRootParameters); }
	void SetComputeSrvRootParameters(UINT nRootParameterStartIndex);
	void SetComputeSrvRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex);
	int GetComputeSrvRootParameterIndex(int nIndex) { return(m_pnComputeSrvRootParameterIndices[nIndex]); }

	int GetComputeUavRootParameters() { return(m_nComputeUavRootParameters); }
	void SetComputeUavRootParameters(UINT nRootParameterStartIndex);
	void SetComputeUavRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex);
	int GetComputeUavRootParameterIndex(int nIndex) { return(m_pnComputeUavRootParameterIndices[nIndex]); }

	void SetUavGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dUavGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavGpuDescriptorHandle(int nIndex) { return(m_pd3dUavGpuDescriptorHandles[nIndex]); }

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC GetUnorderedAccessViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;
	CTexture* m_pTexture = NULL;
	CShader* m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture* pTexture);
	void SetShader(CShader* pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGameObject
{
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

public:
	XMFLOAT4X4 m_xmf4x4World;

	//게임 객체는 여러 개의 메쉬를 포함하는 경우 게임 객체가 가지는 메쉬들에 대한 포인터와 그 개수이다.
	CMesh **m_ppMeshes = NULL;
	int m_nMeshes = 0;

	CMaterial* m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	bool							m_bActive = true;

	BoundingBox						m_xmBoundingBox;

	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
protected:

public:
	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	void SetMaterial(CMaterial* pMaterial);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	// 상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList 
	*pd3dCommandList);
	// 상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
	virtual void OnPrepareRender() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	
	//virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	void ReleaseUploadBuffers();

// 게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

// 게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

// 게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

// 게임 객체를 회전(x-축, y-축, z-축)한다. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void  Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetActive(bool bActive) { m_bActive = bActive; }
	void CalculateBoundingBox();
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 높이 맵을 사용한 실외 지형을 표현하는 게임 객체
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage* m_pHeightMapImage;

	int							m_nWidth; //Width of HeightMapImage
	int							m_nLength; //Height of HeightMapImage

	XMFLOAT3					m_xmf3Scale; //Scaler for World Coordinates

public:
	float GetHeight(float x, float z);
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z), m_xmf3Scale)); }

	int GetRawImageWidth() { return(m_pHeightMapImage->GetRawImageWidth()); }
	int GetRawImageLength() { return(m_pHeightMapImage->GetRawImageLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
#ifdef _WITH_VERTICES_AS_SCALING
	float GetWidth() { return(float(m_nWidth)); }
	float GetLength() { return(float(m_nLength)); }
#else
	int GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	int GetLength() { return(m_nLength * m_xmf3Scale.z); }
#endif
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCoverObject : public CGameObject
{
public:
	CCoverObject(int nMeshes = 1);
	virtual ~CCoverObject();

	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
};

class CMovingCoverObject : public CCoverObject
{
public:
	CMovingCoverObject(int nMeshes = 1);
	virtual ~CMovingCoverObject();

private:
	// 정해진 방향대로 움직이다가 point1/point2에 도달하면 방향을 바꿈
	XMFLOAT3 m_xmf3Direction;
	float m_fSpeed;

	XMFLOAT3 m_xmf3Point1;
	XMFLOAT3 m_xmf3Point2;

public:
	void SetMovingDirection(XMFLOAT3 xmf3Direction) { m_xmf3Direction = xmf3Direction; }
	void SetMovingSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetPoints(XMFLOAT3 xmf3Center);

public:
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
};

class CInteractiveCoverObject : public CCoverObject
{
public:
	CInteractiveCoverObject(int nMeshes = 1);
	virtual ~CInteractiveCoverObject();

private:
	// 플레이어가 상호작용 키를 누르면 5.0f가 되고 엄폐 가능
	// Animate 함수에서 시간이 감소하다가 0.0f가 되면 엄폐 불가능 상태로 전환
	float m_fInteractTime;

public:
	void InteractedByPlayer() { m_fInteractTime = 5.0f; }
	bool IsInteracted() { return (m_fInteractTime > 0.0f) ? true : false; }

public:
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
};

////////////////////////////////////////////////////

class CCannonballObject : public CGameObject
{
public:
	CCannonballObject();
	virtual ~CCannonballObject();

private:
	XMFLOAT3 m_xmf3Velocity;
	bool m_bIsFired = false;

public:
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
	bool IsReadyToFire();
	void SetValues(XMFLOAT3 origin, XMFLOAT3 velocity);
	void SetFire(bool fire) { m_bIsFired = fire; }
};

//

class CCannonObject : public CGameObject
{
public:
	CCannonObject();
	virtual ~CCannonObject();

private:
	CCannonballObject m_Cannonball;

public:
	virtual void Animate(float fTimeElapsed, CCamera* pCamera = NULL);
	void FireCannonBall(XMFLOAT3 Origin, XMFLOAT3 Velocity);
};