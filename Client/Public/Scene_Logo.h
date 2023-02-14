#pragma once
#include "Scene.h"

BEGIN_NAMESPACE(Client)
class CScene_Logo : public CScene {
public:
	static CScene_Logo* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
private:
	explicit CScene_Logo(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual ~CScene_Logo(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(const _double& dTimeDelta) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
};
END_NAMESPACE

