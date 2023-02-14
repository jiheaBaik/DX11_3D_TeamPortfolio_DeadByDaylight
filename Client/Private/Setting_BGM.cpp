#include "pch.h"
#include "Setting_BGM.h"

CSetting_BGM* const CSetting_BGM::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CSetting_BGM* pInstnace = new CSetting_BGM(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CSetting_BGM::Clone(void* const& pArg) {
	CSetting_BGM* pInstnace = new CSetting_BGM(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CSetting_BGM::CSetting_BGM(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CSetting_BGM::CSetting_BGM(const CSetting_BGM& rhs)
	: CObject(rhs) {
}

void CSetting_BGM::Delete(void) {
	__super::Delete();
	Safe_Release(m_pAudio);
}

const HRESULT CSetting_BGM::Init_Create(void) {
	return S_OK;
}

const HRESULT CSetting_BGM::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));
	return hr;
}

const _ubyte CSetting_BGM::Update(const _double& dTimeDelta) {
	//for (_uint i = 0; i < 10; ++i) {
	//	if (m_iBGM == i) {
	//		m_fCurFade[i] += static_cast<_float>(dTimeDelta);
	//		if (m_fCurFade[i] > m_fFade[i])
	//			m_fCurFade[i] = m_fFade[i];
	//	}
	//	else {
	//		m_fCurFade[i] -= static_cast<_float>(dTimeDelta);
	//		if (m_fCurFade[i] < 0.f)
	//			m_fCurFade[i] = 0.f;
	//	}
	//	_float fRatio = m_fCurFade[i] / m_fFade[i];

	//	m_pAudio->Set_Volume(i, fRatio * m_fVolume[i]);
	//}
	return OBJ_NOEVENT;
}

void CSetting_BGM::Update_Edit(void) {
}

void CSetting_BGM::Late_Update(const _double& dTimeDelta) {
}

const HRESULT CSetting_BGM::Render(const CRenderer::EOrder& eOrder) {
	return S_OK;
}

void CSetting_BGM::Play_BGM(const _tchar* const pSoundKey, const _float& fVolume, const _float& fFade, const _bool& bLoop) {
	/*if (nullptr != pSoundKey) {
		if (m_wstrName[m_iBGM] == pSoundKey)
			return;
	}
	else {
		if (m_wstrName[m_iBGM] == L"")
			return;
	}
	m_fFade[m_iBGM] = fFade;
	m_fCurFade[m_iBGM] = m_fFade[m_iBGM];


	m_iBGM = m_iBGM >= 9 ? 0 : m_iBGM + 1;
	if (nullptr != pSoundKey)
		m_wstrName[m_iBGM] = pSoundKey;
	else
		m_wstrName[m_iBGM] = L"";
	m_fVolume[m_iBGM] = fVolume;
	m_fFade[m_iBGM] = fFade;
	m_fCurFade[m_iBGM] = 0.f;
	if (nullptr == pSoundKey)
		m_pAudio->Stop_Sound(m_iBGM);
	else {
		if (bLoop)
			m_pAudio->Play_Sound(pSoundKey, m_iBGM, 0.f, FMOD_2D | FMOD_LOOP_NORMAL);
		else
			m_pAudio->Play_Sound(pSoundKey, m_iBGM, 0.f, FMOD_2D);
	}*/
}

const wstring CSetting_BGM::Get_BGM_Name(void) {
	return m_wstrName[m_iBGM];
}
