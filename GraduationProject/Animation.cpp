#include "stdafx.h"
#include "Animation.h"
#include "GameObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationCurve::CAnimationCurve(int nKeys)
{
	m_nKeys = nKeys;
	m_pfKeyTimes = new float[nKeys];
	m_pfKeyValues = new float[nKeys];
}

CAnimationCurve::~CAnimationCurve()
{
	if (m_pfKeyTimes) delete[] m_pfKeyTimes;
	if (m_pfKeyValues) delete[] m_pfKeyValues;
}

float CAnimationCurve::GetValueByLinearInterpolation(float fPosition)
{
	for (int k = 0; k < (m_nKeys - 1); k++)
	{
		if ((m_pfKeyTimes[k] <= fPosition) && (fPosition < m_pfKeyTimes[k + 1]))
		{
			float t = (fPosition - m_pfKeyTimes[k]) / (m_pfKeyTimes[k + 1] - m_pfKeyTimes[k]);
			return(m_pfKeyValues[k] * (1.0f - t) + m_pfKeyValues[k + 1] * t);
		}
	}
	return(m_pfKeyValues[m_nKeys - 1]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationLayer::CAnimationLayer()
{
}

CAnimationLayer::~CAnimationLayer()
{
	for (int i = 0; i < m_nAnimatedBoneFrames; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (m_ppAnimationCurves[i][j]) delete m_ppAnimationCurves[i][j];
		}
	}
	if (m_ppAnimationCurves) delete[] m_ppAnimationCurves;

	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
}

void CAnimationLayer::LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE* pInFile)
{
	int nAnimationKeys = ::ReadIntegerFromFile(pInFile);

	m_ppAnimationCurves[nBoneFrame][nCurve] = new CAnimationCurve(nAnimationKeys);

	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyTimes, sizeof(float), nAnimationKeys, pInFile);
	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyValues, sizeof(float), nAnimationKeys, pInFile);
}

XMFLOAT4X4 CAnimationLayer::GetSRT(int nBoneFrame, float fPosition, float fTrackWeight)
{
	CGameObject* pBoneFrame = m_ppAnimatedBoneFrameCaches[nBoneFrame];
	XMFLOAT3 xmf3S = pBoneFrame->m_xmf3Scale;
	XMFLOAT3 xmf3R = pBoneFrame->m_xmf3Rotation;
	XMFLOAT3 xmf3T = pBoneFrame->m_xmf3Translation;

	if (m_ppAnimationCurves[nBoneFrame][0]) xmf3T.x = m_ppAnimationCurves[nBoneFrame][0]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][1]) xmf3T.y = m_ppAnimationCurves[nBoneFrame][1]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][2]) xmf3T.z = m_ppAnimationCurves[nBoneFrame][2]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][3]) xmf3R.x = m_ppAnimationCurves[nBoneFrame][3]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][4]) xmf3R.y = m_ppAnimationCurves[nBoneFrame][4]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][5]) xmf3R.z = m_ppAnimationCurves[nBoneFrame][5]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][6]) xmf3S.x = m_ppAnimationCurves[nBoneFrame][6]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][7]) xmf3S.y = m_ppAnimationCurves[nBoneFrame][7]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][8]) xmf3S.z = m_ppAnimationCurves[nBoneFrame][8]->GetValueByLinearInterpolation(fPosition);

	float fWeight = m_fWeight * fTrackWeight;
	XMMATRIX S = XMMatrixScaling(xmf3S.x * fWeight, xmf3S.y * fWeight, xmf3S.z * fWeight);
	//	XMMATRIX R = XMMatrixRotationRollPitchYaw(xmf3R.x * fWeight, xmf3R.y * fWeight, xmf3R.z * fWeight);
	XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(xmf3R.x * fWeight), XMMatrixRotationY(xmf3R.y * fWeight)), XMMatrixRotationZ(xmf3R.z * fWeight));
	XMMATRIX T = XMMatrixTranslation(xmf3T.x * fWeight, xmf3T.y * fWeight, xmf3T.z * fWeight);

	XMFLOAT4X4 xmf4x4Transform;
	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixMultiply(XMMatrixMultiply(S, R), T));

	return(xmf4x4Transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSet::CAnimationSet(float fStartTime, float fEndTime, char* pstrName)
{
	m_fStartTime = fStartTime;
	m_fEndTime = fEndTime;
	m_fLength = fEndTime - fStartTime;

	strcpy_s(m_pstrAnimationSetName, 64, pstrName);
}

CAnimationSet::~CAnimationSet()
{
	if (m_pAnimationLayers) delete[] m_pAnimationLayers;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
}

void CAnimationSet::HandleCallback()
{
	if (m_pAnimationCallbackHandler)
	{
		for (int i = 0; i < m_nCallbackKeys; i++)
		{
			if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, Animation::Callback_Epsilon))
			{
				if (m_pCallbackKeys[i].m_pCallbackData) m_pAnimationCallbackHandler->HandleCallback(m_pCallbackKeys[i].m_pCallbackData, m_fPosition);
				break;
			}
		}
	}
}

