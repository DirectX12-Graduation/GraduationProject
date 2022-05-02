#include "stdafx.h"
#include "Collision.h"
#include "CollisionManager.h"


CCollisionManager::CCollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	CGameObject* pGameObject, string& filename)
{
	LoadFromFileBoundInfo(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, filename);
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::LoadFromFileBoundInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pGameObject, string& filename)
{
	ifstream boundingInfo(filename);
	string s, frame;
	XMFLOAT3 center, extends;
	float radius;

	while (boundingInfo >> s >> frame)
	{
		if (s.compare("<Sphere>:") == 0)
		{
			boundingInfo >> center.x >> center.y >> center.z;
			boundingInfo >> radius;
			CGameObject* pBoneObject = pGameObject->FindFrame(frame.c_str());

			BoundingSphere BS;
			XMStoreFloat3(&BS.Center, XMLoadFloat3(&center));
			BS.Radius = radius;

			CCollision* cols = new CSphereCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BS);
			cols->SetFrameObject(pBoneObject);
			collisions[static_cast<int>(BOUNDING_INFO::SPHERE)] = cols;
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
			collisions[static_cast<int>(BOUNDING_INFO::BOX)] = cols;
		}		
		if (s.compare("<Sword>:") == 0)
		{
			boundingInfo >> center.x >> center.y >> center.z;
			boundingInfo >> extends.x >> extends.y >> extends.z;
			CGameObject* pBoneObject = pGameObject->FindFrame(frame.c_str());
			BoundingBox BB;
			XMStoreFloat3(&BB.Center, XMLoadFloat3(&center));
			XMStoreFloat3(&BB.Extents, XMLoadFloat3(&extends));

			CCollision* cols = new CBBCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BB);
			cols->SetFrameObject(pBoneObject);
			collisions[static_cast<int>(BOUNDING_INFO::SWORD)] = cols;
		}
	}
}

void CCollisionManager::UpdateCollisions()
{
	for (auto iter : collisions)
	{
		iter.second->UpdateBound(m_xmBoundingBox, m_xmBoundingSphere);
	}
}

void CCollisionManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto iter : collisions)
	{
		if (!iter.second->GetDebug()) continue;

		iter.second->UpdateBoundTransform();
		iter.second->UpdateBound(m_xmBoundingBox, m_xmBoundingSphere);
		iter.second->Render(pd3dCommandList, pCamera);
	}
}

//////////////////////////////////////////////////////////////////////////////////

CPlayerCollisionManager::CPlayerCollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pGameObject, string& filename)
	: CCollisionManager(pd3dDevice,pd3dCommandList,pd3dGraphicsRootSignature,pGameObject,filename)
{
}

CPlayerCollisionManager::~CPlayerCollisionManager()
{
}

void CPlayerCollisionManager::UpdateCollisions()
{
	for (auto iter : collisions)
	{
		iter.second->UpdateBound(
			(iter.first==static_cast<int>(SWORD)) ? m_xmSwordBound : m_xmBoundingBox, m_xmBoundingSphere);
	}
}
