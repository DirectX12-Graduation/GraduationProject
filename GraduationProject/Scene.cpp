#include "stdafx.h"
#include "Scene.h"
#include "Player.h"
#include "Collision.h"
#include "CollisionManager.h"

ID3D12DescriptorHeap* CScene::m_pd3dCbvSrvUavDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dUavCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dUavGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dUavCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dUavGPUDescriptorNextHandle;

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

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[Descriptor::Graphics::length];

	pd3dDescriptorRanges[Descriptor::Graphics::texture].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].BaseShaderRegister = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::texture].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	pd3dDescriptorRanges[Descriptor::Graphics::terrain_textures].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain_textures].NumDescriptors = 5; //gtxtTerrainBaseTexture, gtxtTerrainDetailTexture[3], gtxtTerrainAlphaTexture
	pd3dDescriptorRanges[Descriptor::Graphics::terrain_textures].BaseShaderRegister = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain_textures].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::terrain_textures].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::skybox].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].BaseShaderRegister = 6;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::skybox].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::g_input].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].NumDescriptors = 3;
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].BaseShaderRegister = 7; //t4~t6 : gtxinput
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::g_input].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::g_output].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].BaseShaderRegister = 10; //t7: gtxoutput
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::g_output].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::animation_diffuse].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::animation_diffuse].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::animation_diffuse].BaseShaderRegister = 11; // t8 animation_diffuse
	pd3dDescriptorRanges[Descriptor::Graphics::animation_diffuse].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::animation_diffuse].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::model_diffuse].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::model_diffuse].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::model_diffuse].BaseShaderRegister = 12; //t7: model_diffuse
	pd3dDescriptorRanges[Descriptor::Graphics::model_diffuse].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::model_diffuse].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::particle_texture].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_texture].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_texture].BaseShaderRegister = 13;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_texture].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_texture].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[Descriptor::Graphics::particle_buffer].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_buffer].NumDescriptors = 1;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_buffer].BaseShaderRegister = 14;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_buffer].RegisterSpace = 0;
	pd3dDescriptorRanges[Descriptor::Graphics::particle_buffer].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER pd3dRootParameters[Signature::Graphics::length];

	pd3dRootParameters[Signature::Graphics::player].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::player].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[Signature::Graphics::player].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::player].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::camera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::camera].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[Signature::Graphics::camera].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::camera].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::object].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[Signature::Graphics::object].Constants.Num32BitValues = 20; //Game Object
	pd3dRootParameters[Signature::Graphics::object].Constants.ShaderRegister = 2;
	pd3dRootParameters[Signature::Graphics::object].Constants.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::object].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::meterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::meterial].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[Signature::Graphics::meterial].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::meterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::light].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::light].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[Signature::Graphics::light].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::light].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::gfw].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::gfw].Descriptor.ShaderRegister = 6; //Framework Info
	pd3dRootParameters[Signature::Graphics::gfw].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::gfw].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::bone_offsets].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::bone_offsets].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[Signature::Graphics::bone_offsets].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::bone_offsets].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[Signature::Graphics::bone_trans].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::bone_trans].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[Signature::Graphics::bone_trans].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::bone_trans].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[Signature::Graphics::hp].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::hp].Descriptor.ShaderRegister = 9; // hp
	pd3dRootParameters[Signature::Graphics::hp].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::hp].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::particle].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[Signature::Graphics::particle].Descriptor.ShaderRegister = 10; // particle
	pd3dRootParameters[Signature::Graphics::particle].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[Signature::Graphics::particle].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::texture].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::texture].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::texture];
	pd3dRootParameters[Signature::Graphics::texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::terrain_textures].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::terrain_textures].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::terrain_textures].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::terrain_textures];
	pd3dRootParameters[Signature::Graphics::terrain_textures].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

	pd3dRootParameters[Signature::Graphics::animation_diffuse].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::animation_diffuse].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::animation_diffuse].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::animation_diffuse];
	pd3dRootParameters[Signature::Graphics::animation_diffuse].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	
	pd3dRootParameters[Signature::Graphics::model_diffuse].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::model_diffuse].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::model_diffuse].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::model_diffuse];
	pd3dRootParameters[Signature::Graphics::model_diffuse].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[Signature::Graphics::particle_texture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::particle_texture].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::particle_texture].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::particle_texture];
	pd3dRootParameters[Signature::Graphics::particle_texture].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[Signature::Graphics::particle_buffer].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[Signature::Graphics::particle_buffer].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[Signature::Graphics::particle_buffer].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[Descriptor::Graphics::particle_buffer]; 
	pd3dRootParameters[Signature::Graphics::particle_buffer].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

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

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
	pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice, d3dRootSignatureFlags, _countof(pd3dRootParameters), pd3dRootParameters, _countof(pd3dSamplerDescs), pd3dSamplerDescs);

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::CreateComputeRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[Descriptor::Compute::c_input].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[Descriptor::Compute::c_input].NumDescriptors = 3;
	pd3dDescriptorRanges[Descriptor::Compute::c_input].BaseShaderRegister = 7;
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

	CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 300,1); //Gunship(2), Player:Mi24(1), Angrybot()
	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildLightsAndMaterials();

	XMFLOAT3 xmf3Scale(80.0f, 20.0f, 80.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.3f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/terrain.raw"), 257, 514, xmf3Scale, xmf4Color);
	m_pRawFormatImage = new CRawFormatImage(L"../Assets/Image/Objects/ObjectsMap03.raw", 257, 257, true);

	m_pNavMesh = new CNavMesh(pd3dDevice, pd3dCommandList, xmf3Scale);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	CObjectFactory * pObject = new CObjectFactory();
	pObject->BuildObjects(pd3dDevice, m_pd3dGraphicsRootSignature, pd3dCommandList, m_pTerrain);
	pObject->SetObjectCollision(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	_factory.emplace_back(pObject);

	CCannonFactory* pCannon = new CCannonFactory();
	pCannon->BuildObjects(pd3dDevice, m_pd3dGraphicsRootSignature, pd3dCommandList, m_pTerrain);
	pCannon->SetObjectCollision(pd3dDevice, pd3dCommandList,m_pd3dGraphicsRootSignature);
	_factory.emplace_back(pCannon);

	CMonsterFactory* pMonster = new CMonsterFactory();
	pMonster->BuildObjects(pd3dDevice, m_pd3dGraphicsRootSignature, pd3dCommandList, m_pTerrain);
	pMonster->SetObjectCollision(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	_factory.emplace_back(pMonster);

	_particles = new CParticleFactory();
	_particles->BuildObjects(pd3dDevice, m_pd3dGraphicsRootSignature, pd3dCommandList, m_pTerrain);

	m_pBoss = new CBossMonster(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	//////
	
	BuildCollisions(pd3dDevice, pd3dCommandList);


	////////

	//float fHeight = m_pTerrain->GetHeight(x * 10, z * 10);
	//pRotatingObject = new CStreamParticleObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, XMFLOAT3(x*10, fHeight, z*10), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(15.0f, 15.0f), 15.0f, MAX_PARTICLES);

	//CStreamExplosionObject* emptyObject = NULL;
	//emptyObject = new CStreamExplosionObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, XMFLOAT3(x * 10, fHeight, z * 10), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(15.0f, 15.0f), 15.0f, MAX_PARTICLES);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::BuildUIObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	_ui = new CUIFactory();
	((CUIFactory*)_ui)->BuildObjects(pd3dDevice, m_pd3dGraphicsRootSignature, pd3dCommandList, m_pTerrain, m_pPlayer);
}

void CScene::RenderParticle(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//_particles->Render(pd3dCommandList, pCamera);
}

void CScene::BuildCollisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	ifstream boundingInfo("../Assets/Model/Bounding/scene.txt");
	string s;
	XMFLOAT3 center, extends;
	while (boundingInfo >> s)
	{
		if (s.compare("<Box>:") == 0)
		{
			boundingInfo >> center.x >> center.y >> center.z;
			boundingInfo >> extends.x >> extends.y >> extends.z;
			BoundingBox BB;
			XMStoreFloat3(&BB.Center, XMLoadFloat3(&center));
			XMStoreFloat3(&BB.Extents, XMLoadFloat3(&extends));

			CCollision* cols = new CBBCollision(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, BB);
			collisions.emplace_back(cols);
		}
	}
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
	vector<CGameObject*> test = _factory[2]->GetGameObjects();
	for (int i = 0; i < test.size(); ++i) {
		((CMonsterObject*)(test[i]))->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	}

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			case 'E':
				CheckInteraction();
				break;
			case VK_SPACE:
				dynamic_cast<CCannonFactory*>(_factory[1])->ActiveCannon();
				//m_ppParticleObjects[0]->SetActive(false);
				::gnPatricleMode = 0x00;
				break;
			case 'W': case 'A': case 'S': case 'D':
				dynamic_cast<CCannonFactory*>(_factory[1])->RotateCannon(wParam);
				break;
			case 'B':
				int n = m_pBoss->m_pSkinnedAnimationController->GetCurrentTrackNum();
				n++;
				if (n == CBossMonster::track_name::length) n = 0;
				m_pBoss->m_pSkinnedAnimationController->SwitchAnimationState(n);
				m_pBoss->m_pSkinnedAnimationController->SetIdleNum(n);
				break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void CScene::CreateCbvSrvUavDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews, int nUnorderedAccessViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews + nUnorderedAccessViews; //CBVs + SRVs + UAVs
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvUavDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvUavDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvUavDescriptorIncrementSize * nConstantBufferViews);
	m_d3dUavCPUDescriptorNextHandle.ptr = m_d3dUavCPUDescriptorStartHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr + (::gnCbvSrvUavDescriptorIncrementSize * nShaderResourceViews);
	m_d3dUavGPUDescriptorNextHandle.ptr = m_d3dUavGPUDescriptorStartHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr + (::gnCbvSrvUavDescriptorIncrementSize * nShaderResourceViews);

}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvUavDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvUavDescriptorIncrementSize;
	}
	return(d3dCbvGPUDescriptorHandle);
}

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

