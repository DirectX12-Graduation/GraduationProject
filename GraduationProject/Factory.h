#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"

class CFactory
{
public:
	CFactory() {};
	~CFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		ID3D12GraphicsCommandList* pd3dCommandList, void* pContext) = 0;
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamrea = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	vector<CGameObject*> GetGameObjects() { return _gameObjects; }

protected:
	vector<CGameObject*> _gameObjects;
};

/////////////////////

class CObjectFactory : public CFactory
{
public:
	CObjectFactory() {};
	~CObjectFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};

class CCannonFactory : public CFactory
{
public:
	CCannonFactory() {};
	~CCannonFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};

class CMonsterFactory : public CFactory
{
public:
	CMonsterFactory() {};
	~CMonsterFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

};

class CUIFactory : public CFactory
{
public:
	CUIFactory() {};
	~CUIFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	CCamera* m_pCamera = NULL;
};