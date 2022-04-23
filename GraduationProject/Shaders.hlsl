cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	matrix		gmtxViewProjection : packoffset(c8);
	float3		gvCameraPosition : packoffset(c12);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

#include "Light.hlsl"

cbuffer cbFrameworkInfo : register(b6)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	uint		gnRenderMode : packoffset(c0.z);
	uint		gnBlurMode : packoffset(c0.w);
};

#define DEBUG_TESSELLATION			0x20
#define DEBUG_BLURRING				0x21

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 f4Scene : SV_TARGET0; //Swap Chain Back Buffer
	float4 f4Color : SV_TARGET1;
	float4 fDepth : SV_TARGET2;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = input.color;
	output.fDepth = 1.0f - input.position.z;
	return(output);
}

VS_DIFFUSED_OUTPUT VSCollider(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSCollider(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = input.color;
	output.fDepth = 1.0f - input.position.z;
	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_NORMAL_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_NORMAL_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normalW : NORMAL;
};

VS_NORMAL_OUTPUT VSNormalCollider(VS_NORMAL_INPUT input)
{
	VS_NORMAL_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.normalW = input.normal;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSNormalCollider(VS_NORMAL_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = float4(1.0f, 0.0f, 0.0f,1.0f);
	output.fDepth = 1.0f - input.position.z;
	return(output);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	input.normalW = normalize(input.normalW);
	float4 uvs[MAX_LIGHTS];
	float4 cIllumination = Lighting(input.positionW, input.normalW, false, uvs);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor*cIllumination;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}
PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSLighting_Transparent(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	cColor.a = 0.5f;
	input.normalW = normalize(input.normalW);
	float4 uvs[MAX_LIGHTS];
	float4 cIllumination = Lighting(input.positionW, input.normalW, false, uvs);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor*cIllumination;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

TextureCube gtxtSkyCubeTexture : register(t3);

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gClampSamplerState, input.positionL);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

//--------------------------------------------------------------------------------------
Texture2D<float4> gtxtTerrainBaseTexture : register(t1);
Texture2D<float4> gtxtTerrainDetailTexture : register(t2);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gSamplerState, input.uv1*2.0f);
	//float4 cColor = saturate((cBaseTexColor * 0.6f) + (cDetailTexColor * 0.4f));
	float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
RWTexture2D<float4> gtxtRWOutput : register(u0);
Texture2D<float4> gtxtInputTextures[3] : register(t4);
Texture2D gtxtOutput : register(t7);

groupshared float4 gTextureCache[(480 + 2 * 5)];
groupshared float4 gHorzCache[(640 + 2 * 5)];
static float gfWeights[11] = { 0.05f,0.05f,0.1f,0.1f,0.1f,0.2f,0.1f,0.1f,0.1f,0.05f,0.05f };

[numthreads(1, 480, 1)]
void VertBlurCS(int3 vGroupThreadID: SV_GroupThreadID, int3 vDispatchThreadID : SV_DispatchThreadID)
{
	if (vGroupThreadID.y < 5)
	{
		int y = max(vDispatchThreadID.y - 5, 0);
		gTextureCache[vGroupThreadID.y] = gtxtInputTextures[0][int2(vDispatchThreadID.x, y)];
	}
	else if (vGroupThreadID.y >= 480 - 5)
	{
		int y = min(vDispatchThreadID.y + 5, gtxtInputTextures[0].Length.y - 1);
		gTextureCache[vGroupThreadID.y + 2 * 5] = gtxtInputTextures[0][int2(vDispatchThreadID.x, y)];
	}
	gTextureCache[vGroupThreadID.y + 5] = gtxtInputTextures[0][min(vDispatchThreadID.xy, gtxtInputTextures[0].Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 cBlurredColor = float4(0, 0, 0, 0);
	for (int i = -5; i <= 5; ++i)
	{
		int k = vGroupThreadID.y + 5 + i;
		cBlurredColor += gfWeights[i + 5] * gTextureCache[k];
	}
	gtxtRWOutput[vDispatchThreadID.xy] = cBlurredColor;
}

[numthreads(640, 1, 1)]
void HorzBlurCS(int3 vGroupThreadID: SV_GroupThreadID, int3 vDispatchThreadID : SV_DispatchThreadID)
{
	if (vGroupThreadID.x < 5)
	{
		int x = max(vDispatchThreadID.x - 5, 0);
		gHorzCache[vGroupThreadID.x] = gtxtInputTextures[0][int2(x, vDispatchThreadID.y)];
	}
	else if (vGroupThreadID.x >= 640 - 5)
	{
		int x = min(vDispatchThreadID.x + 5, gtxtInputTextures[0].Length.x - 1);
		gHorzCache[vGroupThreadID.x + 2 * 5] = gtxtInputTextures[0][int2(x, vDispatchThreadID.y)];
	}
	gHorzCache[vGroupThreadID.x + 5] = gtxtInputTextures[0][min(vDispatchThreadID.xy, gtxtInputTextures[0].Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 cBlurredColor = float4(0, 0, 0, 0);
	for (int i = -5; i <= 5; ++i)
	{
		int k = vGroupThreadID.x + 5 + i;
		cBlurredColor += gfWeights[i + 5] * gHorzCache[k];
	}
	gtxtRWOutput[vDispatchThreadID.xy] = cBlurredColor;
}

float4 GetColorFromDepth(float fDepth)
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (fDepth > 1.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.00625f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (fDepth < 0.0125f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (fDepth < 0.025f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (fDepth < 0.05f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (fDepth < 0.075f) cColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.1f) cColor = float4(1.0f, 0.5f, 0.5f, 1.0f);
	else if (fDepth < 0.4f) cColor = float4(0.5f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.6f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
	else if (fDepth < 0.8f) cColor = float4(0.5f, 0.5f, 1.0f, 1.0f);
	else if (fDepth < 0.9f) cColor = float4(0.5f, 1.0f, 0.5f, 1.0f);
	else cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return(cColor);
}

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_SCREEN_RECT_TEXTURED_OUTPUT VSPostProcessing(uint nVertexID : SV_VertexID)
{
	VS_SCREEN_RECT_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT)0;

	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }

	else if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

float4 PSPostProcessing(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor;
	if (gnBlurMode & (DEBUG_BLURRING)) cColor = gtxtOutput.Sample(gSamplerState, input.uv);
	else cColor = gtxtInputTextures[0].Sample(gSamplerState, input.uv);

	return(cColor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Animation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Texture2D<float4> gtxtModelDiffuseTexture : register(t9);
struct VS_WIREFRAME_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_WIREFRAME_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_WIREFRAME_OUTPUT VSModelTextured(VS_WIREFRAME_INPUT input)
{
	VS_WIREFRAME_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSModelTextured(VS_WIREFRAME_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtModelDiffuseTexture.Sample(gSamplerState, input.uv);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;
	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128
Texture2D<float4> gtxtAnimationDiffuseTexture : register(t8);

cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_INPUT
{
	float3 position : POSITION;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
	float2 uv :TEXCOORD;
};

struct VS_SKINNED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_SKINNED_OUTPUT VSSkinnedAnimation(VS_SKINNED_INPUT input)
{
	VS_SKINNED_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	}
	output.uv = input.uv;
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	//	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSSkinnedAnimation(VS_SKINNED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtAnimationDiffuseTexture.Sample(gSamplerState, input.uv);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}