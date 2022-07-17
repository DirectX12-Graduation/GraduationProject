cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	matrix		gmtxViewProjection : packoffset(c8);
	matrix		gmtxOrthoProjection : packoffset(c12);
	float3		gvCameraPosition : packoffset(c16);
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
	uint		gnParticleMode : packoffset(c0.z);
	uint		gnBlurMode : packoffset(c0.w);
	float4		gcFogColor : packoffset(c1);
	float4		gvFogParameter : packoffset(c2);
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

// Get FogFactor by Mode
float GetFogFactor(float4 cameraPosition)
{
	float fogFactor = 0.0f;
	if (gvFogParameter.x == 1.0f) {
		fogFactor = saturate((gvFogParameter.z - cameraPosition.z) / (gvFogParameter.z - gvFogParameter.y));
	}
	else if (gvFogParameter.x == 2.0f) {
		fogFactor = 1 / exp(cameraPosition.z * gvFogParameter.w);
	}
	else if (gvFogParameter.x == 3.0f) {
		fogFactor = 1 / exp(pow(cameraPosition.z * gvFogParameter.w, 2));
	}

	return fogFactor;
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

TextureCube gtxtSkyCubeTexture : register(t6);

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
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTextures[3] : register(t2);
Texture2D gtxtTerrainAlphaTexture : register(t5);

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
	float fogFactor : FOG;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	// fogFactor
	float4 cameraPosition = mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView);
	output.fogFactor = GetFogFactor(cameraPosition);

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);

	float4 cDetailTexColors[3];
	cDetailTexColors[0] = gtxtTerrainDetailTextures[0].Sample(gSamplerState, input.uv1);
	cDetailTexColors[1] = gtxtTerrainDetailTextures[1].Sample(gSamplerState, input.uv1);
	cDetailTexColors[2] = gtxtTerrainDetailTextures[2].Sample(gSamplerState, input.uv1);

	float fAlphaR = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0).r;
	float fAlphaG = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0).g;
	float fAlphaB = gtxtTerrainAlphaTexture.Sample(gSamplerState, input.uv0).b;

	float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColors[0] * 0.5f));

	cColor += cDetailTexColors[1] * fAlphaG;
	cColor += cDetailTexColors[2] * fAlphaB;

	// fogColor
	cColor = input.fogFactor * cColor + (1.0f - input.fogFactor) * gcFogColor;

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
RWTexture2D<float4> gtxtRWOutput : register(u0);
Texture2D<float4> gtxtInputTextures[3] : register(t7);
Texture2D gtxtOutput : register(t10);

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

Texture2D<float4> gtxtModelDiffuseTexture : register(t12);
struct VS_WIREFRAME_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_WIREFRAME_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float fogFactor : FOG;
};

VS_WIREFRAME_OUTPUT VSModelTextured(VS_WIREFRAME_INPUT input)
{
	VS_WIREFRAME_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	// fogFactor
	float4 cameraPosition = mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView);
	output.fogFactor = GetFogFactor(cameraPosition);

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSModelTextured(VS_WIREFRAME_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtModelDiffuseTexture.Sample(gSamplerState, input.uv);

	// fogColor
	cColor = input.fogFactor * cColor + (1.0f - input.fogFactor) * gcFogColor;

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;
	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128
Texture2D<float4> gtxtAnimationDiffuseTexture : register(t11);

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
	float fogFactor : FOG;
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

	// fogFactor
	float4 cameraPosition = mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView);
	output.fogFactor = GetFogFactor(cameraPosition);

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSSkinnedAnimation(VS_SKINNED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtAnimationDiffuseTexture.Sample(gSamplerState, input.uv);

	// fogColor
	cColor = input.fogFactor * cColor + (1.0f - input.fogFactor) * gcFogColor;

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}
/////////////////////////

cbuffer cbFrameworkInfo : register(b9)
{
	float ratioHp : packoffset(c0.x);
};

VS_TEXTURED_OUTPUT VSHp(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxOrthoProjection);

	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSHp(VS_TEXTURED_OUTPUT input) :SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	
	if (input.uv.x > ratioHp)
	{
		cColor = float4(0.6f, 0.6f, 0.6f, 1);
	}

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PARTICLE_TYPE_EMITTER	0
#define PARTICLE_TYPE_FLARE		0x0ff

struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
	float3 velocity : VELOCITY;
	float3 acceleration : ACCELERATION;
	float2 size : SIZE;
	float2 age : AGELIFETIME; //(Age, Lifetime)
	uint type : PARTICLETYPE;
};


cbuffer cbParticleInfo : register(b10)
{
	float size_x : packoffset(c0.x);
	float size_y : packoffset(c0.y);
	float3 vec3 : packoffset(c1);
};

VS_PARTICLE_INPUT VSParticleStreamOutput(VS_PARTICLE_INPUT input)
{
	return(input);
}

Buffer<float4> gRandomBuffer : register(t14);

