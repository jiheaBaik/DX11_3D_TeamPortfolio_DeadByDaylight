#include "Audio.h"
#include "Sound.h"
#include "Transform.h"

CAudio* const CAudio::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CAudio* pInstance = new CAudio(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CAudio::Clone(void* const& pArg) {
	CAudio* pInstance = new CAudio(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CAudio::CAudio(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
	m_pSound = CSound::Get_Instance();
	Safe_AddRef(m_pSound);
}

CAudio::CAudio(const CAudio& rhs)
	: CComponent(rhs),
	m_pSound(rhs.m_pSound),
	m_pSystem(rhs.m_pSystem) {
	Safe_AddRef(m_pSound);
}

void CAudio::Delete(void) {
	__super::Delete();
	Stop_Sound_All();
	m_pSystem = nullptr;
	Safe_Release(m_pSound);
}

const HRESULT CAudio::Init_Create(void) {
	m_pSystem = m_pSound->Get_System();
	return S_OK;
}

const HRESULT CAudio::Init_Clone(void* const& pArg) {
	return S_OK;
}

void CAudio::Set_Listener(CTransform* const pTransform, const _float3& vVelocity) {
	_matrix WorldMatrix = pTransform->Get_Matrix();
	FMOD_3D_ATTRIBUTES listener;
	listener.position = *(FMOD_VECTOR*)&WorldMatrix.r[3];
	listener.forward = *(FMOD_VECTOR*)&WorldMatrix.r[2];
	listener.up = *(FMOD_VECTOR*)&WorldMatrix.r[1];
	listener.velocity = *(FMOD_VECTOR*)&vVelocity;
	FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &listener.position, &listener.velocity, &listener.forward, &listener.up);
}

void CAudio::Set_Listener(CTransform* const pCameraTransform, CTransform* const pPlayerTransform , const _float3& vVelocity) {
	_matrix CameraWorldMatrix = pCameraTransform->Get_Matrix();
	_matrix PlayerWorldMatrix = pPlayerTransform->Get_Matrix();
	FMOD_3D_ATTRIBUTES listener;
	listener.position = *(FMOD_VECTOR*)&PlayerWorldMatrix.r[3];
	listener.forward = *(FMOD_VECTOR*)&CameraWorldMatrix.r[2];
	listener.up = *(FMOD_VECTOR*)&CameraWorldMatrix.r[1];
	listener.velocity = *(FMOD_VECTOR*)&vVelocity;
	FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &listener.position, &listener.velocity, &listener.forward, &listener.up);
}

void CAudio::Set_3DAttribute(const _ubyte& byChannel, CTransform* const pTransform, const _float3& vVelocity) {
	_matrix WorldMatrix = pTransform->Get_Matrix();

	FMOD_3D_ATTRIBUTES attr;
	attr.position = *(FMOD_VECTOR*)&WorldMatrix.r[3];
	attr.forward = *(FMOD_VECTOR*)&WorldMatrix.r[2];
	attr.up = *(FMOD_VECTOR*)&WorldMatrix.r[1];
	attr.velocity = *(FMOD_VECTOR*)&vVelocity;
	FMOD_RESULT eRes = FMOD_Channel_Set3DAttributes(m_pChannel[byChannel], &attr.position, &attr.velocity);
	if (FMOD_OK != eRes)
		int i = 0;
}

void CAudio::Set_3DAttribute(const _ubyte& byChannel, _fmatrix Matrix, const _float3& vVelocity) {
	FMOD_3D_ATTRIBUTES attr;
	attr.position = *(FMOD_VECTOR*)&Matrix.r[3];
	attr.forward = *(FMOD_VECTOR*)&Matrix.r[2];
	attr.up = *(FMOD_VECTOR*)&Matrix.r[1];
	attr.velocity = *(FMOD_VECTOR*)&vVelocity;
	FMOD_RESULT eRes = FMOD_Channel_Set3DAttributes(m_pChannel[byChannel], &attr.position, &attr.velocity);
}

void CAudio::Set_Volume(const _ubyte& byChannel, const _float& fVolume) {
	FMOD_Channel_SetVolume(m_pChannel[byChannel], fVolume);
}

void CAudio::Set_Mode(const _ubyte& byChannel, const FMOD_MODE& iMode) {
	FMOD_Channel_SetMode(m_pChannel[byChannel], iMode);
}


void CAudio::Set_MaxChannel(const _uint iMaxChannel) {
	m_iChannel = iMaxChannel;
	m_iMaxChannel = iMaxChannel;
}

void CAudio::Set_Distance(const _ubyte& byChannel, const _float& fMin, const _float& fMax) {
	FMOD_Channel_Set3DMinMaxDistance(m_pChannel[byChannel], fMin, fMax);
}

void CAudio::Add_Sound(const _ubyte& byScene, const _tchar* const pSoundKey) {
	FMOD_SOUND* pSound = m_pSound->Get_Sound(byScene, pSoundKey);
	if (nullptr == pSound)
		return;
	m_mapSound.emplace(pSoundKey, pSound);

}

void CAudio::Play_Sound(const _tchar* const pSoundKey, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, CTransform* const pTransform, const _float3& vVelocity, const _float2& fDist) {
	if ((iMode & FMOD_3D) && !(iMode & FMOD_LOOP_NORMAL)) {
		FMOD_VECTOR fvPos;
		FMOD_System_Get3DListenerAttributes(m_pSystem, 0, &fvPos, nullptr, nullptr, nullptr);
		_vector vPos = XMLoadFloat3((_float3*)&fvPos);
		_float vDist = XMVectorGetX(XMVector3Length(vPos - pTransform->Get_Row(CTransform::ERow::POS)));
		if (vDist > fDist.y)
			return;
		else
			int i = 0;
	}
	FMOD_SOUND* pSound = Find_Sound(pSoundKey);
	if (nullptr == pSound) {
		for (_ubyte i = 0; i < m_pSound->Get_Scene(); ++i) {
			pSound = m_pSound->Get_Sound(i, pSoundKey);
			if (nullptr != pSound) {
				m_mapSound.emplace(pSoundKey, pSound);
				break;
			}
		}
		if (nullptr == pSound)
			return;
	}

	if (-1 == iChannel) {
		m_iChannel = m_iChannel < FMOD_MAX_CHANNEL_WIDTH - 1 ? m_iChannel + 1 : m_iMaxChannel;
		FMOD_Channel_Stop(m_pChannel[m_iChannel]);
		FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &m_pChannel[m_iChannel]);
		FMOD_Channel_SetVolume(m_pChannel[m_iChannel], fVolume);
		if (iMode & FMOD_2D) {
			FMOD_Channel_SetMode(m_pChannel[m_iChannel], FMOD_2D);
		}
		if (iMode & FMOD_LOOP_NORMAL) {
			FMOD_Channel_SetMode(m_pChannel[m_iChannel], FMOD_LOOP_NORMAL);
		}
		if (iMode & FMOD_3D) {
			FMOD_Channel_SetMode(m_pChannel[m_iChannel], FMOD_3D_LINEARROLLOFF);
			Set_3DAttribute(m_iChannel, pTransform, vVelocity);
			Set_Distance(m_iChannel, fDist.x, fDist.y);
		}
		FMOD_Channel_SetPaused(m_pChannel[m_iChannel], FALSE);
	}
	else {
		FMOD_Channel_Stop(m_pChannel[iChannel]);
		FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &m_pChannel[iChannel]);
		FMOD_Channel_SetVolume(m_pChannel[iChannel], fVolume);
		if (iMode & FMOD_2D) {
			FMOD_Channel_SetMode(m_pChannel[iChannel], FMOD_2D);
		}
		if (iMode & FMOD_LOOP_NORMAL) {
			FMOD_Channel_SetMode(m_pChannel[iChannel], FMOD_LOOP_NORMAL);
		}
		if (iMode & FMOD_3D) {
			FMOD_Channel_SetMode(m_pChannel[iChannel], FMOD_3D_LINEARROLLOFF); //FMOD_3D, FMOD_3D_LINEARROLLOFF
			Set_3DAttribute(iChannel, pTransform, vVelocity);
			Set_Distance(iChannel, fDist.x, fDist.y);
		}
		FMOD_Channel_SetPaused(m_pChannel[iChannel], FALSE);
	}
}

