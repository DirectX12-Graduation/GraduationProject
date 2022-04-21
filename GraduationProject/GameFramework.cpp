#include "stdafx.h"
#include "GameFramework.h"
#include "../Imgui/imgui_impl_dx12.h"
#include "../Imgui/imgui_impl_win32.h"

CGameFramework::CGameFramework()
{
	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_ppd3dSwapChainBackBuffers[i] = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;


	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework()
{
}

// 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출된다는 것에 유의하라.
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	/*Direct3D 디바이스, 명령 큐와 명령 리스트,
	스왑 체인 등을 생성하는 함수를 호출한다.*/
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
	CreateDepthStencilView();

	BuildObjects();
	//렌더링할 게임 객체를 생성한다. 
	return(true);
}

void CGameFramework::InitImGui(const HWND& hMainWnd)
{
	ImGui_ImplWin32_Init(hMainWnd);
	ImGui_ImplDX12_Init(m_pd3dDevice, m_nSwapChainBuffers,
		DXGI_FORMAT_R8G8B8A8_UNORM, m_pScene->m_pd3dCbvSrvUavDescriptorHeap,
		m_pScene->m_pd3dCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pScene->m_pd3dCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();

	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::CreateSwapChain()
{
#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
#endif

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);

	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break;
	}

	if (!m_pd3dDevice)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}

	if (!m_pd3dDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnCbvSrvUavDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	//직접(Direct) 명령 큐를 생성한다.
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

	//직접(Direct) 명령 할당자를 생성한다.
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);

	//직접(Direct) 명령 리스트를 생성한다.
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_TYPE d3dDescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다. 
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers + 3;
	d3dDescriptorHeapDesc.Type = d3dDescriptorHeapType;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);

	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.
	m_nRtvDescriptorIncrementSize = m_pd3dDevice-> GetDescriptorHandleIncrementSize(d3dDescriptorHeapType);

	d3dDescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // 깊이-스텐실 뷰
	//깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다. 
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = d3dDescriptorHeapType;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);

	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다. 
	m_nDsvDescriptorIncrementSize = m_pd3dDevice-> GetDescriptorHandleIncrementSize(d3dDescriptorHeapType);
}

// 스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다.
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		//m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		m_pd3dSwapChainBackBufferRTVCPUHandles[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;

	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	//d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));

	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	//깊이-스텐실 버퍼를 생성한다.
	d3dClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	//d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	//깊이-스텐실 버퍼 뷰를 생성한다. 
	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	//d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_d3dDsvDescriptorCPUHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, m_d3dDsvDescriptorCPUHandle);
	//m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}


void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// 씬 객체를 생성하고 씬에 포함될 게임 객체들을 생성한다. 
	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	CAnimPlayer* pPlayer = new CAnimPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

	m_pScene->m_pPlayer = m_pPlayer = pPlayer;
	m_pCamera = m_pPlayer->GetCamera();

	CreateShaderVariables();

	//////////////////////////////////////////

	m_nComputeShaders = 2;
	m_ppComputeShaders = new CComputeShader * [m_nComputeShaders];

	CVertComputeShader* pComputeShader1 = new CVertComputeShader();
	pComputeShader1->CreateShader(m_pd3dDevice, m_pScene->m_pd3dComputeRootSignature, FRAME_BUFFER_WIDTH, 1, 1);
	CHorzComputeShader* pComputeShader2 = new CHorzComputeShader();
	pComputeShader2->CreateShader(m_pd3dDevice, m_pScene->m_pd3dComputeRootSignature, 1, FRAME_BUFFER_HEIGHT, 1);

	m_ppComputeShaders[0] = pComputeShader1;
	m_ppComputeShaders[1] = pComputeShader2;

	//////////////////////////////////////////

	m_pPostProcessingShader = new CBlurringShader();
	m_pPostProcessingShader->CreateShader(m_pd3dDevice, m_pScene->GetGraphicsRootSignature(), 1, NULL, DXGI_FORMAT_D32_FLOAT);
	m_pPostProcessingShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * m_nSwapChainBuffers);

	DXGI_FORMAT pdxgiResourceFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	m_pPostProcessingShader->CreateResourcesAndViews(m_pd3dDevice, 3, pdxgiResourceFormats, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, d3dRtvCPUDescriptorHandle, 3); //SRV to (Render Targets) + (Depth Buffer)

	// 씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다. 
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	// 그래픽 명령 리스트들이 모두 실행될 때까지 기다린다.
	WaitForGpuComplete();
	
	// 그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸시킨다.
	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	
	m_GameTimer.Reset();
}

