#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Animation.h"

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05
#define RESOURCE_STRUCTURED_BUFFER	0x06

class CShader;

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

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex, bool bIsDDSFile = true);
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
	ID3D12Resource* GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
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
	CMaterial(int nTextures);
	virtual ~CMaterial();

private:
	int						m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	XMFLOAT4				m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4				m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4				m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4				m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	UINT					m_nReflection = 0;
	CShader*				m_pShader = NULL;

	UINT					m_nType = 0x00;

	float					m_fGlossiness = 0.0f;
	float					m_fSmoothness = 0.0f;
	float					m_fSpecularHighlight = 0.0f;
	float					m_fMetallic = 0.0f;
	float					m_fGlossyReflection = 0.0f;

	int 					m_nTextures = 0;
	CTexture** m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;

public:
	static CShader* m_pWireFrameShader;
	static CShader* m_pSkinnedAnimationWireFrameShader;

public:

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4AlbedoColor = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(CTexture* pTexture, UINT nTexture = 0);
	void SetTexture(int nTexture);
	void SetShader(CShader* pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void ReleaseUploadBuffers();

	//void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, CTexture** ppTexture, CGameObject* pParent, FILE* pInFile, CShader* pShader);

	static void CMaterial::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void SetWireFrameShader() { CMaterial::SetShader(m_pWireFrameShader); }
	void SetSkinnedAnimationWireFrameShader() { CMaterial::SetShader(m_pSkinnedAnimationWireFrameShader); }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGameObject
{
private:
	int								m_nReferences = 0;

public:
	void AddRef();
	void Release();

public:
	CGameObject();
	CGameObject(int nMaterials);
	virtual ~CGameObject();

public:
	char m_pstrFrameName[64];

	bool m_bActive = true;

	XMFLOAT4X4 m_xmf4x4ToParent;
	XMFLOAT4X4 m_xmf4x4World;

	XMFLOAT3 m_xmf3Scale;
	XMFLOAT3 m_xmf3Rotation;
	XMFLOAT3 m_xmf3Translation;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

	CMesh* m_pMesh = NULL;

	int	m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	BoundingBox						m_xmBoundingBox;

	CTexture* m_pTexture;

public:
	void SetMesh(CMesh* pMesh);
	void SetShader(CShader* pShader);
	void SetShader(int nMaterial, CShader* pShader);
	void SetWireFrameShader();
	void SetSkinnedAnimationWireFrameShader();
	void SetMaterial(int nMaterial, CMaterial* pMaterial);
	void SetScale(XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

	virtual void OnPrepareAnimate() { }
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);

	virtual void OnPrepareRender() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial);

	void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	void SetActive(bool bActive) { m_bActive = bActive; }
	void CalculateBoundingBox();

	CGameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	CGameObject* FindFrame(char* pstrFrameName);
	void SetActive(char* pstrFrameName, bool bActive);
	UINT GetMeshType(int n) { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

public:
	CAnimationController* m_pSkinnedAnimationController = NULL;

	CSkinnedMesh* FindSkinnedMesh(char* pstrSkinnedMeshName);
	void FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	static void LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel);
	static CGameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader, int* pnSkinnedMeshes);

	static CLoadedModelInfo* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader);

	static void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
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
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, 
		int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage* m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
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
	CCannonballObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CCannonballObject();

private:
	XMFLOAT3 m_xmf3Velocity;
	bool m_bIsFired = false;
	LPVOID m_pUpdatedContext;

public:
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);
	void SetUpdatedContext(LPVOID pContext) { m_pUpdatedContext = pContext; }

	void SetValues(XMFLOAT3 origin, XMFLOAT3 velocity);
	void SetFire(bool fire) { m_bIsFired = fire; }

	bool IsReadyToFire();
};

//

class CCannonObject : public CGameObject
{
public:
	CCannonObject();
	virtual ~CCannonObject();

private:
	CCannonballObject* m_pCannonball;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Animate(float fTimeElapsed, CCamera* pCamera = NULL);

	void RotateCannon(XMFLOAT3* pxmf3Axis, float fAngle);

	void FireCannonBall(XMFLOAT3 Origin, XMFLOAT3 Velocity);
	void SetCannonball(CCannonballObject* pCannonball) { m_pCannonball = pCannonball; }
	CCannonballObject* GetCannonball() { return m_pCannonball; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CAngrybotObject : public CGameObject
{
public:
	CAngrybotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks);
	virtual ~CAngrybotObject();
};
