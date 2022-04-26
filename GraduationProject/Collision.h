#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CollisionShader.h"
#include "Camera.h"
#include "Animation.h"

enum BOUNDING_INFO
{
	SPHERE,
	BOX,
	LENGTH
};

class CCollisionManager
{
public:
	CCollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		CGameObject* pGameObject, string& filename);
	~CCollisionManager();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void LoadFromFileBoundInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pGameObject, string& filename);
private:
	BoundingBox m_xmBoundingBox;
	BoundingSphere m_xmBoundingSphere;
	array<CCollision*, static_cast<int>(BOUNDING_INFO::LENGTH)> collisions;
};

class CCollision : public CGameObject
{
public:
	CCollision();
	~CCollision();

	bool IsRotate() { return m_bRotate; }
	void SetIsRotate(bool bVal) { m_bRotate = bVal; }
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetFrameObject(CGameObject* pObject) { if (FrameObject == nullptr) FrameObject = pObject; }
	virtual void UpdateBoundings(XMFLOAT4X4 pxmf4x4World);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ToggleDebug() { m_bDebug = !m_bDebug; }
	void SetBBScale(float x, float y, float z);

protected:
	bool m_bDebug = false;
	bool m_bRotate = false;
	bool isScale = false;
	CGameObject* FrameObject = nullptr;
	XMFLOAT3 m_xmf3Scale;
};

////////////////////////////////

class CBBCollision : public CCollision
{
public:
	CBBCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,BoundingBox BB);
	~CBBCollision();

	void SetBBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBB(DirectX::BoundingBox& BB);
	virtual void UpdateBoundings(XMFLOAT4X4 pxmf4x4World);
	virtual BoundingBox GetBoundingBox() { return m_xmCollBoundingBox; }
private:
	BoundingBox m_xmCollBoundingBox;
};

/////////////////////////////////
class CSphereCollision : public CCollision
{
public:
	CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fradius);
	~CSphereCollision();
	
	void UpdateBoundings(XMFLOAT4X4 pxmf4x4World);
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBoundingSphere(DirectX::XMFLOAT3& center, float fradius);
	virtual BoundingSphere GetBoundingSphere() { return m_xmCollBoundingSphere; }
private:
	BoundingSphere m_xmCollBoundingSphere;
};
