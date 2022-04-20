#include "stdafx.h"
#include "ImGuiManager.h"
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_dx12.h"
#include "../Imgui/imgui_impl_win32.h"

CImGuiManager::CImGuiManager()
{
	IMGUI_CHECKVERSION();	
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

CImGuiManager::~CImGuiManager()
{
	ImGui::DestroyContext();
}

void CImGuiManager::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_bImguiEnable)
	{
		ImGui::Render();
	}
}

void CImGuiManager::Frame()
{
	if (m_bImguiEnable)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void CImGuiManager::OnOffImgui()
{
}

