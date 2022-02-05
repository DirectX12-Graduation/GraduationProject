#include "stdafx.h"
#include "Scene.h"
#include "Player.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

ID3D12RootSignature* CScene::CreateRootSignature(ID3D12Device* pd3dDevice, D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags, UINT nRootParameters, D3D12_ROOT_PARAMETER* pd3dRootParameters, UINT nStaticSamplerDescs, D3D12_STATIC_SAMPLER_DESC* pd3dStaticSamplerDescs)
{
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = nRootParameters;
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = nStaticSamplerDescs;
	d3dRootSignatureDesc.pStaticSamplers = pd3dStaticSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3D12RootSignature* pd3dRootSignature = NULL;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dRootSignature);
}


ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[7];

	pd3dDescriptorRanges[Descriptor::Graphics::object].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[Descriptor::Graphics::object].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::object].BaseShaderRegister = 2;
	pd3dDescriptorRanges[Descriptor::Graphics::object].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::object].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::texture].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].BaseShaderRegister = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	pd3dDescriptorRanges[Descriptor::Graphics::terrain].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain].NumDescriptors = 6;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain].BaseShaderRegister = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].BaseShaderRegister = 7;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::g_input].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].NumDescriptors = 3;
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].BaseShaderRegister = 8; //t8~t10 : gtxinput
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::g_output].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].BaseShaderRegister = 11; //t11: gtxoutput
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::depth].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::depth].NumDescriptors = MAX_DEPTH_TEXTURES;
	pd3dDescriptorRanges[Descriptor::Graphics::depth].BaseShaderRegister = 12; //t12: depthbuffer
	pd3dDescriptorRanges[Descriptor::Graphics::depth].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::depth].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[13];

	pd3dRootParameters[Signature::Graphics::player].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::player].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[Signature::Graphics::player].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::player].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::camera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::camera].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[Signature::Graphics::camera].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::camera].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::object].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::object].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::object].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::object];
	pd3dRootParameters[Signature::Graphics::object].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::meterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::meterial].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[Signature::Graphics::meterial].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::meterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::light].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::light].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[Signature::Graphics::light].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::light].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::ToLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::ToLight].Descriptor.ShaderRegister = 5; //ToLight
	pd3dRootParameters[Signature::Graphics::ToLight].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::ToLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::gfw].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::gfw].Descriptor.ShaderRegister = 6; //Framework Info
	pd3dRootParameters[Signature::Graphics::gfw].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::gfw].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::texture].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::texture].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::texture];
	pd3dRootParameters[Signature::Graphics::texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::terrain].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::terrain].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::terrain].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::terrain];
	pd3dRootParameters[Signature::Graphics::terrain].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::skybox].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::skybox].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::skybox].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::skybox];
	pd3dRootParameters[Signature::Graphics::skybox].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::g_input].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::g_input].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::g_input].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::g_input];
	pd3dRootParameters[Signature::Graphics::g_input].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::g_output].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::g_output].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::g_output].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::g_output];
	pd3dRootParameters[Signature::Graphics::g_output].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::depth].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::depth].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::depth].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::depth]; //Depth Buffer
	pd3dRootParameters[Signature::Graphics::depth].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].MipLODBias = 0.0f;
	pd3dSamplerDescs[2].MaxAnisotropy = 1;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //D3D12_COMPARISON_FUNC_LESS
	pd3dSamplerDescs[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE; // D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
	pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice, d3dRootSignatureFlags, _countof(pd3dRootParameters), pd3dRootParameters, _countof(pd3dSamplerDescs), pd3dSamplerDescs);

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::CreateComputeRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[Descriptor::Compute::c_input].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Compute::c_input].NumDescriptors = 3;
	pd3dDescriptorRanges[Descriptor::Compute::c_input].BaseShaderRegister = 8;
	pd3dDescriptorRanges[Descriptor::Compute::c_input].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Compute::c_input].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[Descriptor::Compute::c_output].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[Descriptor::Compute::c_output].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Compute::c_output].BaseShaderRegister = 0;
	pd3dDescriptorRanges[Descriptor::Compute::c_output].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Compute::c_output].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[Signature::Compute::c_input].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Compute::c_input].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Compute::c_input].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Compute::c_input];
	pd3dRootParameters[Signature::Compute::c_input].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Compute::c_output].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Compute::c_output].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Compute::c_output].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Compute::c_output];
	pd3dRootParameters[Signature::Compute::c_output].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	ID3D12RootSignature* pd3dComputeRootSignature = CreateRootSignature(pd3dDevice, d3dRootSignatureFlags, _countof(pd3dRootParameters), pd3dRootParameters, 0, NULL);

	return(pd3dComputeRootSignature);
}

void CScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 2000.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.73f, 0.73f, 0.73f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(-(_PLANE_WIDTH * 0.5f), 512.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(+1.0f, -1.0f, 0.0f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 1000.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.54f, 0.54f, 0.54f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.13f, 0.13f, 0.13f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 120.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.1f, 0.001f);
	m_pLights->m_pLights[1].m_fFalloff = 16.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(60.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights->m_pLights[2].m_bEnable = false;
	m_pLights->m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[2].m_fRange = 500.0f;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.85f, 0.85f, 0.85f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(0.0f, 256.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(+1.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.01f, 0.0001f);
	m_pLights->m_pLights[2].m_fFalloff = 4.0f;
	m_pLights->m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(60.0f));
	m_pLights->m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights->m_pLights[3].m_bEnable = false;
	m_pLights->m_pLights[3].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 1000.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.83f, 0.83f, 0.83f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(0.0f, 128.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(+1.0f, -1.0f, 0.0f);

	//m_pLights->m_pLights[3].m_bEnable = false;
	//m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	//m_pLights->m_pLights[3].m_fRange = 100.0f;
	//m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	//m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	//m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	//m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(0.128f, 0.128f, 0.128f, 1.0f), XMFLOAT4(0.8f, 0.18f, 0.18f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.28f, 0.28f, 0.28f, 1.0f), XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{	
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_pd3dComputeRootSignature = CreateComputeRootSignature(pd3dDevice);

	BuildLightsAndMaterials();

	XMFLOAT3 xmf3Scale(16.0f, 3.0f, 16.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);

	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 5, 5, xmf3Scale, xmf4Color);
#else
//지형을 하나의 격자 메쉬(257x257)로 생성한다.
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, 
	m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/AerialTerrain01.raw"), 257, 257, 257,
	257, xmf3Scale, xmf4Color);
#endif
	m_pRawFormatImage = new CRawFormatImage(L"../Assets/Image/Objects/ObjectsMap03.raw", 257, 257, true);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };

	m_nShaders = 1;
	m_ppShaders = new CShader * [m_nShaders];

	CObjectsShader* pObjectsShader = new CObjectsShader();
	pObjectsShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
	m_ppShaders[0] = pObjectsShader;

	m_pDepthRenderShader = new CDepthRenderShader(pObjectsShader, m_pLights->m_pLights);
	DXGI_FORMAT pdxgiLightRtvFormat[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthRenderShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, 1, pdxgiLightRtvFormat, DXGI_FORMAT_D32_FLOAT);
	m_pDepthRenderShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);

	m_pShadowShader = new CShadowMapShader(pObjectsShader);
	m_pShadowShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	m_pShadowShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pDepthRenderShader->GetDepthTexture());

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) 
		m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dComputeRootSignature)
		m_pd3dComputeRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pDepthRenderShader)
	{
		m_pDepthRenderShader->ReleaseShaderVariables();
		m_pDepthRenderShader->ReleaseObjects();
		m_pDepthRenderShader->Release();
	}

	if (m_pShadowShader)
	{
		m_pShadowShader->ReleaseShaderVariables();
		m_pShadowShader->ReleaseObjects();
		m_pShadowShader->Release();
	}

	ReleaseShaderVariables();

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;

	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;

	if (m_pRawFormatImage) delete m_pRawFormatImage;
	if (m_pPlayer) delete m_pPlayer;
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	if (m_pShadowShader) m_pShadowShader->ReleaseUploadBuffers();
	if (m_pDepthRenderShader) m_pDepthRenderShader->ReleaseUploadBuffers();
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbLightElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbLightElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	UINT ncbMaterialElementBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void**)&m_pcbMappedMaterials);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
}

bool CScene::OnProcessingMouseMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(
	HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed, CCamera* pCamrea)
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->AnimateObjects(fTimeElapsed, pCamrea);
	if (m_pLights) {}
}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	UpdateShaderVariables(pd3dCommandList);

	if (m_pd3dcbMaterials)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials
	}
	if (m_pd3dcbLights)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	
	m_pDepthRenderShader->UpdateShaderVariables(pd3dCommandList);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pShadowShader) m_pShadowShader->Render(pd3dCommandList, pCamera);

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}
}
