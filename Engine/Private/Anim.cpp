#include "Anim.h"
#include "Channel.h"

CAnim* const CAnim::Create(aiAnimation* const& paiAnim, const vector<class CBone*>& pVecBone) {
	CAnim* pInstance = new CAnim();
	if (FAILED(pInstance->Init_Create(paiAnim, pVecBone)))
		Safe_Release(pInstance);
	return pInstance;
}

CAnim* const CAnim::Create(ifstream& ifs, const vector<class CBone*>& pVecBone) {
	CAnim* pInstance = new CAnim();
	if (FAILED(pInstance->Init_Create(ifs, pVecBone)))
		Safe_Release(pInstance);
	return pInstance;
}

CAnim* const CAnim::Clone(void* const& pArg) {
	CAnim* pInstance = new CAnim(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CAnim::CAnim(void)
	: CBase() {
}

CAnim::CAnim(const CAnim& rhs)
	: CBase(),
	m_dDuration(rhs.m_dDuration),
	m_dTickPerSecond(rhs.m_dTickPerSecond),
	m_vecChannel(rhs.m_vecChannel) {
	strcpy_s(m_szName, rhs.m_szName);
	for (auto& iter : m_vecChannel)
		Safe_AddRef(iter);
}

void CAnim::Delete(void) {
	for (auto& iter : m_vecChannel)
		Safe_Release(iter);
	m_vecChannel.clear();
}

const HRESULT CAnim::Init_Create(aiAnimation* const& paiAnim, const vector<class CBone*>& pVecBone) {
	strcpy_s(m_szName, paiAnim->mName.data);
	m_dDuration = paiAnim->mDuration;
	m_dTickPerSecond = paiAnim->mTicksPerSecond;
	for (_uint i = 0; i < paiAnim->mNumChannels; ++i) {
		CChannel* pChannel = CChannel::Create(paiAnim->mChannels[i], pVecBone);
		if (nullptr == pChannel)
			return E_FAIL;
		m_vecChannel.emplace_back(pChannel);
	}
	return S_OK;
}

const HRESULT CAnim::Init_Create(ifstream& ifs, const vector<class CBone*>& pVecBone) {
	ifs.read(reinterpret_cast<char*>(m_szName), sizeof(char) * MAX_PATH);
	ifs.read(reinterpret_cast<char*>(&m_dDuration), sizeof(_double));
	ifs.read(reinterpret_cast<char*>(&m_dTickPerSecond), sizeof(_double));
	size_t iSize = 0;
	ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
	for (_uint i = 0; i < iSize; ++i) {
		CChannel* pChannel = CChannel::Create(ifs, pVecBone);
		if (nullptr == pChannel)
			return E_FAIL;
		m_vecChannel.emplace_back(pChannel);
	}
	return S_OK;
}

const HRESULT CAnim::Init_Clone(void* const& pArg) {
	vector<class CChannel*> vecChannel;
	for (auto& iter : m_vecChannel) {
		CChannel* pChannel = iter->Clone(pArg);
		Safe_Release(iter);
		vecChannel.emplace_back(pChannel);
	}
	m_vecChannel.clear();
	m_vecChannel = vecChannel;
	return S_OK;
}

const _uint CAnim::Update(const _double& dTimeDelta, const _bool& bLoop) {
	m_dPreTimeAcc = m_dTimeAcc;
	m_dTimeAcc += m_dTickPerSecond * dTimeDelta;
	_uint iEvent = ANIM_NOEVENT;
	if (bLoop) {
		if (m_dTimeAcc >= m_dDuration) {
			m_dTimeAcc = 0.;
			iEvent = ANIM_END;
		}
	}
	else {
		if (m_dTimeAcc >= m_dDuration) {
			m_dTimeAcc = m_dDuration;
			iEvent = ANIM_END;
		}
	}
	for (auto& iter : m_vecChannel)
		iter->Update(m_dTimeAcc);
	return iEvent;
}

const _char* const CAnim::Get_Name(void) const {
	return m_szName;
}

const _double& CAnim::Get_Duration(void) const {
	return m_dDuration;
}

const _double& CAnim::Get_TimeAcc(void) const {
	return m_dTimeAcc;
}

const _uint CAnim::Get_KeyFrame(void) const {
	for (auto& iter : m_vecChannel) {
		if (!strcmp(iter->Get_ChannelBoneName(), "joint_Char"))
			return iter->Get_KeyFrame();
	}
	return 0;
}

void CAnim::Set_TimeAcc(const _double& dTimeAcc) {
	m_dTimeAcc = dTimeAcc;
	m_dPreTimeAcc = m_dTimeAcc;
	for (auto& iter : m_vecChannel)
		iter->Update(m_dTimeAcc);
}

void CAnim::Get_RootPos(vector<_float4>* vecPos) {
	vector<_float4> rootpos;
	auto	iter = find_if(m_vecChannel.begin(), m_vecChannel.end(), [&](CChannel* pChannel) {
		return !strcmp("joint_Char", pChannel->Get_ChannelBoneName());
		});
	if (m_vecChannel.end() == iter)
		return;
	else {
		for (_uint i = 0; i < (*iter)->Get_KeyframeCount(); i++)
			(*vecPos).push_back((*iter)->RootUpdate(i));
	}
}

void CAnim::Get_dTime(vector<_double>* vecdTime) {
	vector<_double> rootpos;
	auto	iter = find_if(m_vecChannel.begin(), m_vecChannel.end(), [&](CChannel* pChannel) {
		return !strcmp("joint_Char", pChannel->Get_ChannelBoneName());
		});
	if (m_vecChannel.end() == iter)
		return;
	else {
		for (_uint i = 0; i < (*iter)->Get_KeyframeCount(); i++)
			(*vecdTime).push_back((*iter)->Get_dTime(i));
	}
}

const _double& CAnim::Get_TickPerSecond() const {
	return m_dTickPerSecond;
}

const _bool CAnim::Get_KeyTimeAcc(const _uint& iKeyTimeAcc) const {
	if (ceil(m_dPreTimeAcc) < iKeyTimeAcc && iKeyTimeAcc <= ceil(m_dTimeAcc))
		return true;
	return false;
}

void CAnim::Save_Data(ofstream& ofs) {
	ofs.write(reinterpret_cast<const char*>(m_szName), sizeof(char) * MAX_PATH);
	ofs.write(reinterpret_cast<const char*>(&m_dDuration), sizeof(_double));
	ofs.write(reinterpret_cast<const char*>(&m_dTickPerSecond), sizeof(_double));
	size_t iSize = m_vecChannel.size();
	ofs.write(reinterpret_cast<const char*>(&iSize), sizeof(size_t));
	for (auto& iter : m_vecChannel)
		iter->Save_Data(ofs);
}
