#include "stdafx.h"
#include "ParticleSystem.h"
#include "ParticleShader.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CParticleSystem::CParticleSystem(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, int nParticles, float fEmitTime) : CGameObject(1)
{
	m_nParticles = nParticles;
	InitParticleSystem(xmf3Position, xmf3Range, fEmitTime);
	CreateParticles();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CParticleSystem::~CParticleSystem()
{
}

void CParticleSystem::CreateParticles()
{
	for (int i = 0; i < m_nParticles; ++i)
	{
		XMFLOAT3 xmf3Pos = GetPosition();
		particle_info parts;

		// 일반 파티클
		parts.m_bActive = true;
		parts.m_xmf3Position.x = xmf3Pos.x + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.x;
		parts.m_xmf3Position.z = xmf3Pos.z + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.z;
		parts.m_xmf3Position.y = xmf3Pos.y;// +(((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.y;
		parts.m_fVelocity = m_fParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleVelocityVariation;
		parts.m_xmf3Vectors = XMFLOAT3(0.0f, 1.0f, 0.0f);

		m_pParticles.push_back(parts);
	}
}

void CParticleSystem::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	VS_VB_PARTICLE_INFO* pParticles = new VS_VB_PARTICLE_INFO[m_pParticles.size()];

	for (int i = 0; i < m_nParticles; ++i)
	{
		pParticles[i].m_xmf3Position = m_pParticles[i].m_xmf3Position;
	}

	m_pd3dInstancesBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pParticles,
		sizeof(VS_VB_PARTICLE_INFO) * m_nParticles, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dInstancesBuffer->Map(0, NULL, (void**)&m_pcbMappedParticles);

	m_d3dInstancingBufferView.BufferLocation = m_pd3dInstancesBuffer->GetGPUVirtualAddress();
	m_d3dInstancingBufferView.StrideInBytes = sizeof(VS_VB_PARTICLE_INFO);
	m_d3dInstancingBufferView.SizeInBytes = sizeof(VS_VB_PARTICLE_INFO) * m_nParticles;
}

void CParticleSystem::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nParticles; ++i)
	{
		XMFLOAT3 xmf3Position = m_pParticles[i].m_xmf3Position;
		XMFLOAT2 age(m_pParticles[i].m_fParticleLife, 1000.0f / m_fEmitTime);
		::memcpy(&m_pcbMappedParticles[i].m_xmf3Position, &xmf3Position, sizeof(XMFLOAT3));
		::memcpy(&m_pcbMappedParticles[i].m_xmfAge, &age, sizeof(XMFLOAT2));
	}
}

void CParticleSystem::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, 0);
				m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
			}
			m_pMesh->Render(pd3dCommandList, i);
		}
	}

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_pMesh->GetPositionBufferView(), m_d3dInstancingBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(1, m_nParticles, 0, 0);
}

void CParticleSystem::Animate(float fElapsedTime, CCamera* pCamera)
{
	KillParticles();
	EmitParticles(fElapsedTime);
	UpdateParticles(fElapsedTime);
	sort(m_pParticles.begin(), m_pParticles.end(), compare);
}

void CParticleSystem::KillParticles()
{
	for (int i = 0; i < m_pParticles.size(); ++i)
	{
		if ((m_pParticles[i].m_xmf3Position.y >= m_xmf3MaxRange.y)
			|| (m_pParticles[i].m_xmf3Position.x >= m_xmf3MaxRange.x)
			|| (m_pParticles[i].m_xmf3Position.z >= m_xmf3MaxRange.z))
		{
			m_pParticles[i].m_bActive = false;
			XMFLOAT3 xmf3Pos = GetPosition();

#ifdef EMIT_MODE
			m_pParticles[i].m_xmf3Position = xmf3Pos;
#else
			m_pParticles[i].m_xmf3Position.x = xmf3Pos.x + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.x;
			m_pParticles[i].m_xmf3Position.z = xmf3Pos.z + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.z;
			m_pParticles[i].m_xmf3Position.y = xmf3Pos.y; //+ (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.y;
#endif
			m_pParticles[i].m_fParticleLife = 0.0f;
			m_pParticles[i].m_xmf3Vectors = XMFLOAT3(0.0f, 1.0f, 0.0f);

			++m_nDeadParticles;
		}
	}
}

