#include "stdafx.h"
#include "Collision.h"
#include "GameObject.h"
#include "Shader.h"
#include "Scene.h"

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

void CCollision::UpdateBoundTransform()
{
	if (FrameObject)
	{
		UpdateTransform(&FrameObject->m_xmf4x4World);
	}
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

void CCollision::UpdateBound(BoundingBox& BB, BoundingSphere& BS)
{
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

void CBBCollision::UpdateBoundTransform()
{
	CCollision::UpdateBoundTransform();
}

void CBBCollision::UpdateBound(BoundingBox& BB, BoundingSphere& BS)
{
	m_xmCollBoundingBox.Transform(BB, XMLoadFloat4x4(&m_xmf4x4World));
}

void CBBCollision::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
//#define _SHOW_DEBUG_BOX
#ifdef _SHOW_DEBUG_BOX
	if (m_bDebug) CGameObject::Render(pd3dCommandList, pCamera);
#endif
}

////////////////////////////////

CSphereCollision::CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, BoundingSphere BS)
{
	SetBoundingSphere(BS);
	
	CSphereMeshIlluminated* DebugSphere = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, BS.Radius);
	SetMesh(DebugSphere);

	SetCollisionMaterial(pd3dDevice, pd3dGraphicsRootSignature, pd3dCommandList);
	m_bDebug = true;
}

void CSphereCollision::SetBoundingSphere(BoundingSphere& BS)
{
	XMStoreFloat3(&m_xmCollBoundingSphere.Center, XMLoadFloat3(&BS.Center));
	m_xmCollBoundingSphere.Radius = BS.Radius;
	SetPosition(m_xmCollBoundingSphere.Center);
	UpdateTransform(nullptr);
}

void CSphereCollision::UpdateBound(BoundingBox& BB, BoundingSphere& BS)
{
	m_xmCollBoundingSphere.Transform(BS, XMLoadFloat4x4(&m_xmf4x4World));
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

void CSphereCollision::UpdateBoundTransform()
{
	CCollision::UpdateBoundTransform();
}

void CSphereCollision::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
//#define _SHOW_DEBUG_SPHERE
#ifdef _SHOW_DEBUG_SPHERE
	if (m_bDebug) CGameObject::Render(pd3dCommandList, pCamera);
#endif
}