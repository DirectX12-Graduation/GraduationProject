#include "stdafx.h"
#include "Factory.h"
#include "Scene.h"
#include "Player.h"
#define terrainSizeOffset 220.0f / 150.0f
#define terrainXOffset 1000.0f;
#define terrainZOffset 1000.0f;

void CFactory::AnimateObjects(float fTimeElapsed, CCamera* pCamrea)
{
	for (auto &object : _gameObjects)
		object->Animate(fTimeElapsed, pCamrea);
}

void CFactory::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto &object : _gameObjects)
		object->Render(pd3dCommandList, pCamera);
}

void CFactory::SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	string root = "../Assets/Model/Bounding/";
	string tail = ".txt";
	for (int i = 0; i < _gameObjects.size(); i++)
	{
		string tag = _gameObjects[i]->GetTag();
		string filename = "../Assets/Model/Bounding/" + tag + ".txt";
		CCollisionManager* manager = new CCollisionManager(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, _gameObjects[i], filename);
		_gameObjects[i]->SetCollisionManager(manager);
		_gameObjects[i]->UpdateTransform(nullptr);
	}
}

//////


void CObjectFactory::BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();
	XMFLOAT3 xmf3TerrainScale = pTerrain->GetScale();

	ifstream objectsInfo("../Assets/Image/Terrain/ObjectsInfo.txt");

	// crystal
	CTexture* pCrystalTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pCrystalTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/crystal_17_2.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pCrystalTexture, Signature::Graphics::model_diffuse, true);

	// barricade
	CTexture* pCoverTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pCoverTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/Wooden_Barricades_AlbedoTransparency.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pCoverTexture, Signature::Graphics::model_diffuse, true);

	// house_1
	CTexture* pHouse1Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pHouse1Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/house_1_Diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pHouse1Texture, Signature::Graphics::model_diffuse, true);
	// house_2
	CTexture* pHouse2Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pHouse2Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/house_2_Diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pHouse2Texture, Signature::Graphics::model_diffuse, true);
	// house_3
	CTexture* pHouse3Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pHouse3Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/house_3_Diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pHouse3Texture, Signature::Graphics::model_diffuse, true);
	// house_4
	CTexture* pHouse4Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pHouse4Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/house_4_Diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pHouse4Texture, Signature::Graphics::model_diffuse, true);
	float houseOffsetX = 30.0f;
	float houseOffsetX_ = 1500.0f;
	float houseOffsetZ = 20.0f;
	float houseOffsetZ_ = 18000.0f;

	// floor_segment
	CTexture* pFloorTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pFloorTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/floor_segment_updated.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pFloorTexture, Signature::Graphics::model_diffuse, true);

	// 
	string line;
	smatch match;
	regex reName(R"(name: (\w+))");
	regex rePosition(R"(position: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	regex reRotation(R"(rotation: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	regex reScale(R"(scale: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	while (getline(objectsInfo, line)) {
		regex_match(line, match, reName);
		string name = match[1].str();

		getline(objectsInfo, line);
		regex_match(line, match, rePosition);
		float px = stof(match[1].str());
		float py = stof(match[2].str());
		float pz = stof(match[3].str());

		getline(objectsInfo, line);
		regex_match(line, match, reRotation);
		float rx = stof(match[1].str());
		float ry = stof(match[2].str());
		float rz = stof(match[3].str());
		float rw = stof(match[4].str());

		getline(objectsInfo, line);
		regex_match(line, match, reScale);
		float sx = stof(match[1].str());
		float sy = stof(match[2].str());
		float sz = stof(match[3].str());


		if (name.compare("Barricade_01") == 0) {
			CCoverObject* pObject = NULL;

			CLoadedModelInfo* pCover1Model = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/Barricade_02.bin", NULL);
			pObject = new CCoverObject;
			pObject->SetChild(pCover1Model->m_pModelRootObject, true);

			pObject->m_pTexture = pCoverTexture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 20.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, -180.0f);

			pObject->SetTag("Barricade_02");

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("Barricade_02") == 0) {
			CCoverObject* pObject = NULL;

			CLoadedModelInfo* pCover2Model = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/Barricade_02.bin", NULL);

			pObject = new CCoverObject;
			pObject->SetChild(pCover2Model->m_pModelRootObject, true);

			pObject->m_pTexture = pCoverTexture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 20.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, -180.0f);

			pObject->SetTag("Barricade_02");

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("crystal") == 0) {
			CGameObject* pObject = NULL;

			pObject = new CGameObject;
			CLoadedModelInfo* pCrystalModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/crystal_17_2.bin", NULL);
			pObject->SetChild(pCrystalModel->m_pModelRootObject, true);

			pObject->m_pTexture = pCrystalTexture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 1.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(1, 1, 1);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, -180.0f);

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("floor_segment") == 0) {
			CGameObject* pObject = NULL;
			CLoadedModelInfo* pFloorModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/floor_segment.bin", NULL);

			pObject = new CGameObject;
			pObject->SetChild(pFloorModel->m_pModelRootObject, true);

			pObject->m_pTexture = pFloorTexture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 1.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(100, 100, 100);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, -180.0f);

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("house_1") == 0) {
			CGameObject* pObject = NULL;
			CLoadedModelInfo* pHouse1Model = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/house_1.bin", NULL);

			pObject = new CGameObject;
			pObject->SetChild(pHouse1Model->m_pModelRootObject, true);

			pObject->m_pTexture = pHouse1Texture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 60.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, 0.0f);
			pObject->Rotate(90.0f, 0.0f, 0.0f);
			pHouse1Model->m_pModelRootObject->Rotate(0.0f,-180.0f, 0.0f);
			pObject->SetTag("house_1");

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("house_2") == 0) {
			CGameObject* pObject = NULL;
			CLoadedModelInfo* pHouse2Model = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/house_2.bin", NULL);

			pObject = new CGameObject;
			pObject->SetChild(pHouse2Model->m_pModelRootObject, true);

			pObject->m_pTexture = pHouse2Texture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 66.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, 0.0f);
			pObject->Rotate(90.0f, 0.0f, 0.0f);
			pHouse2Model->m_pModelRootObject->Rotate(0.0f, -180.0f, 0.0f);
			pObject->SetTag("house_2");

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("house_3") == 0) {
			CGameObject* pObject = NULL;
			CLoadedModelInfo* pHouse3Model = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/house_3.bin", NULL);


			pObject = new CGameObject;
			pObject->SetChild(pHouse3Model->m_pModelRootObject, true);


			pObject->m_pTexture = pHouse3Texture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 90.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(90.0f, 0.0f, 0.0f);
			pHouse3Model->m_pModelRootObject->Rotate(0.0f, -180.0f, 0.0f);
			pObject->SetTag("house_3");

			_gameObjects.emplace_back(pObject);
		}
		else if (name.compare("house_4") == 0) {
			CGameObject* pObject = NULL;
			CLoadedModelInfo* pHouse4Model = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/house_4.bin", NULL);
			pObject = new CGameObject;
			pObject->SetChild(pHouse4Model->m_pModelRootObject, true);

			pObject->m_pTexture = pHouse4Texture;

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 60.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(0.0f, 180.0f, 0.0f);
			pObject->Rotate(90.0f, 0.0f, 0.0f);
			pHouse4Model->m_pModelRootObject->Rotate(0.0f, -180.0f, 0.0f);
			pObject->SetTag("house_4");

			_gameObjects.emplace_back(pObject);
		}
	}

}