D3D12_UNORDERED_ACCESS_VIEW_DESC GetUnorderedAccessViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC d3dUnorderedAccessViewDesc;
	d3dUnorderedAccessViewDesc.Format = d3dResourceDesc.Format;

	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		d3dUnorderedAccessViewDesc.Texture2D.MipSlice = 0;
		d3dUnorderedAccessViewDesc.Texture2D.PlaneSlice = 0;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		d3dUnorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
		d3dUnorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dUnorderedAccessViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		d3dUnorderedAccessViewDesc.Texture2DArray.PlaneSlice = 0;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dUnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		d3dUnorderedAccessViewDesc.Buffer.FirstElement = 0;
		d3dUnorderedAccessViewDesc.Buffer.NumElements = 0;
		d3dUnorderedAccessViewDesc.Buffer.StructureByteStride = 0;
		d3dUnorderedAccessViewDesc.Buffer.CounterOffsetInBytes = 0;
		d3dUnorderedAccessViewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		break;
	}
	return(d3dUnorderedAccessViewDesc);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nRootParameter, bool bAutoIncrement, bool isGetTextureValue)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
			if (isGetTextureValue == false)
				d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			else 
				d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
			pTexture->SetSrvGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
		}
	}

	int nRootParameters = pTexture->GetGraphicsSrvRootParameters();
	for (int i = 0; i < nRootParameters; ++i)
		pTexture->SetGraphicsSrvRootParameterIndex(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, i);

	return(d3dSrvGPUDescriptorHandle);
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetTexture(nIndex);
	int nTextureType = pTexture->GetTextureType();
	if (pShaderResource)
	{
		ID3D12Resource* pShaderResource = pTexture->GetTexture(nIndex);
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
		pTexture->SetSrvGpuDescriptorHandle(nIndex, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateUnorderedAccessViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dUavGPUDescriptorNextHandle;
	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_UNORDERED_ACCESS_VIEW_DESC d3dUnorderedAccessViewDesc = GetUnorderedAccessViewDesc(d3dResourceDesc, nTextureType);
			pd3dDevice->CreateUnorderedAccessView(pShaderResource,NULL, &d3dUnorderedAccessViewDesc, m_d3dUavCPUDescriptorNextHandle);
			m_d3dUavCPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
			pTexture->SetUavGpuDescriptorHandle(i, m_d3dUavGPUDescriptorNextHandle);
			m_d3dUavGPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
		}
	}

	return(d3dSrvGPUDescriptorHandle);
}