void CAudio::Play_Sound_Rand(const _tchar* const pSoundKey, const _uint& iRand, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, CTransform* const pTransform, const _float3& vVelocity, const _float2& fDist) {
	_uint iIndex = rand() % iRand;
	wstring wstrIndex = TEXT("-") + to_wstring(iIndex);
	wstring wstrSoundKey = pSoundKey + wstrIndex;

	Play_Sound(wstrSoundKey.c_str(), iChannel, fVolume, iMode, pTransform, vVelocity, fDist);
}

void CAudio::Play_Sound2(const _tchar* const pSoundKey, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, _fmatrix matrix, const _float3& vVelocity, const _float2& fDist) {
	if ((iMode & FMOD_3D) && !(iMode & FMOD_LOOP_NORMAL)) {
		FMOD_VECTOR fvPos;
		FMOD_System_Get3DListenerAttributes(m_pSystem, 0, &fvPos, nullptr, nullptr, nullptr);
		_vector vPos = XMLoadFloat3((_float3*)&fvPos);
		_float vDist = XMVectorGetX(XMVector3Length(vPos - matrix.r[3]));
		if (vDist > fDist.y)
			return;
		else
			int i = 0;
	}
	FMOD_SOUND* pSound = Find_Sound(pSoundKey);
	if (nullptr == pSound) {
		for (_ubyte i = 0; i < m_pSound->Get_Scene(); ++i) {
			pSound = m_pSound->Get_Sound(i, pSoundKey);
			if (nullptr != pSound) {
				m_mapSound.emplace(pSoundKey, pSound);
				break;
			}
		}
		if (nullptr == pSound)
			return;
	}

	if (-1 == iChannel) {
		m_iChannel = m_iChannel < FMOD_MAX_CHANNEL_WIDTH - 1 ? m_iChannel + 1 : m_iMaxChannel;
		FMOD_Channel_Stop(m_pChannel[m_iChannel]);
		FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &m_pChannel[m_iChannel]);
		FMOD_Channel_SetVolume(m_pChannel[m_iChannel], fVolume);
		if (iMode & FMOD_2D) {
			FMOD_Channel_SetMode(m_pChannel[m_iChannel], FMOD_2D);
		}
		if (iMode & FMOD_LOOP_NORMAL) {
			FMOD_Channel_SetMode(m_pChannel[m_iChannel], FMOD_LOOP_NORMAL);
		}
		if (iMode & FMOD_3D) {
			FMOD_Channel_SetMode(m_pChannel[m_iChannel], FMOD_3D_LINEARROLLOFF);
			Set_3DAttribute(m_iChannel, matrix, vVelocity);
			Set_Distance(m_iChannel, fDist.x, fDist.y);
		}
		FMOD_Channel_SetPaused(m_pChannel[m_iChannel], FALSE);
	}
	else {
		FMOD_Channel_Stop(m_pChannel[iChannel]);
		FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &m_pChannel[iChannel]);
		FMOD_Channel_SetVolume(m_pChannel[iChannel], fVolume);
		if (iMode & FMOD_2D) {
			FMOD_Channel_SetMode(m_pChannel[iChannel], FMOD_2D);
		}
		if (iMode & FMOD_LOOP_NORMAL) {
			FMOD_Channel_SetMode(m_pChannel[iChannel], FMOD_LOOP_NORMAL);
		}
		if (iMode & FMOD_3D) {
			FMOD_Channel_SetMode(m_pChannel[iChannel], FMOD_3D_LINEARROLLOFF);
			Set_3DAttribute(iChannel, matrix, vVelocity);
			Set_Distance(iChannel, fDist.x, fDist.y);
		}
		FMOD_Channel_SetPaused(m_pChannel[iChannel], FALSE);
	}
}
void CAudio::Play_Sound_Rand2(const _tchar* const pSoundKey, const _uint& iRand, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, _fmatrix matrix, const _float3& vVelocity, const _float2& fDist) {
	_uint iIndex = rand() % iRand;
	wstring wstrIndex = TEXT("-") + to_wstring(iIndex);
	wstring wstrSoundKey = pSoundKey + wstrIndex;

	Play_Sound2(wstrSoundKey.c_str(), iChannel, fVolume, iMode, matrix, vVelocity, fDist);
}

void CAudio::Stop_Sound(const _ubyte& byChannel) {
	FMOD_Channel_Stop(m_pChannel[byChannel]);
}

void CAudio::Stop_Sound_All(void) {
	for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; ++i)
		FMOD_Channel_Stop(m_pChannel[i]);
}

FMOD_SOUND* const CAudio::Find_Sound(const _tchar* const& pSoundKey) const {
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](map<wstring, FMOD_SOUND*>::value_type sour) {
		return sour.first == pSoundKey;
		});
	if (m_mapSound.end() == iter)
		return nullptr;
	return iter->second;
}