///


void CCannonFactory::BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();
	XMFLOAT3 xmf3TerrainScale = pTerrain->GetScale();

	ifstream objectsInfo("../Assets/Image/Terrain/ObjectsInfo.txt");

	// cannon
	CTexture* pCannonTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pCannonTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/cannon_diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pCannonTexture, Signature::Graphics::model_diffuse, true);

	// cannonball
	CCannonballObject* pCannonballObject = new CCannonballObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pCannonballObject->SetUpdatedContext(pTerrain);

	// 
	string line;
	smatch match;
	regex reName(R"(name: (\w+))");
	regex rePosition(R"(position: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	regex reRotation(R"(rotation: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	regex reScale(R"(scale: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	while (getline(objectsInfo, line)) 
	{
		regex_match(line, match, reName);
		string name = match[1].str();

		getline(objectsInfo, line);
		regex_match(line, match, rePosition);
		float px = stof(match[1].str());
		float py = stof(match[2].str());
		float pz = stof(match[3].str());

		getline(objectsInfo, line);
		regex_match(line, match, reRotation);
		float rx = stof(match[1].str());
		float ry = stof(match[2].str());
		float rz = stof(match[3].str());
		float rw = stof(match[4].str());

		getline(objectsInfo, line);
		regex_match(line, match, reScale);
		float sx = stof(match[1].str());
		float sy = stof(match[2].str());
		float sz = stof(match[3].str());

		if (name.compare("cannon") == 0) {
			CCannonObject* pObject = NULL;
			CLoadedModelInfo* pCannonModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/cannon.bin", NULL);

			pObject = new CCannonObject;
			pObject->SetChild(pCannonModel->m_pModelRootObject, true);

			pObject->m_pTexture = pCannonTexture;

			pObject->SetCannonball(pCannonballObject);

			float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
			float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 50.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(-90.0f, 0.0f, 0.0f);
			pObject->SetTag("cannon");

			_gameObjects.emplace_back(pObject);
		}
	}
}

void CCannonFactory::ActiveCannon()
{
	if (!m_pInteractedCannon) return;

	CGameObject* pBarrel = m_pInteractedCannon->m_pChild->FindFrame("Cube_001");
	XMFLOAT3 origin = pBarrel->GetPosition();
	XMFLOAT3 offset = Vector3::ScalarProduct(pBarrel->GetUp(), 100.0f * m_pInteractedCannon->m_xmf3Scale.y);
	origin = Vector3::Add(origin, offset);
	XMFLOAT3 velocity = Vector3::ScalarProduct(pBarrel->GetUp(), 5.0f);

	m_pInteractedCannon->FireCannonBall(origin, velocity);
}

void CCannonFactory::RotateCannon(WPARAM wParam)
{
	if (!m_pInteractedCannon) return;

	XMFLOAT3 xmf3RotateAxis;
	float fAngle;

	switch (wParam) {
	case 'W':
		xmf3RotateAxis = XMFLOAT3(1, 0, 0);
		fAngle = +10.0f;
		break;
	case 'A':
		xmf3RotateAxis = XMFLOAT3(0, 0, 1);
		fAngle = -10.0f;
		break;
	case 'S':
		xmf3RotateAxis = XMFLOAT3(1, 0, 0);
		fAngle = -10.0f;
		break;
	case 'D':
		xmf3RotateAxis = XMFLOAT3(0, 0, 1);
		fAngle = +10.0f;
		break;
	}

	m_pInteractedCannon->RotateCannon(&xmf3RotateAxis, fAngle);
}

XMFLOAT3& CCannonFactory::GetCannonPosition()
{
	if (!m_pInteractedCannon) return XMFLOAT3(0.0f, 0.0f, 0.0f);
	return m_pInteractedCannon->GetCannonball()->GetPosition();
}

///

void CMonsterFactory::BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();
	XMFLOAT3 xmf3TerrainScale = pTerrain->GetScale();

	ifstream objectsInfo("../Assets/Image/Terrain/ObjectsInfo.txt");

	// Zombie_1
		CTexture* pZombieTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pZombieTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/zombie_diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pZombieTexture, Signature::Graphics::animation_diffuse, true);

	// Zombie_2
		CTexture* pClownTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pClownTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/Texture/whiteclown_diffuse.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pClownTexture, Signature::Graphics::animation_diffuse, true);

	string line;
	smatch match;
	regex reName(R"(name: (\w+))");
	regex rePosition(R"(position: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	regex reRotation(R"(rotation: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");
	regex reScale(R"(scale: \(([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*), ([+-]?\d*[.]?\d*)\))");


	while (getline(objectsInfo, line)) {
		regex_match(line, match, reName);
		string name = match[1].str();

		getline(objectsInfo, line);
		regex_match(line, match, rePosition);
		float px = stof(match[1].str());
		float py = stof(match[2].str());
		float pz = stof(match[3].str());

		getline(objectsInfo, line);
		regex_match(line, match, reRotation);
		float rx = stof(match[1].str());
		float ry = stof(match[2].str());
		float rz = stof(match[3].str());
		float rw = stof(match[4].str());

		getline(objectsInfo, line);
		regex_match(line, match, reScale);
		float sx = stof(match[1].str());
		float sy = stof(match[2].str());
		float sz = stof(match[3].str());

		if (name.compare("Zombie_1") != 0 && name.compare("Zombie_2") != 0) {
			continue;
		}

		//
		CMonsterObject* pObject = NULL;

		pObject = new CMonsterObject;
		pObject->SetUpdatedContext(pTerrain);

		if (name.compare("Zombie_1") == 0) {
			CLoadedModelInfo* pZombieModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/ZombieGirl.bin", NULL);

			pObject->SetChild(pZombieModel->m_pModelRootObject, true);
			pObject->m_pTexture = pZombieTexture;
			pObject->m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, CMonsterObject::track_name::length, pZombieModel);
			pObject->SetHp(200.0f);
			pObject->SetMaxHp(200.0f);
			pObject->SetDamage(30.0f);
		}
		if (name.compare("Zombie_2") == 0) {
			CLoadedModelInfo* pClownModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/WhiteClown.bin", NULL);

			pObject->SetChild(pClownModel->m_pModelRootObject, true);
			pObject->m_pTexture = pClownTexture;
			pObject->m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, CMonsterObject::track_name::length, pClownModel);
			pObject->SetHp(250.0f);
			pObject->SetMaxHp(250.0f);
			pObject->SetDamage(50.0f);
		}

		float transX = px * xmf3TerrainScale.x * terrainSizeOffset + terrainXOffset;
		float transZ = pz * xmf3TerrainScale.z * terrainSizeOffset + terrainZOffset;
		float terrainY = pTerrain->GetHeight(transX, transZ);

		XMFLOAT3 position = XMFLOAT3(transX, terrainY + 1.0f * sy, transZ);
		pObject->SetPosition(position);
		pObject->SetScale(sx, sy, sz);
		XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
		pObject->Rotate(&xmf4Rotation);

		int TrackNum = CMonsterObject::track_name::idle1;

		pObject->m_pSkinnedAnimationController->SetCurrentTrackNum(TrackNum);
		bool bTrackAnimType[CMonsterObject::track_name::length] = { false,false,false,false,false,false,false };

		pObject->m_pSkinnedAnimationController->SetAnimationTracks(bTrackAnimType);

		bool bAnimType[CMonsterObject::track_name::length] = { false, false, false, false, true, true, true };
		pObject->m_pSkinnedAnimationController->SetAnimationTypes(bAnimType);
		pObject->m_pSkinnedAnimationController->SetIdleNum(CMonsterObject::track_name::idle1);

		pObject->SetTag("Monster");

		_gameObjects.emplace_back(pObject);
	}
}

