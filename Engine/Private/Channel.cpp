#include "Channel.h"
#include "Bone.h"

CChannel* const CChannel::Create(aiNodeAnim* const& paiChannel, const vector<class CBone*>& pVecBone) {
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->Init_Create(paiChannel, pVecBone)))
		Safe_Release(pInstance);
	return pInstance;
}

CChannel* const CChannel::Create(ifstream& ifs, const vector<class CBone*>& pVecBone) {
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->Init_Create(ifs, pVecBone)))
		Safe_Release(pInstance);
	return pInstance;
}

CChannel* const CChannel::Clone(void* const& pArg) {
	CChannel* pInstance = new CChannel(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CChannel::CChannel(void)
	: CBase() {
}

CChannel::CChannel(const CChannel& rhs)
	: CBase(),
	m_vecKeyFrame(rhs.m_vecKeyFrame),
	m_pBone(rhs.m_pBone) {
	Safe_AddRef(m_pBone);
}

void CChannel::Delete(void) {
	Safe_Release(m_pBone);
}

const HRESULT CChannel::Init_Create(aiNodeAnim* const& paiChannel, const vector<class CBone*>& pVecBone) {
	_uint iMaxKeyFrame = max(paiChannel->mNumScalingKeys, paiChannel->mNumRotationKeys);
	iMaxKeyFrame = max(iMaxKeyFrame, paiChannel->mNumPositionKeys);

	SKeyFrame KeyFrame;
	for (_uint i = 0; i < iMaxKeyFrame; ++i) {
		if (i < paiChannel->mNumPositionKeys) {
			memcpy(&KeyFrame.vPos, &paiChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.dTime = paiChannel->mPositionKeys[i].mTime;
		}
		if (i < paiChannel->mNumRotationKeys) {
			KeyFrame.vRot.x = paiChannel->mRotationKeys[i].mValue.x;
			KeyFrame.vRot.y = paiChannel->mRotationKeys[i].mValue.y;
			KeyFrame.vRot.z = paiChannel->mRotationKeys[i].mValue.z;
			KeyFrame.vRot.w = paiChannel->mRotationKeys[i].mValue.w;
			KeyFrame.dTime = paiChannel->mRotationKeys[i].mTime;
		}
		if (i < paiChannel->mNumScalingKeys) {
			memcpy(&KeyFrame.vScale, &paiChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.dTime = paiChannel->mScalingKeys[i].mTime;
		}
		m_vecKeyFrame.emplace_back(KeyFrame);
	}

	auto iter = find_if(pVecBone.begin(), pVecBone.end(), [&](CBone* pTemp) {
		return !strcmp(paiChannel->mNodeName.data, pTemp->Get_Name());
		});
	if (pVecBone.end() == iter)
		return E_FAIL;
	m_pBone = *iter;
	Safe_AddRef(m_pBone);
	return S_OK;
}

const HRESULT CChannel::Init_Create(ifstream& ifs, const vector<class CBone*>& pVecBone) {
	size_t iSize = 0;
	ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
	SKeyFrame KeyFrame;
	for (_uint i = 0; i < iSize; ++i) {
		ifs.read(reinterpret_cast<char*>(&KeyFrame), sizeof(SKeyFrame));
		m_vecKeyFrame.emplace_back(KeyFrame);
	}
	_uint iIndex = 0;
	ifs.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
	m_pBone = pVecBone[iIndex];
	Safe_AddRef(m_pBone);
	return S_OK;
}

const HRESULT CChannel::Init_Clone(void* const& pArg) {
	CBone* pBone = (*reinterpret_cast<vector<class CBone*>*>(pArg))[m_pBone->Get_Index()];
	Safe_Release(m_pBone);
	m_pBone = pBone;	
	Safe_AddRef(m_pBone);
	return S_OK;
}

void CChannel::Update(const _double& dTimeAcc) {
	_vector vPos, vScale, vRot;

	if (dTimeAcc >= m_vecKeyFrame.back().dTime) {
		vPos = XMLoadFloat4(&m_vecKeyFrame.back().vPos);
		vRot = XMLoadFloat4(&m_vecKeyFrame.back().vRot);
		vScale = XMLoadFloat3(&m_vecKeyFrame.back().vScale);
	}
	else {
		if (dTimeAcc < m_vecKeyFrame[1].dTime)
			m_iKeyFrame = 0;

		while (dTimeAcc > m_vecKeyFrame[m_iKeyFrame + 1].dTime)
			++m_iKeyFrame;

		_float fRatio = static_cast<_float>((dTimeAcc - m_vecKeyFrame[m_iKeyFrame].dTime) / (m_vecKeyFrame[m_iKeyFrame + 1].dTime - m_vecKeyFrame[m_iKeyFrame].dTime));

		_vector vSourPos = XMLoadFloat4(&m_vecKeyFrame[m_iKeyFrame].vPos);
		_vector vDestPos = XMLoadFloat4(&m_vecKeyFrame[m_iKeyFrame + 1].vPos);

		_vector vSourRot = XMLoadFloat4(&m_vecKeyFrame[m_iKeyFrame].vRot);
		_vector vDestRot = XMLoadFloat4(&m_vecKeyFrame[m_iKeyFrame + 1].vRot);

		_vector vSourScale = XMLoadFloat3(&m_vecKeyFrame[m_iKeyFrame].vScale);
		_vector vDestScale = XMLoadFloat3(&m_vecKeyFrame[m_iKeyFrame + 1].vScale);

		vPos = XMVectorLerp(vSourPos, vDestPos, fRatio);
		vRot = XMQuaternionSlerp(vSourRot, vDestRot, fRatio);
		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
	}
	m_pBone->Set_Matrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vPos));
}

_float4 CChannel::RootUpdate(const _uint& dTimeAcc){
	_matrix affine = XMMatrixAffineTransformation(XMLoadFloat3(&m_vecKeyFrame[dTimeAcc].vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&m_vecKeyFrame[dTimeAcc].vRot), XMLoadFloat4(&m_vecKeyFrame[dTimeAcc].vPos));
	_float4 vPos;
	XMStoreFloat4(&vPos, affine.r[3]);
	return vPos;
}

const _uint CChannel::Get_KeyframeCount() const{
	return static_cast<_uint>(m_vecKeyFrame.size());
}

const _char* CChannel::Get_ChannelBoneName(){
	return m_pBone->Get_Name();
}

const _uint& CChannel::Get_KeyFrame()const {
	return m_iKeyFrame;
}

const _double& CChannel::Get_dTime(const _uint& dTimeAcc) const{
	return  m_vecKeyFrame[dTimeAcc].dTime;
}

void CChannel::Save_Data(ofstream& ofs) {
	size_t iSize = m_vecKeyFrame.size();
	ofs.write(reinterpret_cast<const char*>(&iSize), sizeof(size_t));
	for (auto& iter : m_vecKeyFrame)
		ofs.write(reinterpret_cast<const char*>(&iter), sizeof(SKeyFrame));
	ofs.write(reinterpret_cast<const char*>(&m_pBone->Get_Index()), sizeof(_uint));
}