float3 GetParticleColor(float fAge, float fLifetime)
{
	float3 cColor = float3(1.0f, 1.0f, 1.0f);

	if (fLifetime == 0.0f)
		cColor = float3(0.0f, 0.0f, 0.0f);
	else
	{
		float t = fAge / fLifetime;
		cColor = lerp(float3(1.0f, 0.0f, 0.0f), float3(1.0f, 0.9f, 0.8f), t * 1.0f);
	}

	return(cColor);
}

void GetBillboardCorners(float3 position, float2 size, out float4 pf4Positions[4])
{
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Look = normalize(gvCameraPosition - position);
	float3 f3Right = normalize(cross(f3Up, f3Look));

	pf4Positions[0] = float4(position + size.x * f3Right - size.y * f3Up, 1.0f);
	pf4Positions[1] = float4(position + size.x * f3Right + size.y * f3Up, 1.0f);
	pf4Positions[2] = float4(position - size.x * f3Right - size.y * f3Up, 1.0f);
	pf4Positions[3] = float4(position - size.x * f3Right + size.y * f3Up, 1.0f);
}

void GetPositions(float3 position, float2 f2Size, out float3 pf3Positions[8])
{
	float3 f3Right = float3(1.0f, 0.0f, 0.0f);
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Look = float3(0.0f, 0.0f, 1.0f);

	float3 f3Extent = normalize(float3(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < 8; ++i)
	{
		pf3Positions[i] = float3(gmtxGameObject._41, gmtxGameObject._42, gmtxGameObject._43);
	}
}


#define PARTICLE_EXPLOSION			0x30
#define PARTICLE_LINE				0x31

[maxvertexcount(9)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	static int index = 0;
	static float max_elpased = 0.0f;
	VS_PARTICLE_INPUT particle = input[0];

	if (particle.age.x <= particle.age.y)
	{
		if (particle.type == PARTICLE_TYPE_EMITTER)
		{
			particle.color = float3(1.0f, 1.0f, 0.0f);
			//particle.age.y = 0.25f;
			particle.position = float3(gmtxGameObject._41, gmtxGameObject._42, gmtxGameObject._43);
			output.Append(particle);

			particle.age.x = 0.0f;

			for (int j = 0; j < 8; j++)
			{
				if (gnParticleMode & PARTICLE_EXPLOSION)
				{
					output.Append(input[0]);
					continue;
				}

				particle.color = float3(1.0f, 0.0f, 0.0f);

				//float4 f4Random = gRandomBuffer.Load(uint(index * gfElapsedTime * 10000.0f) % 1000);
				float4 f4Random = gRandomBuffer.Load(uint(index * gfCurrentTime * 100.0f) % 1000);
				particle.type = PARTICLE_TYPE_FLARE;
				particle.position = float3(gmtxGameObject._41, gmtxGameObject._42, gmtxGameObject._43);
				particle.velocity = float3(10.0f, 0.0f, 0.0f) * 10000.0f;
				particle.acceleration = float3(vec3.x, vec3.y, vec3.z);
				particle.size = float2(size_x, size_y);
				particle.age.y = 1.8f;

				//particle.age.y = (particle.type == PARTICLE_TYPE_EMITTER) ? 0.25f : 1.5f + 0.75f * abs(j-4);
				//particle.age.y = (particle.type == PARTICLE_TYPE_EMITTER) ? 0.25f : 1.5f + (abs(f4Random.w) * 0.75f * abs(j - 1));
				//particle.age.y = 2.5f;

				++index;

				output.Append(particle);
			}

		}
		else
		{
			particle.age.x += gfElapsedTime;
			if (max_elpased <= gfElapsedTime) max_elpased = gfElapsedTime;

			if (particle.size.x <= size_x * 3.0f)
			{
				float2 size = float2(size_x * 3.5f, size_y * 3.5f) * 1.5f;
				particle.size += size * gfElapsedTime;
			}
			particle.color = GetParticleColor(particle.age.x, particle.age.y);
			particle.position += (0.5f * particle.acceleration * particle.velocity * gfElapsedTime * gfElapsedTime);
			output.Append(particle);
		}

	}
}

VS_PARTICLE_INPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
	return(input);
}

struct GS_PARTICLE_OUTPUT
{
	float4 position : SV_Position;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
	float2 age : AGELIFETIME; //(Age, Lifetime)
	uint type : PARTICLETYPE;
	float fogFactor : FOG;
};

static float2 gf2QuadUVs[4] = { float2(0.0f,1.0f), float2(0.0f,0.0f), float2(1.0f,1.0f), float2(1.0f,0.0f) };

[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_INPUT input[1], inout TriangleStream<GS_PARTICLE_OUTPUT> outputStream)
{
	float4 pVertices[4];
	GetBillboardCorners(input[0].position, input[0].size * 0.5f, pVertices);
	//GetBillboardCorners(mul(float4(input[0].position, 1.0f), gmtxGameObject).xyz, input[0].size * 0.5f, pVertices);

	GS_PARTICLE_OUTPUT output = (GS_PARTICLE_OUTPUT)0;
	output.color = input[0].color;
	output.age = input[0].age;
	output.type = input[0].type;
	for (int i = 0; i < 4; i++)
	{
		output.position = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.uv = gf2QuadUVs[i];

		outputStream.Append(output);
	}
}

