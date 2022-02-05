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

struct CB_TOOBJECTSPACE
{
	matrix		mtxToTexture;
	float4		f4Position;
};

cbuffer cbToLightSpace : register(b5)
{
	CB_TOOBJECTSPACE gcbToLightSpaces[MAX_LIGHTS];
};

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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtSkyBox : register(t7);

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);


	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

//--------------------------------------------------------------------------------------
Texture2D<float4> gtxtTerrainBaseTexture : register(t1);
Texture2D<float4> gtxtTerrainDetailTextures[3] : register(t2); //t2, t3, t4
Texture2D<float4> gtxtTerrainAlphaTexture[2] : register(t5); // t5, t6

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float3 positionW : POSITION1;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 tessellation : TEXCOORD2;
};

[domain("quad")]
//[partitioning("fractional_even")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("HSTerrainTessellationConstant")]
[maxtessfactor(64.0f)]
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;

	return(output);
}

float CalculateTessFactor(float3 f3Position)
{
	float fDistToCamera = distance(f3Position, gvCameraPosition);
	float s = saturate((fDistToCamera - 10.0f) / (500.0f - 10.0f));
	return(lerp(64.0f, 1.0f, s));
	//	return(pow(2, lerp(20.0f, 4.0f, s)));
}

HS_TERRAIN_TESSELLATION_CONSTANT HSTerrainTessellationConstant(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input)
{
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	{
		float3 e0 = 0.5f * (input[0].positionW + input[4].positionW);
		float3 e1 = 0.5f * (input[0].positionW + input[20].positionW);
		float3 e2 = 0.5f * (input[4].positionW + input[24].positionW);
		float3 e3 = 0.5f * (input[20].positionW + input[24].positionW);

		output.fTessEdges[0] = CalculateTessFactor(e0);
		output.fTessEdges[1] = CalculateTessFactor(e1);
		output.fTessEdges[2] = CalculateTessFactor(e2);
		output.fTessEdges[3] = CalculateTessFactor(e3);

		float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 25; i++) f3Sum += input[i].positionW;
		float3 f3Center = f3Sum / 25.0f;
		output.fTessInsides[0] = output.fTessInsides[1] = CalculateTessFactor(f3Center);
	}

	return(output);
}

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);

	return(f3Sum);
}

[domain("quad")]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output = (DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);

	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxGameObject, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	output.tessellation = float4(patchConstant.fTessEdges[0], patchConstant.fTessEdges[1], patchConstant.fTessEdges[2], patchConstant.fTessEdges[3]);

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrainTessellation(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	float4 cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);

	if (gnRenderMode & (DEBUG_TESSELLATION))
	{
		if (input.tessellation.w <= 5.0f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 10.0f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 20.0f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 30.0f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 40.0f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 50.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 55.0f) cColor = float4(0.2f, 0.2f, 0.72f, 1.0f);
		else if (input.tessellation.w <= 60.0f) cColor = float4(0.5f, 0.75f, 0.75f, 1.0f);
		else cColor = float4(0.87f, 0.17f, 1.0f, 1.0f);
	}
	else
	{
		float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
		float fAlphas[2];
		fAlphas[0] = gtxtTerrainAlphaTexture[0].Sample(gSamplerState, input.uv0).w;
		fAlphas[1] = gtxtTerrainAlphaTexture[1].Sample(gSamplerState, input.uv0).w;

		float4 cDetailTexColors[3];
		cDetailTexColors[0] = gtxtTerrainDetailTextures[0].Sample(gSamplerState, input.uv1 * 2.0f);
		cDetailTexColors[1] = gtxtTerrainDetailTextures[1].Sample(gSamplerState, input.uv1 * 2.0f);
		cDetailTexColors[2] = gtxtTerrainDetailTextures[2].Sample(gSamplerState, input.uv1 * 2.0f);

		cColor = input.color * saturate((cBaseTexColor * 0.6f) + (cDetailTexColors[1] * 0.4f));
		cColor += lerp(cBaseTexColor * 0.25f, cDetailTexColors[2], 1.0f - fAlphas[0]);

		if (fAlphas[1] < 0.35f) cColor = saturate((cBaseTexColor * 0.3f) + (cDetailTexColors[0] * 0.7f));
	}

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	//float grayscale = (cColor.r + cColor.g + cColor.b) / 3.0f;
	//output.f4Scene = output.f4Color = float4(grayscale,grayscale,grayscale,1.0f);

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
RWTexture2D<float4> gtxtRWOutput : register(u0);
Texture2D<float4> gtxtInputTextures[3] : register(t8);
Texture2D gtxtOutput : register(t11);

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct PS_DEPTH_OUTPUT
{
	float fzPosition : SV_Target;
	float fDepth : SV_Depth;
};

PS_DEPTH_OUTPUT PSDepthWriteShader(VS_LIGHTING_OUTPUT input)
{
	PS_DEPTH_OUTPUT output;

	output.fzPosition = input.position.z;
	output.fDepth = input.position.z;

	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SHADOW_MAP_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;

	float4 uvs[MAX_LIGHTS] : TEXCOORD0;
};

VS_SHADOW_MAP_OUTPUT VSShadowMapShadow(VS_LIGHTING_INPUT input)
{
	VS_SHADOW_MAP_OUTPUT output = (VS_SHADOW_MAP_OUTPUT)0;

	float4 positionW = mul(float4(input.position, 1.0f), gmtxWorld);
	output.positionW = positionW.xyz;
	output.position = mul(mul(positionW, gmtxView), gmtxProjection);
	output.normalW = mul(float4(input.normal, 0.0f), gmtxWorld).xyz;

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gcbToLightSpaces[i].f4Position.w != 0.0f) output.uvs[i] = mul(positionW, gcbToLightSpaces[i].mtxToTexture);
	}

	return(output);
}

float4 PSShadowMapShadow(VS_SHADOW_MAP_OUTPUT input) : SV_TARGET
{
	float4 cIllumination = Lighting(input.positionW, normalize(input.normalW), true, input.uvs);

	return(cIllumination);
}