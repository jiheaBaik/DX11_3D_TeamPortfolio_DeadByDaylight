#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CBone final : public CBase{
public:
	static CBone* const Create(const char* const& pName, _fmatrix Matrix, class CBone* const& pParent, const _uint& iDepth);
	static CBone* const Create(ifstream& ifs, const vector<class CBone*>& pVecBone);
	virtual CBone* const Clone(void* const& pArg = nullptr);
private:
	explicit CBone(void);
	explicit CBone(const CBone& rhs);
	virtual ~CBone(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const char* const& pName, _fmatrix Matrix, class CBone* const& pParent, const _uint& iDepth);
	const HRESULT Init_Create(ifstream& ifs, const vector<class CBone*>& pVecBone);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	void Update(const _float& fRatio);
public:
	const _char* const Get_Name(void) const;
	const _uint& Get_Index(void) const;
	const _uint& Get_Depth(void) const;
	_matrix Get_CombineMatrix(void) const;
	_matrix Get_OffsetMatrix(void) const;
	const _float4x4& Get_OffsetFloat4x4(void);
	_matrix Get_RootMatrix(void);
	void Set_Index(const _uint& iIndex);
	_matrix Get_Matrix(void);
	void Set_Matrix(_fmatrix Matrix);
	void Set_OffsetMatrix(_fmatrix OffsetMatrix);
	void Set_PerMatrix(void);
public:
	void Save_Data(ofstream& ofs);
private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iIndex = 0;
	_float4x4 m_Float4x4;
	_float4x4 m_PerFloat4x4;
	_float4x4 m_CombineFloat4x4;
	_float4x4 m_OffsetFloat4x4;
	_float4x4 m_RootFloat4x4;
	CBone* m_pParent = nullptr;
	_uint m_iDepth = 0;
};
END_NAMESPACE