void CMonsterFactory::SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	string root = "../Assets/Model/Bounding/";
	string tail = ".txt";
	for (int i = 0; i < _gameObjects.size(); i++)
	{
		string tag = _gameObjects[i]->GetTag();
		string filename = "../Assets/Model/Bounding/" + tag + ".txt";
		CPlayerCollisionManager* manager = new CPlayerCollisionManager(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, _gameObjects[i], filename);
		_gameObjects[i]->SetCollisionManager(manager);
		_gameObjects[i]->UpdateTransform(nullptr);
	}
}

void CMonsterFactory::FindTarget(CGameObject* pObject)
{
	for (auto& monster : _gameObjects)
	{
		dynamic_cast<CMonsterObject*>(monster)->FindTarget(pObject);
	}
}

/////////////////////////

void CUIFactory::BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext,CPlayer* pPlayer)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	m_pCamera = new CCamera();
	m_pCamera->GenerateOrthographicProjectionMatrix(0.0f,1.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	///

	CGameObject* pObject = NULL;
	pObject = new CUIObject();
	CTexture* phptexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	phptexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/hp.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, phptexture, Signature::Graphics::texture, true);
	CBillboardMesh* pMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 150.0f, 15.0f);

	CGameObject* pTarget = pPlayer;
	dynamic_cast<CUIObject*>(pObject)->SetTarget(pTarget);

	pObject->SetPosition(-FRAME_BUFFER_WIDTH / 2 + 90, -FRAME_BUFFER_HEIGHT / 2 + 15, 0.0f);
	pObject->SetMesh(pMesh);
	pObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CHpShader* m_pShader = new CHpShader();

	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);

	CMaterial* m_pMaterial = new CMaterial(1);
	m_pMaterial->SetTexture(phptexture);
	m_pMaterial->SetShader(m_pShader);
	pObject->SetMaterial(0, m_pMaterial);
	_gameObjects.emplace_back(pObject);

	////

	CGameObject* pMonsterUIObject = NULL;
	pMonsterUIObject = new CUIObject();
	CBillboardMesh* pMesh2 = new CBillboardMesh(pd3dDevice, pd3dCommandList, 200.0f, 20.0f);
	pMonsterUIObject->SetPosition(0.0f, FRAME_BUFFER_HEIGHT/4 + FRAME_BUFFER_HEIGHT/6, 0.0f);
	pMonsterUIObject->SetMesh(pMesh2);
	pMonsterUIObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	pMonsterUIObject->SetMaterial(0, m_pMaterial);
	_gameObjects.emplace_back(pMonsterUIObject);

}

