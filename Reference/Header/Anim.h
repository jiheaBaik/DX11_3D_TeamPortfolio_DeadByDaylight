#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CAnim final : public CBase {
public:
	static CAnim* const Create(aiAnimation* const& paiAnim, const vector<class CBone*>& pVecBone);
	static CAnim* const Create(ifstream& ifs, const vector<class CBone*>& pVecBone);
	virtual CAnim* const Clone(void* const& pArg = nullptr);
private:
	explicit CAnim(void);
	explicit CAnim(const CAnim& rhs);
	virtual ~CAnim(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(aiAnimation* const& paiAnim, const vector<class CBone*>& pVecBone);
	const HRESULT Init_Create(ifstream& ifs, const vector<class CBone*>& pVecBone);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	const _uint Update(const _double& dTimeDelta, const _bool& bLoop = false);
public:
	const _char* const Get_Name(void) const;
	const _double& Get_Duration(void) const;
	const _double& Get_TimeAcc(void) const;
	const _uint Get_KeyFrame(void)const;
	void Set_TimeAcc(const _double& dTimeAcc);
	void Get_RootPos(vector<_float4>* Pos);
	void Get_dTime(vector<_double>* dTime);
	const _double& Get_TickPerSecond(void) const;
	const _bool Get_KeyTimeAcc(const _uint& iKeyTimeAcc) const;
public:
	void Save_Data(ofstream& ofs);
private:
	_char m_szName[MAX_PATH] = "";
	_double m_dDuration = 0.;
	_double m_dTickPerSecond = 0.;
	vector<class CChannel*> m_vecChannel;
private:
	_double m_dPreTimeAcc = 0.f;
	_double m_dTimeAcc = 0.;
};
END_NAMESPACE
