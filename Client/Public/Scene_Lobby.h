#pragma once
#include "Scene.h"

BEGIN_NAMESPACE(Client)
class CScene_Lobby final : public CScene {
public:
	static CScene_Lobby* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
private:
	explicit CScene_Lobby(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual ~CScene_Lobby(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(const _double& dTimeDelta) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
public:
	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
private:
	_float m_fStartTime = 0.f;
	_float m_fSoundVolume = 0.f;
};
END_NAMESPACE