void CGameFramework::CreateShaderVariables()
{
	UINT ncbElementBytes = ((sizeof(CB_FRAMEWORK_INFO) + 255) & ~255);				//256의 배수
	m_pd3dcbFrameworkInfo = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList,
		NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbFrameworkInfo->Map(0, NULL, (void**)&m_pcbMappedFrameworkInfo);
}

void CGameFramework::UpdateShaderVariables()
{
	m_pcbMappedFrameworkInfo->m_fCurrentTime = m_GameTimer.GetTotalTime();
	m_pcbMappedFrameworkInfo->m_fElapsedTime = m_GameTimer.GetTimeElapsed();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFrameworkInfo->GetGPUVirtualAddress();
	m_pd3dCommandList->SetGraphicsRootConstantBufferView(Signature::Graphics::gfw, d3dGpuVirtualAddress);
}

void CGameFramework::ReleaseShaderVariables()
{
	if (m_pd3dcbFrameworkInfo)
	{
		m_pd3dcbFrameworkInfo->Unmap(0, NULL);
		m_pd3dcbFrameworkInfo->Release();
	}
}

void CGameFramework::ReleaseObjects()
{
	ReleaseShaderVariables();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_pPostProcessingShader) m_pPostProcessingShader->ReleaseObjects();
	if (m_pPostProcessingShader) m_pPostProcessingShader->Release();

	if (m_ppComputeShaders) {
		for (int i = 0; i < m_nComputeShaders; ++i)
		{
			if (m_ppComputeShaders[i]) m_ppComputeShaders[i]->ReleaseObjects();
			if (m_ppComputeShaders[i]) m_ppComputeShaders[i]->Release();
		}
		delete[] m_ppComputeShaders;
	}
}

void CGameFramework::OnProcessingMouseMessage
(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		// 마우스 캡쳐를 하고 현재 마우스 위치를 가져온다.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		// 마우스 캡쳐를 해제한다.
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}
void CGameFramework::OnProcessingKeyboardMessage
(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	if (m_pPlayer) m_pPlayer->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_LEFT:
			((CCannonObjectsShader*)m_pScene->m_ppShaders[1])->RotateCannon(XMFLOAT3(0, 0, 1), -10.0f);
			break;
		case VK_RIGHT:
			((CCannonObjectsShader*)m_pScene->m_ppShaders[1])->RotateCannon(XMFLOAT3(0, 0, 1), 10.0f);
			break;
		case VK_UP:
			((CCannonObjectsShader*)m_pScene->m_ppShaders[1])->RotateCannon(XMFLOAT3(1, 0, 0), 10.0f);
			break;
		case VK_DOWN:
			((CCannonObjectsShader*)m_pScene->m_ppShaders[1])->RotateCannon(XMFLOAT3(1, 0, 0), -10.0f);
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
			
		case VK_CONTROL:
			//m_pScene->FireBullet();
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer)
				m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1),
				m_GameTimer.GetTimeElapsed());
			break;
		case VK_F8:
			break;
			//“F9” 키가 눌려지면 윈도우 모드와 전체화면 모드의 전환을 처리한다.
		case VK_F9:
			ChangeSwapChainState();
			break;
		case 'R':
			m_pcbMappedFrameworkInfo->m_nRenderMode = 0x00;
			m_pcbMappedFrameworkInfo->m_nBlurMode = 0x00;
			break;
		case 'D':
			::gbTerrainTessellationWireframe = !::gbTerrainTessellationWireframe;
			m_pcbMappedFrameworkInfo->m_nRenderMode |= DEBUG_TESSELLATION;
			break;
		case 'B':
			m_pcbMappedFrameworkInfo->m_nBlurMode = DEBUG_BLURRING;
			break;
		case 'Q':
			m_fSpeedVal += 10.0f;
			break;
		case 'W':
			m_fSpeedVal -= 10.0f;
			break;
		case 'F':
			((CCannonObjectsShader*)m_pScene->m_ppShaders[1])->ActivateCannon();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage
