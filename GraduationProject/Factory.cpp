#include "stdafx.h"
#include "Factory.h"
#include "Scene.h"


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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 60.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 66.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 60.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
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

			float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
			float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
			float terrainY = pTerrain->GetHeight(transX, transZ);

			XMFLOAT3 position = XMFLOAT3(transX, terrainY + 80.0f * sy, transZ);
			pObject->SetPosition(position);
			pObject->SetScale(sx, sy, sz);
			XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
			pObject->Rotate(&xmf4Rotation);
			pObject->Rotate(90.0f, 0.0f, 0.0f);
			pObject->Rotate(0.0f, 0.0f, -180.0f);
			pObject->SetTag("cannon");

			_gameObjects.emplace_back(pObject);
		}
	}
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
		}
		if (name.compare("Zombie_2") == 0) {
			CLoadedModelInfo* pClownModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Assets/Model/WhiteClown.bin", NULL);

			pObject->SetChild(pClownModel->m_pModelRootObject, true);
			pObject->m_pTexture = pClownTexture;
			pObject->m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, CMonsterObject::track_name::length, pClownModel);
		}

		float transX = px * xmf3TerrainScale.x * 257.0f / 150.0f;
		float transZ = pz * xmf3TerrainScale.z * 257.0f / 150.0f;
		float terrainY = pTerrain->GetHeight(transX, transZ);

		XMFLOAT3 position = XMFLOAT3(transX, terrainY + 1.0f * sy, transZ);
		pObject->SetPosition(position);
		pObject->SetScale(sx, sy, sz);
		XMFLOAT4 xmf4Rotation(rx, ry, rz, rw);
		pObject->Rotate(&xmf4Rotation);
		//pObject->Rotate(90.0f, 0.0f, 0.0f);
		//pObject->Rotate(0.0f, 180.0f, 0.0f);

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