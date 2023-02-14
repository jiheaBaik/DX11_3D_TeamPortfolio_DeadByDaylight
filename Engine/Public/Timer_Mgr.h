#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CTimer_Mgr final : public CBase {
	DECLARE_SINGLETON(CTimer_Mgr)
private:
	explicit CTimer_Mgr(void);
	virtual ~CTimer_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	void Update_Timer(const _tchar* const& pTimerKey);
public:
	const _double Get_Timer_TimeDelta(const _tchar* const& pTimerKey) const;
	const HRESULT Add_Timer(const _tchar* const& pTimerKey);
private:
	class CTimer* const Find_Timer(const _tchar* const& pTimerKey) const;
private:
	map<const _tchar* const, class CTimer*> m_mapTimer;
};
END_NAMESPACE