#pragma once
#include "Scene.h"

BEGIN_NAMESPACE(Client)
class CScene_GamePlay final : public CScene {
public:
	static CScene_GamePlay* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
private:
	explicit CScene_GamePlay(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual ~CScene_GamePlay(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(const _double& dTimeDelta) override;
	virtual void Late_Update(const _double& dTimeDelta) override;

	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
public:
	void Plus_CamperKillCount(void) { ++m_iCamperKillCount; }
private:
	_bool m_bClear = true;
	_bool m_bGameEnd = false;
	_float m_fEndingTime = 0.f;
	_bool m_bUI_FadeIn = false;
	_bool m_bUI_FadeOut = false;
	_bool m_bUI_First = false;
	_bool m_bUI_Second = false;
	class CAudio* m_pAudio = false;
private:
	_uint m_iCamperCount = 0;
	_uint m_iCamperKillCount = 0;

};
END_NAMESPACE