#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Client)
class CGame_Main final : public CBase {
public:
	static CGame_Main* const Create(void);
private:
	explicit CGame_Main(void);
	virtual ~CGame_Main(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	void Update(const _double& dTimeDelta);
	const HRESULT Render(void);
private:
	class CGame_Instance* m_pGame_Instance = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CNetwork* m_pNetwork = nullptr;
	class CInfo* m_pInfo = nullptr;
private:
	class CRenderer* m_pRenderer = nullptr;
#ifdef _DEBUG
	_tchar m_szFPS[_MAX_PATH] = L"";
	_double m_dAccTime = 0.0;
	_uint m_iFPS = 0;
#endif // _DEBUG


};
END_NAMESPACE