float CAnimationSet::SetPosition(float fElapsedTime, float fTrackStartTime, float fTrackEndTime)
{
	switch (m_nType)
	{
	case Animation::Type::Loop:
	{
		m_fPosition += fElapsedTime;
		if (m_fPosition < fTrackStartTime) m_fPosition = fTrackStartTime;
		if (m_fPosition >= fTrackEndTime) m_fPosition = fTrackStartTime;
		//			m_fPosition = fmod(fTrackPosition, m_pfKeyFrameTimes[m_nKeyFrames-1]); // m_fPosition = fTrackPosition - int(fTrackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			m_fPosition = fmod(fTrackPosition, m_fLength); //if (m_fPosition < 0) m_fPosition += m_fLength;
		//			m_fPosition = fTrackPosition - int(fTrackPosition / m_fLength) * m_fLength;

		break;
	}
		case Animation::Type::Once:
			m_fPosition += fElapsedTime;
			if (m_fPosition < fTrackStartTime) m_fPosition = fTrackStartTime;

			if (m_fPosition >= fTrackEndTime)
			{
				m_fPosition = fTrackStartTime;
				m_bLoopEnd = true;
			}
			break;
	case Animation::Type::PingPong:
		break;
	}

	return(m_fPosition);
}

void CAnimationSet::Animate(float fElapsedTime, float fTrackWeight, float fTrackStartTime, float fTrackEndTime)
{
	float fPosition = SetPosition(fElapsedTime, fTrackStartTime, fTrackEndTime);

	for (int i = 0; i < m_nAnimationLayers; i++)
	{
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
		{
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = m_pAnimationLayers[i].GetSRT(j, fPosition, fTrackWeight);
		}
	}
}

bool CAnimationSet::IsOnceLoopEnd()
{
	if (m_nType != Animation::Type::Once) return false;
	return (m_bLoopEnd) ? true : false;
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void* pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSet::SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSets::CAnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_ppAnimationSets = new CAnimationSet * [nAnimationSets];
	for (int i = 0; i < m_nAnimationSets; i++) m_ppAnimationSets[i] = NULL;
}

CAnimationSets::~CAnimationSets()
{
	for (int i = 0; i < m_nAnimationSets; i++) if (m_ppAnimationSets[i]) delete m_ppAnimationSets[i];
	if (m_ppAnimationSets) delete[] m_ppAnimationSets;
}

void CAnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_ppAnimationSets[nAnimationSet]->m_nCallbackKeys = nCallbackKeys;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
{
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSets::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler)
{
	m_ppAnimationSets[nAnimationSet]->SetAnimationCallbackHandler(pCallbackHandler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel, bool bApplyRootMotion)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];

	m_bApplyRootMotion = bApplyRootMotion;

	SetAnimationSets(pModel->m_pAnimationSets);

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes = new CSkinnedMesh * [m_nSkinnedMeshes];
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256ÀÇ ¹è¼ö
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;

	if (m_pAnimationSets) m_pAnimationSets->Release();

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CAnimationController::SetAnimationSets(CAnimationSets* pAnimationSets)
{
	if (m_pAnimationSets) m_pAnimationSets->Release();

	m_pAnimationSets = pAnimationSets;
	if (m_pAnimationSets) m_pAnimationSets->AddRef();
}

void CAnimationController::SetAnimationSetsType(int nAnimationSet, int nType)
{
	if (m_pAnimationSets == nullptr) return;

	m_pAnimationSets->m_ppAnimationSets[nAnimationSet]->SetType(nType);
}


void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKeys(nAnimationSet, nCallbackKeys);
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKey(nAnimationSet, nKeyIndex, fKeyTime, pData);
}

void CAnimationController::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler)
{
	if (m_pAnimationSets) m_pAnimationSets->SetAnimationCallbackHandler(nAnimationSet, pCallbackHandler);
}

void CAnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks)
	{
		m_pAnimationTracks[nAnimationTrack].SetAnimationSet(nAnimationSet);
		m_pAnimationTracks[nAnimationTrack].SetStartEndTime(m_pAnimationSets->m_ppAnimationSets[nAnimationSet]->m_fStartTime, m_pAnimationSets->m_ppAnimationSets[nAnimationSet]->m_fEndTime);
	}
}

void CAnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void CAnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void CAnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void CAnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void CAnimationController::SetTrackStartEndTime(int nAnimationTrack, float fStartTime, float fEndTime)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetStartEndTime(fStartTime, fEndTime);
}

void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
}

void CAnimationController::AdvanceTime(float fTimeElapsed, CGameObject* pRootGameObject)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; i++)
		{
			if (m_pAnimationTracks[i].m_bEnable)
			{
				CAnimationSet* pAnimationSet = m_pAnimationSets->m_ppAnimationSets[m_pAnimationTracks[i].m_nAnimationSet];
				pAnimationSet->Animate(fTimeElapsed * m_pAnimationTracks[i].m_fSpeed, m_pAnimationTracks[i].m_fWeight, m_pAnimationTracks[i].m_fStartTime, m_pAnimationTracks[i].m_fEndTime);
			}
		}

		pRootGameObject->UpdateTransform(NULL);

		for (int k = 0; k < m_nAnimationTracks; k++)
		{
			if (m_pAnimationTracks[k].m_bEnable) m_pAnimationSets->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet]->HandleCallback();
		}

		for (int i = 0; i < m_nAnimationTracks; i++)
		{
			if (m_pAnimationTracks[i].m_bEnable)
			{
				CAnimationSet* pAnimationSet = m_pAnimationSets->m_ppAnimationSets[m_pAnimationTracks[i].m_nAnimationSet];
				if (pAnimationSet->IsOnceLoopEnd())
				{
					SetTrackEnable(i, false);
					SetTrackEnable(0, true);
					pAnimationSet->m_bLoopEnd = false;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CLoadedModelInfo::~CLoadedModelInfo()
{
	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CLoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new CSkinnedMesh * [m_nSkinnedMeshes];
	m_pModelRootObject->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);

	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i]->PrepareSkinning(m_pModelRootObject);
}
