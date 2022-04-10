#pragma once
#include "Player.h"

enum track_name
{
	idle,
	impact,
	run,
	run_back,
	run_right,
	run_left,
	attack_down,
	attack_slash,
	attack_spin,
	handling,
	lifting,
	interaction,
	length
};

class CAnimPlayer : public CPlayer
{
public:
	CAnimPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	void SetAnimationTypes();
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

	void SetAnimationTracks(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pAngrybotModel);
	void SwitchAnimationState(int nType);

private:
	int m_nTracks = track_name::length;
	int m_nCurrentTracks = track_name::idle;
};
