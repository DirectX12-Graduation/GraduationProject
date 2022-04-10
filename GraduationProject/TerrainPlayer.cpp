#include "stdafx.h"
#include "TerrainPlayer.h"
#include "Scene.h"

CTerrainPlayer::CTerrainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int
	nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CLoadedModelInfo* pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/Maria.bin", NULL);
	SetChild(pAngrybotModel->m_pModelRootObject, true);

	SetAnimationTracks(pd3dDevice, pd3dCommandList, pAngrybotModel);
	SetResource(pd3dDevice, pd3dCommandList);
	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	InitPlayerMatrics(pContext);

	m_pSkinnedAnimationController->SetAnimationSetsType(track_name::slash, Animation::Type::Once);
	m_pSkinnedAnimationController->SetAnimationSetsType(track_name::impact, Animation::Type::Once);

	if (pAngrybotModel) delete pAngrybotModel;
}

void CTerrainPlayer::InitPlayerMatrics(void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetPosition(XMFLOAT3(310.0f, pTerrain->GetHeight(310.0f, 595.0f), 595.0f));
	SetScale(XMFLOAT3(0.2f, 0.2f, 0.2f));
	Rotate(0.0f, 0.0f, 90.0f);
}

void CTerrainPlayer::SetResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CTexture* pAnimationTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pAnimationTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/maria_diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pAnimationTexture, Signature::Graphics::animation_diffuse, true);
	m_pTexture = pAnimationTexture;
}

void CTerrainPlayer::SetAnimationTracks(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pModel)
{
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, m_nTracks, pModel);

	for (int i = 0; i < m_nTracks; ++i)
	{
		m_pSkinnedAnimationController->SetTrackAnimationSet(i, i);
		bool bEnable = (i == m_nCurrentTracks) ? true : false;
		m_pSkinnedAnimationController->SetTrackEnable(i, bEnable);
	}
}

CTerrainPlayer::~CTerrainPlayer()
{
}

CCamera* CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		//1��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPACESHIP_CAMERA:
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 50.0f, -100.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	break; default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	Update(fTimeElapsed);

	return(m_pCamera);
}

void CTerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera* p3rdPersonCamera = (CThirdPersonCamera*)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}

void CTerrainPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
}

bool CTerrainPlayer::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case 'Q':
					SwitchAnimationState(track_name::slash);
					break;
	
				case 'W':
				case 'S':
				case 'A':
				case 'D':
					SwitchAnimationState(track_name::run);
					break;

				default:
					break;
			}
			break;

		case WM_KEYUP:
			switch (wParam)
			{
				case 'W':
				case 'S':
				case 'A':
				case 'D':
					SwitchAnimationState(track_name::idle);
					break;
			}

			break;
		default:
			break;
	}
	return(false);
}

void CTerrainPlayer::SwitchAnimationState(int nType)
{
	m_pSkinnedAnimationController->SetTrackEnable(m_nCurrentTracks, false);
	m_nCurrentTracks = nType;
	m_pSkinnedAnimationController->SetTrackEnable(m_nCurrentTracks, true);
}
