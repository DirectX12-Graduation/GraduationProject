#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "AnimPlayer.h"
#include "Shader.h"
#include "ComputeShader.h"

struct CB_FRAMEWORK_INFO
{
	float					m_fCurrentTime;
	float					m_fElapsedTime;
	UINT					m_nRenderMode;
	UINT					m_nBlurMode;
};

#define DEBUG_TESSELLATION			0x20
#define DEBUG_BLURRING				0x21

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	int	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	//DXGI 팩토리 인터페이스에 대한 포인터이다.
	IDXGIFactory4* m_pdxgiFactory = NULL;
	/*스왑 체인 인터페이스에 대한 포인터이다.
	주로 디스플레이를 제어하기 위하여 필요하다.*/
	IDXGISwapChain3* m_pdxgiSwapChain = NULL;
	/*Direct3D 디바이스 인터페이스에 대한 포인터이다.
	주로 리소스를 생성하기 위하여 필요하다.*/
	ID3D12Device* m_pd3dDevice = NULL;

	// MSAA 다중 샘플링을 활성화
	bool m_bMsaa4xEnable = false;
	// MSAA 다중 샘플링 레벨을 설정한다. 
	UINT m_nMsaa4xQualityLevels = 0;

	//스왑 체인의 후면 버퍼의 개수이다. 
	static const UINT m_nSwapChainBuffers = 2;
	//현재 스왑 체인의 후면 버퍼 인덱스이다. 
	UINT m_nSwapChainBufferIndex;

	// 렌더 타겟 버퍼
	//m_ppd3dRenderTargetBuffers -> m_ppd3dSwapChainBackBuffers 이름 변경
	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = NULL;
	// 렌더 타겟 서술자 원소의 크기
	UINT m_nRtvDescriptorIncrementSize;

	// 깊이-스텐실 버퍼
	ID3D12Resource* m_pd3dDepthStencilBuffer = NULL;
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = NULL;
	// 깊이-스텐실 서술자 원소의 크기
	UINT m_nDsvDescriptorIncrementSize = NULL;

	// 명령 큐
	ID3D12CommandQueue* m_pd3dCommandQueue = NULL;
	// 명령(Command) 할당자(Allocator)
	ID3D12CommandAllocator* m_pd3dCommandAllocator = NULL;
	// 명령 리스트 인터페이스 퐁니터
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;

	// 펜스 인터페이스 포인터
	ID3D12Fence* m_pd3dFence;
	// 펜스의 값
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	// 이벤트 핸들
	HANDLE m_hFenceEvent = NULL;

	// 다음은 게임 프레임워크에서 사용할 타이머이다.
	CGameTimer m_GameTimer;
	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다. 
	_TCHAR m_pszFrameRate[50];

	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다. 
	ID3D12PipelineState* m_pd3dPipelineState;

	CScene *m_pScene = NULL;

	// 후처리
	CBlurringShader* m_pPostProcessingShader = NULL;

	CComputeShader** m_ppComputeShaders = NULL;
	int	m_nComputeShaders = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBuffers];

public:
	CCamera* m_pCamera = NULL;
	// 플레이어 객체에 대한 포인터이다.
	CPlayer *m_pPlayer = NULL;
	// 마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다.
	POINT m_ptOldCursorPos;

	float m_fSpeedVal = 50.0f;

protected:
	ID3D12Resource* m_pd3dcbFrameworkInfo = NULL;
	CB_FRAMEWORK_INFO* m_pcbMappedFrameworkInfo = NULL;
public:
	CGameFramework();
	~CGameFramework();

	//프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다). 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	// 스왑 체인 생성
	void CreateSwapChain();
	// 디바이스 생성
	void CreateDirect3DDevice();
	// 명령 큐/할당자/리스트 생성
	void CreateCommandQueueAndList();

	// 서술자 힙 생성
	void CreateRtvAndDsvDescriptorHeaps();

	// 렌더 타겟 뷰 생성
	void CreateRenderTargetViews();
	// 깊이-스텐실 뷰 생성
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.
	void BuildObjects();
	void ReleaseObjects();

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//CPU와 GPU를 동기화하는 함수이다.
	void WaitForGpuComplete();
	void MoveToNextFrame();


	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage
	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage
	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage
	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void CreateShaderVariables();
	void UpdateShaderVariables();
	void ReleaseShaderVariables();
};

