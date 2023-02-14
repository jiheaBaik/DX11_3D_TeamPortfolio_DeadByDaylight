#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CSetting_BGM final : public CObject {
public:
	static CSetting_BGM* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CSetting_BGM(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CSetting_BGM(const CSetting_BGM& rhs);
	virtual ~CSetting_BGM(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Play_BGM(const _tchar* const pSoundKey, const _float& fVolume, const _float& fFade = 0.f, const _bool& bLoop = true);
	const wstring Get_BGM_Name(void);
private:
	class CAudio* m_pAudio = nullptr;

	_uint m_iBGM = 0;
	wstring m_wstrName[10];
	_float m_fVolume[10];
	_float m_fFade[10];
private:
	_float m_fCurFade[10];
};
END_NAMESPACE