void CUIFactory::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);
	for (auto& object : _gameObjects)
	{
		if (!dynamic_cast<CUIObject*>(object)->IsTarget()) continue;
		object->Render(pd3dCommandList, pCamera);
	}
	pCamera->UpdateShaderVariables(pd3dCommandList);
}

void CUIFactory::AnimateObjects(float fTimeElapsed, CCamera* pCamrea)
{
	for (auto& object : _gameObjects)
	{
		object->Animate(fTimeElapsed, m_pCamera);
	}
}

void CUIFactory::SetTargetMonster(CGameObject* pObject)
{
	dynamic_cast<CUIObject*>(_gameObjects[1])->SetTarget(pObject);
}

///////////////////////////////

void CParticleFactory::BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1, 0, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Effect/Fire0.dds", 0);

	CScene::CreateShaderResourceViews(pd3dDevice, pTexture, Signature::Graphics::particle_texture, true);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetTexture(pTexture);
	XMFLOAT3 xmf3Position = { 0.0f,0.0f,0.0f };
	XMFLOAT2 xmf2Size = { 10.0f,10.0f };
	CParticleMesh* ParticleMesh = new CParticleMesh(pd3dDevice, pd3dCommandList, xmf3Position, xmf2Size);

	float fxPitch = 1500.0f * 3.5f;
	float fzPitch = 1500.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = int(fTerrainWidth / fxPitch);
	int zObjects = int(fTerrainLength / fzPitch);


	CParticleShader* pObjectShader = new CParticleShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	pObjectShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pMaterial->SetShader(pObjectShader);

	CParticleSystem* pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			float xPosition = x * fxPitch;
			float zPosition = z * fzPitch;
			float fHeight = pTerrain->GetHeight(xPosition, zPosition);

			XMFLOAT3 xmfRange(10.0f, 10.0f, 10.0f);

			XMFLOAT3 xmf3Position(xPosition, fHeight + 30.0f, zPosition);
			pRotatingObject = new CBreathParticle(pd3dDevice, pd3dCommandList, xmf3Position, xmfRange, 500);

			pRotatingObject->SetMesh(ParticleMesh);
			pRotatingObject->SetMaterial(0, pMaterial);

			_gameObjects.emplace_back(pRotatingObject);
		}
	}
}
