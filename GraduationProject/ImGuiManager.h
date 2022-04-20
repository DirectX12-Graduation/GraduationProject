#pragma once
class CImGuiManager
{
public:
	CImGuiManager();
	~CImGuiManager();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void Frame();
	void OnOffImgui();
	
	bool IsImguiEnable() { return m_bImguiEnable; }
	bool m_bImguiEnable = true;
};

