#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CChannel final : public CBase {
public:
	struct SKeyFrame {
		_double dTime = 0.;
		_float4 vPos = _float4{ 0.f,0.f ,0.f ,1.f };
		_float4 vRot = _float4{ 0.f,0.f ,0.f ,0.f };
		_float3 vScale = _float3{ 0.f,0.f,0.f };
	};
public:
	static CChannel* const Create(aiNodeAnim* const& paiChannel, const vector<class CBone*>& pVecBone);
	static CChannel* const Create(ifstream& ifs, const vector<class CBone*>& pVecBone);
	virtual CChannel* const Clone(void* const& pArg = nullptr);
private:
	explicit CChannel(void);
	explicit CChannel(const CChannel& rhs);
	virtual ~CChannel(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(aiNodeAnim* const& paiChannel, const vector<class CBone*>& pVecBone);
	const HRESULT Init_Create(ifstream& ifs, const vector<class CBone*>& pVecBone);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	void Update(const _double& dTimeAcc);
	_float4 RootUpdate(const _uint& dTimeAcc);
	const _uint Get_KeyframeCount() const;
	const _char* Get_ChannelBoneName();
	const _uint& Get_KeyFrame() const;
	const _double& Get_dTime(const _uint& dTimeAcc) const;
public:
	void Save_Data(ofstream& ofs);
private:
	vector<SKeyFrame> m_vecKeyFrame;
	_uint m_iKeyFrame = 0;
	class CBone* m_pBone = nullptr;
};
END_NAMESPACE

