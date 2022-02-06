#pragma once
#include "Timer.h"
#include "Shader.h"

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

struct MATERIAL
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4				m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
	CScene();
	~CScene();

	// 씬에서 마우스와 키보드 메시지를 처리한다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	void BuildLightsAndMaterials();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed,CCamera* AnimateObjects = NULL);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateRootSignature(ID3D12Device* pd3dDevice, D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags, UINT nRootParameters, D3D12_ROOT_PARAMETER* pd3dRootParameters, UINT nStaticSamplerDescs, D3D12_STATIC_SAMPLER_DESC* pd3dStaticSamplerDescs);
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	ID3D12RootSignature* GetComputeRootSignature() { return(m_pd3dComputeRootSignature); }

	ID3D12RootSignature* CreateComputeRootSignature(ID3D12Device* pd3dDevice);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	CPlayer* m_pPlayer = NULL;

	CHeightMapTerrain* m_pTerrain = NULL;
	CShader** m_ppShaders = NULL;
	int	m_nShaders = 0;

	BoundingBox						m_xmBoundingBox;

	//루트 시그너쳐를 나타내는 인터페이스 포인터이다. 
	ID3D12RootSignature* m_pd3dComputeRootSignature = NULL;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
protected:

	CSkyBox* m_pSkyBox = NULL;
	CRawFormatImage* m_pRawFormatImage;

	LIGHTS* m_pLights = NULL;
	float							m_fLightRotationAngle = 0.0f;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	MATERIALS* m_pMaterials = NULL;

	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;

public:
	CDepthRenderShader* m_pDepthRenderShader = NULL;
	CShadowMapShader* m_pShadowShader = NULL;

};

