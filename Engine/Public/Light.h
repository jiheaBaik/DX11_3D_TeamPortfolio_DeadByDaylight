#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CLight abstract : public CObject{
public:
	enum class EType : _byte { DIR, POINT, SPOT, END };
	struct SDesc {
		const _tchar* pKey = nullptr;
		EType eType = EType::DIR;
		_float fRange = 20.0f;
		_float4 vDiffuse = _float4{ 1.f, 1.f, 1.f, 1.f };
		_float4 vAmbient = _float4{ 1.f, 1.f, 1.f, 1.f };
		_float4 vSpecular = _float4{ 1.f, 1.f, 1.f, 1.f };
		_float2 fAngle = _float2{ 0.f, 0.f };
	};
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CLight(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CLight(const CLight& rhs);
	virtual ~CLight(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) PURE;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) PURE;
	virtual const HRESULT Render(const CRenderer::EOrder& Order) PURE;
public:
	const EType& Get_Type(void) const;
	_vector Get_Pos(void) const;
	_vector Get_Look(void) const;
	const _float& Get_Range(void) const;
	const _float4& Get_Diffuse(void) const;
	const _float4& Get_Ambient(void) const;
	const _float4& Get_Specular(void) const;
	const HRESULT Set_RawValue(class CShader* const& pShader);
	const _bool& Get_OnOff(void) const;
	void Set_OnOff(const _bool& bOnOff);
protected:
	class CTransform* m_pTransform = nullptr;
	SDesc m_sDesc;
protected:
	char m_szKey[MAX_PATH] = "";
	_bool m_bOnOff = true;
public:
	static class CLight* const Get_Light(const _uint& iLight);
	static class CLight* const Get_Light(const _tchar* const& pLightKey);
	static vector<class CLight*> const& Get_VecLight(void);
protected:
	static vector<class CLight*> s_vecLight;
	static map<wstring, class CLight*> s_mapLight;
};
END_NAMESPACE