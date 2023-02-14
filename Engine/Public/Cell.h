#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CCell final : public CBase{
public:
	enum class EPoint : _ubyte { A, B, C, END };
	enum class ELine : _ubyte { AB, BC, CA, END };
	enum class EType : _ubyte { FLOOR, WALL, CEILING, NOCAMERAWALL, END };
public:
	static CCell* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, _float3* const& pPoint, const EType& eType, const _uint& iIndex);
private:
	explicit CCell(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CCell(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(_float3* const& pPoint, const EType & eType, const _uint & iIndex);
	const HRESULT Render(class CShader* const& pShader, const _uint & iPass);
public:
	const _uint& Get_Index(void) const;
	_vector Get_Point(const EPoint & ePoint) const;
	_float3* Get_Point(void);
	const _bool Compare_Point(_fvector vSour, _fvector vDest);
	void Set_Neighbor(const ELine & eLine, const _uint & iIndex);
	const _bool Is_In(_fvector vPos, _int & iNeighbor);
	class CBuffer* const Get_Buffer(void);
	void Set_Type(const EType & eType);
	const EType& Get_Type(void) const;
	_vector Get_Cross(void);
	void Set_Point(const _float3* const& vPoint);
public:
	void Save_Data(ofstream & ofs);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_uint m_iIndex = 0;

	_float3 m_vPoint[static_cast<_ubyte>(EPoint::END)];
	_float3 m_vLine[static_cast<_ubyte>(ELine::END)];
	_float3 m_vNormal[static_cast<_ubyte>(ELine::END)];

	_int m_iNeighbor[static_cast<_ubyte>(ELine::END)] = { -1, -1, -1 };
	class CTriangle* m_pBuffer = nullptr;
private:
	EType m_eType = EType::END;
	_float3 m_vCross;
};
END_NAMESPACE