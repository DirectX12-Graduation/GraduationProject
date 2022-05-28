#pragma once
#include "Vertex.h"

class CGameObject;
class CCollision;

////////////////////////////////////////////////////////////////////////
class CMesh
{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	char							m_pstrMeshName[64] = { 0 };

protected:
	UINT							m_nType = 0x00;

	XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nOffset = 0;

protected:
	UINT m_nVertices = 0;

	XMFLOAT3* m_pxmf3Positions = NULL;

	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								m_nSubMeshes = 0;
	int* m_pnSubSetIndices = NULL;
	UINT** m_ppnSubSetIndices = NULL;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews = NULL;


	XMFLOAT2* m_pxmf2TextureCoords = NULL;
	ID3D12Resource* m_pd3dTextureCoordBuffer = NULL;
	ID3D12Resource* m_pd3dTextureCoordUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoordBufferView;

	UINT m_nStride = 0;

public:
	BoundingBox m_xmBoundingBox;

public:
	UINT GetType() { return(m_nType); }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet, int nPipelineState = 0);
	virtual void OnPostRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);

	void CalculateBoundingBox(XMFLOAT3* pxmf3Points, UINT nStride);

	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	void SetBoundingBoxValues(DirectX::XMFLOAT3& max, DirectX::XMFLOAT3& min);
	virtual void UpdateBoundingTransform(CCollision* pCollision, XMFLOAT4X4& xmf4x4World);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CTriangleMesh() { }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCubeMeshDiffused : public CMesh
{
public:
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList 
	*pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshTextured : public CMesh
{
public:
	CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CRawFormatImage
{
protected:
	BYTE* m_pRawImagePixels = NULL;

	int							m_nWidth;
	int							m_nLength;

public:
	CRawFormatImage(LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY = false);
	~CRawFormatImage(void);

	BYTE GetRawImagePixel(int x, int z) { return(m_pRawImagePixels[x + (z * m_nWidth)]); }
	void SetRawImagePixel(int x, int z, BYTE nPixel) { m_pRawImagePixels[x + (z * m_nWidth)] = nPixel; }

	BYTE* GetRawImagePixels() { return(m_pRawImagePixels); }

	int GetRawImageWidth() { return(m_nWidth); }
	int GetRawImageLength() { return(m_nLength); }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapImage
{
private:
	BYTE* m_pHeightMapPixels;

	int								m_nWidth;
	int								m_nLength;
	XMFLOAT3						m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	float GetHeight(float x, float z, bool bReverseQuad = false);
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapGridMesh : public CMesh
{
protected:
	int								m_nWidth;
	int								m_nLength;
	XMFLOAT3						m_xmf3Scale;

protected:
	XMFLOAT4* m_pxmf4Colors = NULL;
	XMFLOAT2* m_pxmf2TextureCoords0 = NULL;
	XMFLOAT2* m_pxmf2TextureCoords1 = NULL;

	ID3D12Resource* m_pd3dColorBuffer = NULL;
	ID3D12Resource* m_pd3dColorUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dColorBufferView;

	ID3D12Resource* m_pd3dTextureCoord0Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord0UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	ID3D12Resource* m_pd3dTextureCoord1Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord1UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord1BufferView;

public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void* pContext);
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);

	virtual void ReleaseUploadBuffers();


	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedRectMesh : public CMesh
{
public:
	CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CBillboardMesh : public CMesh
{
public:
	CBillboardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f);
	virtual ~CBillboardMesh();
};

////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMeshIlluminated();

public:
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminatedTextured : public CMeshIlluminated
{
public:
	CMeshIlluminatedTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshIlluminatedTextured : public CMeshIlluminatedTextured
{
public:
	CCubeMeshIlluminatedTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlaneMeshIlluminated : public CMeshIlluminated
{
public:
	CPlaneMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CPlaneMeshIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxMesh : public CMesh
{
public:
	CSkyBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~CSkyBoxMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius = 2.0f, UINT nSlices = 20, UINT nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CModelMesh : public CMesh
{
public:
	CModelMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CModelMesh();
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);
	virtual void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define SKINNED_ANIMATION_BONES		128

class CSkinnedMesh : public CMesh
{
public:
	CSkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CSkinnedMesh();

protected:
	int								m_nBonesPerVertex = 4;

	XMINT4* m_pxmn4BoneIndices = NULL;

	ID3D12Resource* m_pd3dBoneIndexBuffer = NULL;
	ID3D12Resource* m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	XMFLOAT4* m_pxmf4BoneWeights = NULL;

	ID3D12Resource* m_pd3dBoneWeightBuffer = NULL;
	ID3D12Resource* m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								m_nSkinningBones = 0;

	char(*m_ppstrSkinningBoneNames)[64];
	CGameObject** m_ppSkinningBoneFrameCaches = NULL;

	XMFLOAT4X4* m_pxmf4x4BindPoseBoneOffsets = NULL;

	ID3D12Resource* m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4* m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL; //Transposed

	ID3D12Resource* m_pd3dcbSkinningBoneTransforms = NULL;
	XMFLOAT4X4* m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void PrepareSkinning(CGameObject* pModelRootObject);
	void LoadSkinDeformationsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);
	virtual void UpdateBoundingTransform(CCollision* pCollision, XMFLOAT4X4& xmf4x4World);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_PARTICLES					999
#define _WITH_QUERY_DATA_SO_STATISTICS

class CParticleMesh : public CMesh
{
public:
	CParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, float fLifetime, UINT nMaxParticles);
	virtual ~CParticleMesh();

	bool								m_bStart = true;

	UINT								m_nMaxParticles = MAX_PARTICLES;

	ID3D12Resource* m_pd3dStreamOutputBuffer = NULL;
	ID3D12Resource* m_pd3dDrawBuffer = NULL;

	ID3D12Resource* m_pd3dDefaultBufferFilledSize = NULL;
	ID3D12Resource* m_pd3dUploadBufferFilledSize = NULL;
	UINT64* m_pnUploadBufferFilledSize = NULL;

#ifdef _WITH_QUERY_DATA_SO_STATISTICS
	ID3D12QueryHeap* m_pd3dSOQueryHeap = NULL;
	ID3D12Resource* m_pd3dSOQueryBuffer = NULL;
	D3D12_QUERY_DATA_SO_STATISTICS* m_pd3dSOQueryDataStatistics = NULL;
#else
	ID3D12Resource* m_pd3dReadBackBufferFilledSize = NULL;
#endif


	D3D12_STREAM_OUTPUT_BUFFER_VIEW		m_d3dStreamOutputBufferView;

	virtual void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, float fLifetime);
	virtual void CreateStreamOutputBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nMaxParticles);

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnPostRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, int nPipelineState = 0);
};
