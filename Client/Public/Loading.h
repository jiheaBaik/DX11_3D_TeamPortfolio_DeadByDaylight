#pragma once
#include "Scene.h"

BEGIN_NAMESPACE(Client)
class CLoading final : public CScene {
public:
	static CLoading* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, const EScene& eScene);
private:
	explicit CLoading(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual ~CLoading(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const EScene& eScene);
	virtual void Update(const _double& dTimeDelta) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
private:
	class CLoader* m_pLoader = nullptr;
};
END_NAMESPACE