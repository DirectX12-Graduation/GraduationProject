#pragma once
#include "GameObject.h"
#include "Camera.h"

enum BOUNDING_INFO
{
	SPHERE,
	BOX,
	SWORD,
	LENGTH
};

class CCollisionManager
{
public:
	CCollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		CGameObject* pGameObject, string& filename);
	~CCollisionManager();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void LoadFromFileBoundInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pGameObject, string& filename);
	virtual void UpdateCollisions();
	virtual BoundingBox GetBoundingBox(bool bSword = false) { return m_xmBoundingBox; }
	BoundingSphere GetBoundingSphere() { return m_xmBoundingSphere; }

protected:
	BoundingBox m_xmBoundingBox;
	BoundingSphere m_xmBoundingSphere;
	unordered_map<int, CCollision*> collisions;
};

class CPlayerCollisionManager : public CCollisionManager
{
public:
	CPlayerCollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		CGameObject* pGameObject, string& filename);
	~CPlayerCollisionManager();
	virtual BoundingBox GetBoundingBox(bool bSword = false) { return (bSword) ? m_xmSwordBound : m_xmBoundingBox; }

	virtual void UpdateCollisions();

protected:
	BoundingBox m_xmSwordBound;
};