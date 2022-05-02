#pragma once
#include "Player.h"

enum track_name
{
	idle,
	impact,
	run,
	walk,
	walk_back,
	walk_left,
	walk_right,
	attack_object,
	attack,
	attack_combo,
	attack_magic,
	attack_magic2,
	interaction,
	handling,
	lifting,
	sitting,
	deth,
	length
};

class CAnimPlayer : public CPlayer
{
public:
	CAnimPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CAnimPlayer();

	void SetResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void InitPlayerMatrics(void* pContext);

	virtual void OnPrepareRender();
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	virtual void Update(float fTimeElapsed);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pAngrybotModel);
	bool SetInteraction(XMFLOAT3& center, XMFLOAT4X4& world);
	void SetPlayerLookAtPos(DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT3& center);
	void SetAnimationTypes();
	bool IsPlayerInteraction() { return isMove; }

private:
	bool isMove = false;
};
