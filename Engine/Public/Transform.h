#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CTransform : public CComponent {
public:
	enum class ERow : _ubyte { RIGHT, UP, LOOK, POS, END };
public:
	static CTransform* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
protected:
	explicit CTransform(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg);
	virtual void Update_Edit(const _int& iPushID) override;
public:
	_matrix Get_Matrix(void) const;
	_matrix Get_Matrix_Inverse(void) const;
	_matrix Get_Matrix_Transpose(void) const;
	const _float4x4& Get_Float4x4(void) const;
	const _float4x4& Get_Float4x4_Inverse(void) const;
	const _float4x4 Get_Float4x4_Transpose(void) const;
	void Set_Matrix(const _float4x4& Matrix);
	void Set_Matrix(_fmatrix Matrix);

	_vector Get_Row(const ERow& eRow) const;
	const _float3& Get_Float3(const ERow& eRow) const;
	void Set_Row(const ERow& eRow, _fvector vState);

	void Translate(_fvector vDir, const _float& fDistance, const _double& dTimeDelta, class CNavi* pNavi = nullptr);
	void Rotation(_fvector vAxis, const _float& fRadian);
	void Rotate(_fvector vAxis, const _float& fRadian, const _double& dTimeDelta);

	const _float3 Get_Scale(void);
	void Set_Scale(const _float3& vScale);
	void Set_Scale(_fvector vScale);
	void Set_Scale(const ERow& eRow, const _float& fScale);

	const _float Get_Dist(CTransform* pTransform) const;
	const _float Get_Dist(_fvector	vTargetPos) const;

	void LookAt(_fvector vAt);
	void UpAt(_fvector vAt);
	const HRESULT Set_RawValue(class CShader* const& pShader);
public:
	void Trace(_fvector vTargetPos, const _float& fSpeed,const _float& fLimitDistance, const _double& dTimeDelta, class CNavi* pNavi = nullptr);
public:
	virtual void Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	_float4x4 m_Float4x4;
};
END_NAMESPACE