bool compare(particle_info& p1, particle_info& p2)
{
	return p1.m_xmf3Position.z < p2.m_xmf3Position.z;
}

void CParticleSystem::EmitParticles(float fElapsedTime)
{
	m_fLifeTime += fElapsedTime;
	bool bCheckEmit = false;

	if (m_fLifeTime > (1000.0f / m_fEmitTime))
	{
		m_fLifeTime = 0.0f;
		bCheckEmit = true;
	}

	if ((bCheckEmit) && (m_nDeadParticles))
	{
		--m_nDeadParticles;

		XMFLOAT3 xmf3OnePos = GetPosition();
		XMFLOAT3 xmf3ParPos{
			xmf3OnePos.x + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.x,
			xmf3OnePos.y, //+ (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.y,
			xmf3OnePos.z + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.z };

		for (int i = 0; i < m_pParticles.size(); ++i) {
			if (!m_pParticles[i].m_bActive)
			{
				m_pParticles[i].m_bActive = true;
				m_pParticles[i].m_xmf3Position = xmf3ParPos;
				m_pParticles[i].m_fVelocity = m_fParticleVelocity +
					(((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleVelocityVariation;
				m_pParticles[i].m_fParticleLife = 0.0f;
				m_pParticles[i].m_xmf3Vectors = XMFLOAT3(0.0f, 1.0f, 0.0f);
				break;
			}
		}
	}
}

void CParticleSystem::UpdateParticles(float fElapsedTime)
{
	for (int i = 0; i < m_pParticles.size(); ++i)
	{
#ifdef EMIT_MODE
		if (!m_pParticles[i].m_bActive) continue;
#endif

		m_pParticles[i].m_xmf3Position.y += fElapsedTime * m_pParticles[i].m_fVelocity * m_pParticles[i].m_xmf3Vectors.y;
		m_pParticles[i].m_xmf3Position.x += fElapsedTime * m_pParticles[i].m_fVelocity * m_pParticles[i].m_xmf3Vectors.x;
		m_pParticles[i].m_xmf3Position.z += fElapsedTime * m_pParticles[i].m_fVelocity * m_pParticles[i].m_xmf3Vectors.z;
		m_pParticles[i].m_fParticleLife += fElapsedTime;
	}
}

void CParticleSystem::InitParticleSystem(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, float fEmitTime)
{
	SetPosition(xmf3Position);
	m_xmf3ParticleRange = xmf3Range;
	m_fEmitTime = fEmitTime;
	m_xmf3MaxRange = Vector3::Add(GetPosition(), m_xmf3ParticleRange);
	m_nDeadParticles = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CExplosiveParticle::CExplosiveParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
 XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, int nParticles, float fEmitTime)
	: CParticleSystem()
{
	m_nParticles = nParticles;
	InitParticleSystem(xmf3Position, xmf3Range, fEmitTime);
	CreateParticles();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CExplosiveParticle::~CExplosiveParticle()
{
}

void CExplosiveParticle::CreateParticles()
{
	for (int i = 0; i < m_nParticles; ++i)
	{
		XMFLOAT3 xmf3Pos = GetPosition();
		particle_info parts;

		// 폭죽 파티클
		parts.m_bActive = true;
		parts.m_xmf3Position.x = xmf3Pos.x;
		parts.m_xmf3Position.z = xmf3Pos.z;
		parts.m_xmf3Position.y = xmf3Pos.y;
		XMStoreFloat3(&parts.m_xmf3Vectors, ::RandomUnitVectorOnSphere());

		parts.m_fVelocity = m_fParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleVelocityVariation;
		m_pParticles.push_back(parts);
	}
}

void CExplosiveParticle::KillParticles()
{
	// 삭제와 동시에 모든 파티클이 재생성
	for (int i = 0; i < m_pParticles.size(); ++i)
	{
		if ((m_pParticles[i].m_xmf3Position.y >= m_xmf3MaxRange.y)
			|| (m_pParticles[i].m_xmf3Position.x >= m_xmf3MaxRange.x)
			|| (m_pParticles[i].m_xmf3Position.z >= m_xmf3MaxRange.z)
			|| m_pParticles[i].m_fParticleLife >= m_fMaxLife)
		{
			XMFLOAT3 xmf3Pos = GetPosition();
			m_pParticles[i].m_xmf3Position = xmf3Pos;
			m_pParticles[i].m_fParticleLife = 0.0f;
			XMStoreFloat3(&m_pParticles[i].m_xmf3Vectors, ::RandomUnitVectorOnSphere());
		}
	}
}

void CExplosiveParticle::EmitParticles(float fElapsedTime)
{

}

//////////////////////////////////////////////////////////////////////////////////////
CBreathParticle::CBreathParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, int nParticles, float fEmitTime)
	: CParticleSystem()
{
	m_nParticles = nParticles;
	InitParticleSystem(xmf3Position, xmf3Range, fEmitTime);
	CreateParticles();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_fParticleVelocity = 15.0f;
	m_fParticleVelocityVariation = 100.0f;
	m_fMaxLife = 5.0f;
}

CBreathParticle::~CBreathParticle()
{
}

void CBreathParticle::CreateParticles()
{
	for (int i = 0; i < m_nParticles; ++i)
	{
		particle_info parts;
		
		parts.m_bActive = true;
		parts.m_xmf3Position = GetPosition();
		parts.m_fVelocity = m_fParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_fParticleVelocityVariation;
		parts.m_xmf3Vectors = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pParticles.push_back(parts);
	}
}

void CBreathParticle::EmitParticles(float fElapsedTime)
{
	m_fLifeTime += fElapsedTime;
	bool bCheckEmit = false;

	if (m_fLifeTime > (10.0f / m_fEmitTime))
	{
		m_fLifeTime = 0.0f;
		bCheckEmit = true;
	}

	if ((bCheckEmit) && (m_nDeadParticles))
	{
		--m_nDeadParticles;

		XMFLOAT3 xmf3OnePos = GetPosition();

		for (int i = 0; i < m_pParticles.size(); ++i) {
			if (!m_pParticles[i].m_bActive)
			{
				m_pParticles[i].m_bActive = true;
				m_pParticles[i].m_xmf3Position = xmf3OnePos;
				m_pParticles[i].m_fVelocity = m_fParticleVelocity + RandF(0.5f,1.0f) * m_fParticleVelocityVariation;
				m_pParticles[i].m_fParticleLife = 0.0f;
				m_pParticles[i].m_xmf3Vectors = XMFLOAT3(0.0f, 1.0f, 0.0f);
				break;
			}
		}
	}
}

void CBreathParticle::UpdateParticles(float fElapsedTime)
{
	for (int i = 0; i < m_pParticles.size(); ++i)
	{
#ifdef EMIT_MODE
		if (!m_pParticles[i].m_bActive) continue;
#endif
		_sinValue += fElapsedTime * 100.0f;
		m_pParticles[i].m_xmf3Vectors.x = sin(3.141592 * 2 * _sinValue);
		m_pParticles[i].m_xmf3Vectors.z = cos(3.141592 * 2 * _sinValue);

	}

	CParticleSystem::UpdateParticles(fElapsedTime);
}

void CBreathParticle::KillParticles()
{
	for (int i = 0; i < m_pParticles.size(); ++i)
	{
		if (m_pParticles[i].m_fParticleLife < m_fMaxLife) continue;

		m_pParticles[i].m_bActive = false;
		XMFLOAT3 xmf3Pos = GetPosition();

#ifdef EMIT_MODE
		m_pParticles[i].m_xmf3Position = xmf3Pos;
#else
		m_pParticles[i].m_xmf3Position.x = xmf3Pos.x + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.x;
		m_pParticles[i].m_xmf3Position.z = xmf3Pos.z + (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.z;
		m_pParticles[i].m_xmf3Position.y = xmf3Pos.y; //+ (((float)rand() - (float)rand()) / float(RAND_MAX)) * m_xmf3ParticleRange.y;
#endif
		m_pParticles[i].m_fParticleLife = 0.0f;
		m_pParticles[i].m_xmf3Vectors = XMFLOAT3(0.0f, 1.0f, 0.0f);
		_sinValue = 0.0f;

		++m_nDeadParticles;
	}
}
