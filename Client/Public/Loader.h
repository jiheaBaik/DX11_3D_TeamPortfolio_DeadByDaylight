#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Client)
class CLoader final : public CBase {
public:
	static CLoader* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EScene& eScene);
private:
	explicit CLoader(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CLoader(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const EScene& eScene);
	void Update(void);
public:
	const _bool& Get_IsFinish(void) const { return m_bIsFinish; }
	const EScene& Get_EScene(void) const { return m_eScene; }
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	HANDLE m_hThread = 0;
	CRITICAL_SECTION m_Critical_Section;

	EScene m_eScene = EScene::END;
	_bool m_bIsFinish = false;
};
END_NAMESPACE