void CScene::CreateUnorderedAccessView(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetTexture(nIndex);
	int nTextureType = pTexture->GetTextureType();
	if (pShaderResource)
	{
		ID3D12Resource* pShaderResource = pTexture->GetTexture(nIndex);
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_UNORDERED_ACCESS_VIEW_DESC d3dUnorderedAccessViewDesc = GetUnorderedAccessViewDesc(d3dResourceDesc, nTextureType);
		pd3dDevice->CreateUnorderedAccessView(pShaderResource, NULL, &d3dUnorderedAccessViewDesc, m_d3dUavCPUDescriptorNextHandle);
		m_d3dUavCPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
		pTexture->SetUavGpuDescriptorHandle(nIndex, m_d3dUavGPUDescriptorNextHandle);
		m_d3dUavGPUDescriptorNextHandle.ptr += ::gnCbvSrvUavDescriptorIncrementSize;
	}
}

void CScene::AnimateObjects(float fTimeElapsed, CCamera* pCamera)
{
	m_fElapsedTime = fTimeElapsed;

	for (auto& factory :_factory) factory->AnimateObjects(fTimeElapsed, pCamera);

	m_pBoss->FindTarget(m_pPlayer);
	m_pBoss->Animate(fTimeElapsed, pCamera);

	//for (int i = 0; i < m_nParticleObjects; i++) {
	//	m_ppParticleObjects[i]->Animate(fTimeElapsed, pCamera);

	//	if (i > 1) continue;
	//	m_ppParticleObjects[i]->SetVector(XMFLOAT3(1.0f,0.0f,0.0f));
	//	m_ppParticleObjects[i]->SetPosition(dynamic_cast<CCannonFactory*>(_factory[1])->GetCannonPosition());
	//}
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->AnimateObjects(fTimeElapsed, pCamera);
	_particles->AnimateObjects(fTimeElapsed, pCamera);

	if (m_pLights) {}
	_ui->AnimateObjects(fTimeElapsed, pCamera);

}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	UpdateShaderVariables(pd3dCommandList);

	if (m_pd3dcbMaterials)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(Signature::Graphics::meterial, d3dcbMaterialsGpuVirtualAddress); //Materials
	}
	if (m_pd3dcbLights)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(Signature::Graphics::light, d3dcbLightsGpuVirtualAddress); //Lights
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvUavDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvUavDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	if (m_pNavMesh) m_pNavMesh->Render(pd3dCommandList, 0);

	for (auto& factory : _factory) factory->Render(pd3dCommandList, pCamera);

	m_pBoss->Render(pd3dCommandList, pCamera);

	_particles->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}

	for (CCollision* col : collisions)
		col->Render(pd3dCommandList, pCamera);

	CheckMonsterFindTarget();
	CheckMonsterCollision();
	CheckPlayerAttack();
	CheckMonsterAttack();

	XMFLOAT3 cannon_pos = dynamic_cast<CCannonFactory*>(_factory[1])->GetCannonPosition();
	if (m_pTerrain->GetHeight(cannon_pos.x,cannon_pos.z) > cannon_pos.y)
		::gnPatricleMode = 0x30;
}