(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		//OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		//if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		//if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		//if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		//if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		//if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		//if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		
		if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		//if (GetCapture() == m_hWnd)
		//{
		//	SetCursor(NULL);
		//	GetCursorPos(&ptCursorPos);
		//	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		//}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if (dwDirection) m_pPlayer->Move(dwDirection, 5.0f, true);
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	if (m_pScene)
		m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed(),m_pCamera);
	m_pPlayer->Animate(fTimeElapsed,m_pCamera);
}

void CGameFramework::WaitForGpuComplete()
{
	// 원래 후치로 되어있었는데 전치로 변경함
	// CPU 펜스의 값을 증가한다.
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	// GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다. 	
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	/*펜스의 현재 값이 설정한 값보다 작으면
	펜스의 현재 값이 설정한 값이 될 때까지 기다린다.*/
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{

	m_GuiManager.Frame();
	m_pPlayer->SetImGuiCollider();
	m_GuiManager.Render(m_pd3dCommandList);

	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. 
	m_GameTimer.Tick(0.0f);
	ProcessInput();
	AnimateObjects();

	// 명령 할당자와 명령 리스트를 리셋한다.
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pScene->OnPrepareRender(m_pd3dCommandList, m_pCamera);

	UpdateShaderVariables();

	m_pd3dCommandList->ClearDepthStencilView(m_d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pPostProcessingShader->OnPrepareRenderTarget(m_pd3dCommandList, 1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], m_d3dDsvDescriptorCPUHandle);

	m_pScene->Render(m_pd3dCommandList, m_pCamera);

	// 3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다. 
#ifdef _WITH_PLAYER_TOP
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	m_pPostProcessingShader->OnPostRenderTarget(m_pd3dCommandList);

	::SynchronizeResourceTransition(m_pd3dCommandList, (m_pPostProcessingShader->GetTexture())->GetTexture(3),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (m_pScene->m_pd3dComputeRootSignature) m_pd3dCommandList->SetComputeRootSignature(m_pScene->m_pd3dComputeRootSignature);

	// 블러링
	for (int i = 0; i < m_nComputeShaders; i++)
	{
		m_pPostProcessingShader->UpdateTextureShaderVariables(m_pd3dCommandList,m_pScene->m_pd3dCbvSrvUavDescriptorHeap);
		if(m_pScene->m_pPlayer->IsPlayerFast() || m_pcbMappedFrameworkInfo->m_nBlurMode & DEBUG_BLURRING)
			m_ppComputeShaders[i]->Dispatch(m_pd3dCommandList);
	}

	::SynchronizeResourceTransition(m_pd3dCommandList, (m_pPostProcessingShader->GetTexture())->GetTexture(3), 
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	if (m_pScene->m_pd3dGraphicsRootSignature) m_pd3dCommandList->SetComputeRootSignature(m_pScene->m_pd3dGraphicsRootSignature);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], TRUE, NULL);
	m_pPostProcessingShader->Render(m_pd3dCommandList, m_pCamera);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData() ,m_pd3dCommandList);

	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	::ExecuteCommandList(m_pd3dCommandList, m_pd3dCommandQueue, m_pd3dFence, ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

	/*현재의 프레임 레이트를 문자열로 가져와서 주 윈도우의 타이틀로 출력한다. m_pszBuffer 문자열이
	"LapProject ("으로 초기화되었으므로 (m_pszFrameRate+12)에서부터 프레임 레이트를 문자열로 출력
	하여 “ FPS)” 문자열과 합친다.*/

	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();
	//::WaitForGpuComplete(m_pd3dCommandQueue, m_pd3dFence, ++m_nFenceValues[m_nSwapChainBufferIndex], m_hFenceEvent);

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dSwapChainBackBuffers[i])
			m_ppd3dSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth,
		m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}