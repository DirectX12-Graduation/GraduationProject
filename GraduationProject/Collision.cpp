#include "stdafx.h"
#include "Collision.h"
#include "GameObject.h"
#include "Shader.h"
#include "Scene.h"

CCollisionManager::CCollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	CGameObject* pGameObject, string& filename)
{
	for (int i = 0; i < collisions.size(); ++i)
		collisions[i] = new CCollision();
	LoadFromFileBoundInfo(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, filename);
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::LoadFromFileBoundInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,CGameObject* pGameObject, string& filename)
{
	ifstream boundingInfo(filename);
	string s, frame;
	XMFLOAT3 center, extends;
	float radius;

	while (boundingInfo >> s >> frame)
	{
		if (s.compare("<Sphere>:") == 0)
		{
			boundingInfo >> radius;
			CGameObject* pBoneObject = pGameObject->FindFrame(frame.c_str());
			CCollision* cols = new CSphereCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, radius);
			cols->SetFrameObject(pBoneObject);
			collisions[BOUNDING_INFO::SPHERE] = cols;
		}
		if (s.compare("<Box>:") == 0)
		{
			boundingInfo >> center.x >> center.y >> center.z;
			boundingInfo >> extends.x >> extends.y >> extends.z;
			CGameObject* pBoneObject = pGameObject->FindFrame(frame.c_str());
			BoundingBox BB;
			XMStoreFloat3(&BB.Center, XMLoadFloat3(&center));
			XMStoreFloat3(&BB.Extents, XMLoadFloat3(&extends));

			CCollision* cols = new CBBCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BB);
			cols->SetFrameObject(pBoneObject);
			collisions[BOUNDING_INFO::BOX] = cols;
		}
	}
}

void CCollisionManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (CCollision* col : collisions)
	{
		col->Render(pd3dCommandList, pCamera);
	}
}

/////////////////////////////
CCollision::CCollision() : CGameObject(1)
{

}
CCollision::~CCollision()
{

}

void CCollision::SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCollisionShader* pShader = new CCollisionShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetShader(pShader);
	SetMaterial(0, pMaterial);
}

void CCollision::UpdateBoundings(XMFLOAT4X4 xmf4x4World)
{
	if (FrameObject)
	{
		UpdateTransform(&FrameObject->m_xmf4x4World);
	}

	if (m_bRotate) Rotate(-90.0f, 0.0f, 0.0f);
}

void CCollision::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bDebug) CGameObject::Render(pd3dCommandList, pCamera);
}

void CCollision::SetBBScale(float x, float y, float z)
{
	m_xmf3Scale.x = x;
	m_xmf3Scale.y = y;
	m_xmf3Scale.z = z;
}

////////////////////////////////

CBBCollision::CBBCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	BoundingBox BB)
{
	SetBB(BB);
	SetBBMesh(pd3dDevice, pd3dCommandList);
	CCollision::SetCollisionMaterial(pd3dDevice, pd3dGraphicsRootSignature, pd3dCommandList);
	m_bDebug = true;
}

CBBCollision::~CBBCollision()
{
}

void CBBCollision::SetBBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT3 xmf3Depth;
	XMStoreFloat3(&xmf3Depth, XMVectorScale(XMLoadFloat3(&m_xmCollBoundingBox.Extents), 2.0f));
	CCubeMeshDiffused* DebugBox = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, xmf3Depth.x, xmf3Depth.y, xmf3Depth.z);
	SetMesh(DebugBox);
}

void CBBCollision::SetBB(DirectX::BoundingBox& BB)
{
	XMStoreFloat3(&m_xmCollBoundingBox.Center, XMLoadFloat3(&BB.Center));
	SetPosition(m_xmCollBoundingBox.Center);
	UpdateTransform(nullptr);
	XMStoreFloat3(&m_xmCollBoundingBox.Extents, XMLoadFloat3(&BB.Extents));
}

void CBBCollision::UpdateBoundings(XMFLOAT4X4 xmf4x4World)
{
	CCollision::UpdateBoundings(xmf4x4World);
}

////////////////////////////////

CSphereCollision::CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fradius)
{
	XMFLOAT3 center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	SetBoundingSphere(center, fradius);
	
	CSphereMeshIlluminated* DebugSphere = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, fradius);
	SetMesh(DebugSphere);

	SetCollisionMaterial(pd3dDevice, pd3dGraphicsRootSignature, pd3dCommandList);
	m_bDebug = true;
}

void CSphereCollision::SetBoundingSphere(DirectX::XMFLOAT3& center, float fradius)
{
	XMStoreFloat3(&m_xmCollBoundingSphere.Center, XMLoadFloat3(&center));
	m_xmCollBoundingSphere.Radius = fradius;
}

CSphereCollision::~CSphereCollision()
{
}

void CSphereCollision::SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CSphereCollisionShader* pShader = new CSphereCollisionShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetShader(pShader);
	SetMaterial(0, pMaterial);
}

void CSphereCollision::UpdateBoundings(XMFLOAT4X4 xmf4x4World)
{
	CCollision::UpdateBoundings(xmf4x4World);
}