void CScene::UIRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	_ui->Render(pd3dCommandList, pCamera);
}

bool CScene::CheckPlayerByObjectBB(XMFLOAT3 xmf3Shift)
{
	BoundingBox playerBB = m_pPlayer->GetCollManager()->GetBoundingBox();

	for (auto& fac : _factory)
	{
		//if (dynamic_cast<CMonsterFactory*>(fac))continue;
		vector<CGameObject*> objects = fac->GetGameObjects();
		for (int i = 0; i < objects.size(); ++i)
		{
			CCollisionManager* col = objects[i]->GetCollisionManager();
			col->UpdateCollisions();
			BoundingBox BB = col->GetBoundingBox();
			if (CheckAABB(playerBB, BB, xmf3Shift)) return false;
		}
	}

	return true;
}

bool CScene::CheckAABB(BoundingBox A, BoundingBox B, XMFLOAT3 xmf3Shift, bool intersect)
{
	XMFLOAT3 min1 = Vector3::Add(Vector3::Subtract(A.Center, A.Extents),xmf3Shift);
	XMFLOAT3 max1 = Vector3::Add(Vector3::Add(A.Center, A.Extents), xmf3Shift);

	XMFLOAT3 min2 = Vector3::Subtract(B.Center, B.Extents);
	XMFLOAT3 max2 = Vector3::Add(B.Center, B.Extents);

	BoundingBox aabb1;
	BoundingBox::CreateFromPoints(aabb1,XMLoadFloat3(&min1),XMLoadFloat3(&max1));
	BoundingBox aabb2;
	BoundingBox::CreateFromPoints(aabb2, XMLoadFloat3(&min2), XMLoadFloat3(&max2));

	if (intersect)
	{
		if (aabb1.Intersects(aabb2)) return true;
	}
	else 
	{
		if (aabb1.Contains(aabb2)) return true;
	}
	return false;
}

