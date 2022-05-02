#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CollisionShader.h"
#include "Camera.h"

///////////////////////////////////////////////////////

class CCollision : public CGameObject
{
public:
	CCollision();
	~CCollision();

	bool IsRotate() { return m_bRotate; }
	void SetIsRotate(bool bVal) { m_bRotate = bVal; }
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetFrameObject(CGameObject* pObject) { if (FrameObject == nullptr) FrameObject = pObject; }
	virtual void UpdateBoundTransform();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ToggleDebug() { m_bDebug = !m_bDebug; }
	void SetBBScale(float x, float y, float z);
	virtual void UpdateBound(BoundingBox& BB, BoundingSphere& BS);
	bool GetDebug() {return m_bDebug; }
	virtual BoundingBox GetBoundingBox() { return BoundingBox(); }
	virtual BoundingSphere GetBoundingSphere() { return BoundingSphere(); }

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
	virtual void UpdateBoundTransform();
	virtual void UpdateBound(BoundingBox& BB, BoundingSphere& BS);
	virtual BoundingBox GetBoundingBox() { return m_xmCollBoundingBox; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
private:
	BoundingBox m_xmCollBoundingBox;
};

/////////////////////////////////
class CSphereCollision : public CCollision
{
public:
	CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, BoundingSphere BS);
	~CSphereCollision();
	
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBoundingSphere(BoundingSphere& BS);

	virtual void UpdateBound(BoundingBox& BB, BoundingSphere& BS);
	virtual void UpdateBoundTransform();
	virtual BoundingSphere GetBoundingSphere() { return m_xmCollBoundingSphere; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
private:
	BoundingSphere m_xmCollBoundingSphere;
};
