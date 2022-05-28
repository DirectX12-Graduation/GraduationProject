#pragma once
#include "Mesh.h"

class CGameObject;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct CALLBACKKEY
{
	float  							m_fTime = 0.0f;
	void* m_pCallbackData = NULL;
};

#define _WITH_ANIMATION_INTERPOLATION

class CAnimationCallbackHandler
{
public:
	CAnimationCallbackHandler() { }
	~CAnimationCallbackHandler() { }

public:
	virtual void HandleCallback(void* pCallbackData, float fTrackPosition) { }
};

class CAnimationCurve
{
public:
	CAnimationCurve(int nKeys);
	~CAnimationCurve();

public:
	int								m_nKeys = 0;

	float* m_pfKeyTimes = NULL;
	float* m_pfKeyValues = NULL;

public:
	float GetValueByLinearInterpolation(float fPosition);
};

class CAnimationLayer
{
public:
	CAnimationLayer();
	~CAnimationLayer();

public:
	float							m_fWeight = 1.0f;

	int								m_nAnimatedBoneFrames = 0;
	CGameObject** m_ppAnimatedBoneFrameCaches = NULL; //[m_nAnimatedBoneFrames]

	CAnimationCurve* (*m_ppAnimationCurves)[9] = NULL;

public:
	void LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE* pInFile);

	XMFLOAT4X4 GetSRT(int nBoneFrame, float fPosition, float fTrackWeight);
};

class CAnimationSet
{
public:
	CAnimationSet(float fStartTime, float fEndTime, char* pstrName);
	~CAnimationSet();

public:
	char							m_pstrAnimationSetName[64];

	int								m_nAnimationLayers = 0;
	CAnimationLayer* m_pAnimationLayers = NULL;

	float							m_fStartTime = 0.0f;
	float							m_fEndTime = 0.0f;
	float							m_fLength = 0.0f;

	float 							m_fPosition = 0.0f;
	int 							m_nType = Animation::Type::Loop; //Once, Loop, PingPong

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY* m_pCallbackKeys = NULL;

	CAnimationCallbackHandler* m_pAnimationCallbackHandler = NULL;

	bool m_bLoopEnd = false;

public:
	float SetPosition(float fElapsedTime, float fStartTime, float fEndTime);

	void Animate(float fTrackPosition, float fTrackWeight, float fStartTime, float fEndTime);

	void SetType(int nType) { m_nType = nType; }
	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler);

	void* GetCallbackData();
	void HandleCallback();
	bool IsOnceLoopEnd();
};

class CAnimationSets
{
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CAnimationSets(int nAnimationSets);
	~CAnimationSets();

public:
	int								m_nAnimationSets = 0;
	CAnimationSet** m_ppAnimationSets = NULL;

public:
	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler);
};

class CAnimationTrack
{
public:
	CAnimationTrack() { }
	~CAnimationTrack() { }

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

	float							m_fStartTime = 0.0f;
	float							m_fEndTime = 0.0f;
	float							m_fLength = 0.0f;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }
	int GetAnimationSet() { return m_nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }

	void SetStartEndTime(float fTrackStartTime, float fTrackEndTime) { m_fStartTime = fTrackStartTime; m_fEndTime = fTrackEndTime; m_fLength = m_fEndTime - m_fStartTime; }
};

class CLoadedModelInfo
{
public:
	CLoadedModelInfo() { }
	~CLoadedModelInfo();

public:
	CGameObject* m_pModelRootObject = NULL;

	CAnimationSets* m_pAnimationSets = NULL;

	int 							m_nSkinnedMeshes = 0;
	CSkinnedMesh** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

public:
	void PrepareSkinning();
};

class CAnimationController
{
public:
	CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel, bool bApplyRootMotion = false);
	~CAnimationController();

public:
	float 							m_fTime = 0.0f;
	bool							m_bApplyRootMotion = false;

	int 							m_nAnimationTracks = 0;
	CAnimationTrack* m_pAnimationTracks = NULL;

	CAnimationSets* m_pAnimationSets = NULL;

	int 							m_nSkinnedMeshes = 0;
	CSkinnedMesh** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	ID3D12Resource** m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;

private:
	//int m_nTracks = track_name::length;
	int m_nCurrentTracks = 0;
	int m_idleNum = 0;
	bool isAttack = false;

public:
	void SetAttackEnable(bool value) { isAttack = value; }
	bool GetAttackEnable() { return isAttack; }
	int GetCurrentTrackNum() { return m_nCurrentTracks; }
	void SetCurrentTrackNum(int index) { m_nCurrentTracks = index; }
	void SetIdleNum(int num) { m_idleNum = num; }
	void InitAnimationTime();

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetAnimationSets(CAnimationSets* pAnimationSets);
	void SetAnimationSetsType(int nAnimationSet, int nType);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackEnable(int nAnimationTrack, bool bEnable);
	void SetTrackPosition(int nAnimationTrack, float fPosition);
	void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	void SetTrackWeight(int nAnimationTrack, float fWeight);
	void SetTrackStartEndTime(int nAnimationTrack, float fStartTime, float fEndTime);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler);

	void AdvanceTime(float fElapsedTime, CGameObject* pRootGameObject);

	void SetAnimationTypes(bool* types);
	void SetAnimationTracks(bool* isSetNumberOne);
	void SwitchAnimationState(int nType);
};