bool CScene::CheckPlayerInScene(XMFLOAT3 xmf3Shift)
{
	BoundingBox playerBB = m_pPlayer->GetCollManager()->GetBoundingBox();
	for (CCollision* col : collisions)
	{
		if (!col->GetDebug()) continue;
		if (CheckAABB(playerBB, col->GetBoundingBox(),xmf3Shift, true)) return true;
	}

	return false;
}

void CScene::CheckInteraction()
{
	BoundingBox playerBB = m_pPlayer->GetCollManager()->GetBoundingBox();
	vector<CGameObject*> cannon = _factory[1]->GetGameObjects();

	for (auto& can : cannon)
	{
		BoundingSphere BS = can->GetCollisionManager()->GetBoundingSphere();
		if (playerBB.Contains(BS)) {
			CGameObject* object = can->FindFrame("Cube_002");
			bool interact = m_pPlayer->SetInteraction(BS.Center, object->m_xmf4x4World);
			if (interact)
				dynamic_cast<CCannonFactory*>(_factory[1])->m_pInteractedCannon = dynamic_cast<CCannonObject*>(can);
			else
				dynamic_cast<CCannonFactory*>(_factory[1])->m_pInteractedCannon = NULL;
			break;
		}
	}
}

void CScene::CheckMonsterFindTarget()
{
	for (auto factory : _factory) {
		CMonsterFactory* pMonsterFactory = dynamic_cast<CMonsterFactory*>(factory);
		if (pMonsterFactory)
		{
			pMonsterFactory->FindTarget(m_pPlayer);
		}
	}
}

void CScene::CheckMonsterCollision()
{
	m_pPlayer->GetCollManager()->UpdateCollisions();
	BoundingSphere playerBS = m_pPlayer->GetCollManager()->GetBoundingSphere();
	vector<CGameObject*> monsters = _factory[2]->GetGameObjects();

	for (auto& monster : monsters)
	{
		monster->GetCollisionManager()->UpdateCollisions();
		BoundingSphere BS = monster->GetCollisionManager()->GetBoundingSphere();
		if (BS.Contains(playerBS)) {
			((CMonsterObject*)monster)->AttackTarget();
			_ui->SetTargetMonster(monster);
		}
	}
}

void CScene::CheckMonsterAttack()
{
	vector<CGameObject*> objects = _factory[2]->GetGameObjects();
	BoundingBox playerBB = m_pPlayer->GetCollManager()->GetBoundingBox();

	for (auto& object : objects)
	{
		bool Enable = (object->m_pSkinnedAnimationController) ? object->m_pSkinnedAnimationController->GetAttackEnable() : false;
		if (!Enable) continue;

		BoundingBox AttackBB = object->GetCollisionManager()->GetBoundingBox(true);
		CCollisionManager* col = object->GetCollisionManager();
		col->UpdateCollisions();

		if (playerBB.Contains(AttackBB))
		{
			m_pPlayer->DecreaseHp(object->GetDamage());
			object->SetAttackEnable(false);
		}
	}
}

void CScene::CheckPlayerAttack()
{
	bool Enable = (m_pPlayer->m_pSkinnedAnimationController) ? m_pPlayer->m_pSkinnedAnimationController->GetAttackEnable() : false;
	if (!Enable) return;

	m_pPlayer->GetCollManager()->UpdateCollisions();
	BoundingBox AttackBB = m_pPlayer->GetCollManager()->GetBoundingBox(true);
	vector<CGameObject*> objects = _factory[2]->GetGameObjects();
	for (auto& object : objects)
	{
		CCollisionManager* col = object->GetCollisionManager();
		col->UpdateCollisions();
		BoundingBox BB = object->GetCollisionManager()->GetBoundingBox();

		if (BB.Contains(AttackBB))
		{
			((CMonsterObject*)object)->DecreaseHp(m_pPlayer->GetDamage());
			_ui->SetTargetMonster(object);
			m_pPlayer->SetAttackEnable(false);
		}
	}
}
