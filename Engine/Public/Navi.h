#pragma once
#include "Component.h"
#include "Cell.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CNavi final : public CComponent{
public:
	struct SNavi {
		_int iCurIndex;
	};
public:
	static CNavi* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath = nullptr);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CNavi(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CNavi(const CNavi& rhs);
	virtual ~CNavi(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const _tchar* const& pFilePath = nullptr);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual void Update_Edit(const _int& iPushID) override;
	const HRESULT Render(void);
public:
	const _int& Get_Index(void);
	void Set_Index(_int iCurIndex);
	const _bool Is_Move(_fvector vPos);
	void Check_Cell(_float3* const& pPoint);
	const HRESULT Create_Cell(_float3* const& pPoint, const CCell::EType & eType);
	vector<class CCell*>& Get_Cell(void);
	void Delete_Cell(class CCell* pCell);
	_vector Get_FloorPos(_fvector vPos);
public:
	virtual void Save_Data(Json::Value & root) override;
	virtual void Load_Data(Json::Value & root) override;
public:
	void Save_Data(ofstream & ofs);
private:
	_int m_iCurIndex = -1;
	vector<class CCell*> m_vecCell;
	class CShader* m_pShader = nullptr;
public:
	void Clear_DivCell(void);
	void Set_DivCell(const _float& fDivCell);
	const _bool Check_DivCell(const _uint x, const _uint& z, class CCell* const pCell);
	list<class CCell*> Get_DivCell(class CTransform* const pTransform);
	vector<class CCell*> Get_DivCell(const _float3& vSour, const _float3& vDest);
private:
	static _float s_fDivCell;
	static vector<class CCell*> s_vecCell[100][100];
};
END_NAMESPACE