#include "stdafx.h"
#include "AnimPlayer.h"
#include "Scene.h"
#include "CollisionManager.h"

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
	SetHp(1000.0f);
	SetMaxHp(1000.0f);
	SetDamage(50.0f);

	SetAnimationTypes();

	string name = "../Assets/Model/Bounding/Knight.txt";
	m_CollManager = new CPlayerCollisionManager(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, dynamic_cast<CGameObject*>(this),name);

	if (pAngrybotModel) delete pAngrybotModel;
}

void CAnimPlayer::InitPlayerMatrics(void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	float x = 980.0f, z = 2900.0f;;
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
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 150.0f, 50.0f));
		m_pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	//case SPACESHIP_CAMERA:
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(3000.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 200.0f, -500.0f));
		m_pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	break;
	default:
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
	if (isMove) return false;

	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case 'Q':
					if (m_pSkinnedAnimationController->GetCurrentTrackNum() == track_name::attack)
						return false;

					m_pSkinnedAnimationController->SwitchAnimationState(track_name::attack);
					m_pSkinnedAnimationController->SetAttackEnable(true);
					SetDamage(50.0f);
					break;
	
				case 'W':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::run);
					m_pSkinnedAnimationController->SetAttackEnable(false);
					break;

				case 'S':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk_back);
					m_pSkinnedAnimationController->SetAttackEnable(false);
					break;

				case 'A':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk_left);
					m_pSkinnedAnimationController->SetAttackEnable(false);
					break;

				case 'D':
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::walk_right);
					m_pSkinnedAnimationController->SetAttackEnable(false);
					break;

				case '1':

					if (m_pSkinnedAnimationController->GetCurrentTrackNum() == track_name::attack_combo)
						return false;

					m_pSkinnedAnimationController->SwitchAnimationState(track_name::attack_combo);
					m_pSkinnedAnimationController->SetAttackEnable(true);
					SetDamage(70.0f);

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
					m_pSkinnedAnimationController->SwitchAnimationState(track_name::idle);
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
	bool bAnimType[track_name::length] = { true, false, true, true, true, true,true,false, false,false,false,false,false,true,false,true,false};
	m_pSkinnedAnimationController->SetAnimationTypes(bAnimType);
}

void CAnimPlayer::DecreaseHp(float val)
{
	m_fHp -= val;
	if (m_fHp < 0)
	{
		m_pSkinnedAnimationController->SwitchAnimationState(track_name::deth);
	}
}

void CAnimPlayer::SetAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pModel)
{
	bool bAnimType[track_name::length] = {false,true,false,false,false,false,false,true,false,false,false,false,true,true,true,false,false};

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, track_name::length, pModel);
	m_pSkinnedAnimationController->SetCurrentTrackNum(track_name::idle);
	m_pSkinnedAnimationController->SetAnimationTracks(bAnimType);
}

bool CAnimPlayer::SetInteraction(XMFLOAT3& center, XMFLOAT4X4& world)
{
	XMFLOAT3 pos = XMFLOAT3(center.x, m_xmf3Position.y, center.z);
	SetPosition(pos);
	SetPlayerLookAtPos(world, center);

	isMove = !isMove;
	int track = (isMove) ? track_name::handling : track_name::idle;
	m_pSkinnedAnimationController->SwitchAnimationState(track);

	if (track == track_name::handling) {
		ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		dynamic_cast<CFirstPersonCamera*>(m_pCamera)->SetLook(GetLookVector());
		return true;
	}
	else {
		ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
		return false;
	}
}


void CAnimPlayer::SetPlayerLookAtPos(DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT3& center)
{
	XMFLOAT3 lookPos = XMFLOAT3(world._41, 0.0f, world._43);
	XMFLOAT3 playerPos = XMFLOAT3(m_xmf3Position.x, 0.0f, m_xmf3Position.z);

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(playerPos, lookPos, m_xmf3Up);
#else
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtRH(m_xmf3Position, xmf3LookAt, xmf3PlayerUp);
#endif
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
