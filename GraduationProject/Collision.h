#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CollisionShader.h"
#include "Camera.h"
#include "Animation.h"

enum BOUNDING_STATE
{
	SPHERE,
	BODY,
	HIERACY,
	length
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
	virtual void CalculateBoundingBox() {};
	virtual void CalculateBoundingSphere() {};
	void SetBoundingState(int index) { state = static_cast<BOUNDING_STATE>(index);}
	BOUNDING_STATE GetBoundingState() { return state; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ToggleDebug() { m_bDebug = !m_bDebug; }

protected:
	bool m_bDebug = true;
	bool m_bRotate = false;
	CGameObject* FrameObject = nullptr;
	BOUNDING_STATE state = BOUNDING_STATE::HIERACY;
};

////////////////////////////////

class CBBCollision : public CCollision
{
public:
	CBBCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, BoundingBox BB);
	~CBBCollision();

	void SetBBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBB(DirectX::BoundingBox& BB);
	virtual void UpdateBoundings(XMFLOAT4X4 pxmf4x4World);
	virtual void CalculateBoundingBox();
private:
	BoundingBox m_xmBoundingBox;
};

/////////////////////////////////
class CSphereCollision : public CCollision
{
public:
	CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fradius);
	~CSphereCollision();
	
	void UpdateBoundings(XMFLOAT4X4 pxmf4x4World);
	virtual void CalculateBoundingSphere();
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBoundingSphere(DirectX::XMFLOAT3& center, float fradius);
private:
	BoundingSphere m_xmBoundingSphere;
};
