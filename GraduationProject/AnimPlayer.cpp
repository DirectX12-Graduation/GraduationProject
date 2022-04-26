#include "stdafx.h"
#include "AnimPlayer.h"
#include "Scene.h"

CAnimPlayer::CAnimPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int
	nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CLoadedModelInfo* pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/Knight.bin", NULL);
	SetChild(pAngrybotModel->m_pModelRootObject, true);
	//LoadFromCollision(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/Bounding/Knight.txt");

	SetAnimationController(pd3dDevice, pd3dCommandList, pAngrybotModel);
	SetResource(pd3dDevice, pd3dCommandList);
	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	InitPlayerMatrics(pContext);

	SetAnimationTypes();

	if (pAngrybotModel) delete pAngrybotModel;
}

void CAnimPlayer::InitPlayerMatrics(void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	float x = 974.0f, z = 1313.0f;
	SetPosition(XMFLOAT3(x * 10, pTerrain->GetHeight(x * 10, z * 10), z * 10));
	SetScale(XMFLOAT3(1, 1, 1));
	Rotate(0.0f, 0.0f, 90.0f);
}

void CAnimPlayer::SetResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CTexture* pAnimationTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pAnimationTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/knight_diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pAnimationTexture, Signature::Graphics::animation_diffuse, true);
	m_pTexture = pAnimationTexture;
}

CAnimPlayer::~CAnimPlayer()
{
}

CCamera* CAnimPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		//1인칭 카메라일 때 플레이어에 y-축 방향으로 중력이 작용한다.
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(3000.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 100.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPACESHIP_CAMERA:
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(3000.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 250.0f, -500.0f));
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

void CAnimPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
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

void CAnimPlayer::OnCameraUpdateCallback(float fTimeElapsed)
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

void CAnimPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationX(0.0f), m_xmf4x4ToParent);
}

void CAnimPlayer::Update(float fTimeElapsed)
{
	CPlayer::Update(fTimeElapsed);
}

void CAnimPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	CPlayer::SetMoveShift(dwDirection, fDistance);
}

bool CAnimPlayer::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case 'Q':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::attack);
					break;
	
				case 'W':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::run);
					break;

				case 'S':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk_back);
					break;

				case 'A':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk_left);
					break;

				case 'D':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk_right);
					break;

				case '1':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::attack_combo);
					break;
				case '2':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::attack_magic);
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
					//SwitchAnimationState(track_name::idle);
					break;
			}
			break;
		default:
			break;
	}
	return(false);
}

void CAnimPlayer::SetAnimationTypes()
{
	bool bAnimType[track_name::length] = { true, false, true, true, true, true,true, false, false,false,false,false,false,false,false,true,false};
	m_pSkinnedAnimationController->SetAnimationTypes(bAnimType);
}

void CAnimPlayer::SetAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pModel)
{
	bool bAnimType[track_name::length] = {false,true,false,false,false,false,false,true,false,false,false,false,true,true,true,false,false};

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, track_name::length, pModel);
	m_pSkinnedAnimationController->SetCurrentTrackNum(track_name::idle);
	m_pSkinnedAnimationController->SetAnimationTracks(bAnimType);
}