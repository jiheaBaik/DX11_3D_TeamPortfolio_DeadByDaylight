#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CEnviro abstract : public CObject{
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CEnviro(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEnviro(const CEnviro& rhs);
	virtual ~CEnviro(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const _ubyte Update(const _double& dTimeDelta) PURE;
	virtual void Late_Update(const _double& dTimeDelta) PURE;
	virtual const HRESULT Render(const CRenderer::EOrder& Order) PURE;
public:
	ID3D11ShaderResourceView* const Get_CubeSRV(void);
protected:
	ID3D11Texture2D* m_pTextureCube = nullptr;
	ID3D11ShaderResourceView* m_pCubeSRV = nullptr;
};
END_NAMESPACE