Texture2D<float4> gtxtParticleTexture : register(t13);

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSParticleDraw(GS_PARTICLE_OUTPUT input) : SV_TARGET
{

	float4 cColor = gtxtParticleTexture.Sample(gSamplerState, input.uv);
	cColor.rgb *= GetParticleColor(input.age.x, input.age.y);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

///** explosion particle ** //

[maxvertexcount(9)]
void GSParticleExStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	static int index = 0;
	VS_PARTICLE_INPUT particle = input[0];

	if (gnParticleMode & PARTICLE_EXPLOSION) particle.age.x += gfElapsedTime;
	if (particle.age.x <= particle.age.y)
	{
		//if (particle.type == PARTICLE_TYPE_EMITTER)
		if (gnParticleMode & PARTICLE_EXPLOSION)
		{
			particle.color = GetParticleColor(particle.age.x, particle.age.y);

			if (particle.type == PARTICLE_TYPE_EMITTER)
			{
				if (particle.size.x <= size_x * 50.0f)
				{
					float2 size = float2(size_x * 3.5f, size_y * 3.5f) * 8.5f;
					particle.size += size * gfElapsedTime;
				}
			}
			else particle.position += (0.5f * particle.acceleration * particle.velocity * gfElapsedTime);

			output.Append(particle);
		}
		else
		{
			particle.color = float3(1.0f, 0.0f, 0.0f);
			particle.size = float2(size_x, size_y);
			//particle.type = PARTICLE_TYPE_EMITTER;
			//			particle.age.x = 0.0f;
			output.Append(particle);


			float3 pf3Positions[8];
			GetPositions(particle.position, float2(particle.size.x * 1.25f, particle.size.x * 1.25f), pf3Positions);

			particle.color = float3(0.0f, 0.0f, 1.0f);
			particle.age.x = 0.0f;

			for (int j = 0; j < 8; j++)
			{
				//float4 f4Random = gRandomBuffer.Load(uint(index * gfElapsedTime * 10000.0f) % 1000);
				float4 f4Random = gRandomBuffer.Load(uint(index * gfCurrentTime * 100.0f) % 1000);
				particle.type = (j == 0) ? PARTICLE_TYPE_EMITTER : PARTICLE_TYPE_FLARE;
				particle.position = pf3Positions[j].xyz;
				particle.velocity = float3(
					particle.size.x * particle.age.y * 4.0f
					, particle.size.x * particle.age.y * 4.0f
					, particle.size.x * particle.age.y * 4.0f) * 3.0f;
				particle.acceleration = float3(f4Random.x, f4Random.y, f4Random.z);
				particle.size = float2(size_x*3.5f, size_y*3.5f);

				particle.age.y = 3.5f;

				++index;

				output.Append(particle);
			}
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_BILLBOARD_OUT {
	float3 centerW:POSITION;
	float2 sizeW:SIZE;
	float2 age : AGE;
};

struct VS_BILLBOARD_IN {
	float3 posW:POSITION;
	float2 sizeW:SIZE;
	float3 instancePosition : INSTANCEPOSITION;
	float2 instanceAge : INSTANCEAGE;
};

struct GS_BILLBOARD_OUT {
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normalW : NORMAL;
	float2 uv: TEXCOORD;
	float2 age : AGE;
	uint primID:SV_PrimitiveID;
};

VS_BILLBOARD_OUT VSBillboard(VS_BILLBOARD_IN input)
{
	VS_BILLBOARD_OUT output;
	output.centerW = input.instancePosition;
	output.sizeW = input.sizeW;
	output.age = input.instanceAge;
	return(output);
}

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_OUT input[1], uint primID:SV_PrimitiveID, inout TriangleStream<GS_BILLBOARD_OUT> outStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].centerW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fHalfW = input[0].sizeW.x * 0.5f;
	float fHalfH = input[0].sizeW.y * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].centerW + fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[1] = float4(input[0].centerW + fHalfW * vRight + fHalfH * vUp, 1.0f);
	pVertices[2] = float4(input[0].centerW - fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[3] = float4(input[0].centerW - fHalfW * vRight + fHalfH * vUp, 1.0f);

	float2 pUVs[4] = { float2(0.0f,1.0f),float2(0.0f,0.0f),float2(1.0f,1.0f),float2(1.0f,0.0f) };
	GS_BILLBOARD_OUT output;
	for (int i = 0; i < 4; ++i) {
		output.posW = pVertices[i].xyz;
		output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.uv = pUVs[i];
		output.primID = primID;
		output.age = input[0].age;
		outStream.Append(output);
	}
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSBillboard(GS_BILLBOARD_OUT input) : SV_TARGET
{
	float4 cColor = gtxtParticleTexture.Sample(gSamplerState, input.uv);
	cColor.rgb *= GetParticleColor(input.age.x, input.age.y);


	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.posH.z;

	return(output);
}

