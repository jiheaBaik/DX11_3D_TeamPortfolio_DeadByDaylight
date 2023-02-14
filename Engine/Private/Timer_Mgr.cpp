#include "Timer_Mgr.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Mgr)

CTimer_Mgr::CTimer_Mgr(void)
	: CBase() {
}

void CTimer_Mgr::Delete(void) {
	for (auto& Pair : m_mapTimer)
		Safe_Release(Pair.second);
	m_mapTimer.clear();
}

void CTimer_Mgr::Update_Timer(const _tchar* const& pTimerKey) {
	CTimer* pTimer = Find_Timer(pTimerKey);
	if (nullptr == pTimer)
		return;
	pTimer->Update();
}

const _double CTimer_Mgr::Get_Timer_TimeDelta(const _tchar* const& pTimerKey) const {
	CTimer* pTimer = Find_Timer(pTimerKey);
	if (nullptr == pTimer)
		return 0.;
	return pTimer->Get_TimeDelta();
}

const HRESULT CTimer_Mgr::Add_Timer(const _tchar* const& pTimerKey) {
	if (nullptr != Find_Timer(pTimerKey))
		return E_FAIL;
	m_mapTimer.emplace(pTimerKey, CTimer::Create());
	return S_OK;
}

CTimer* const CTimer_Mgr::Find_Timer(const _tchar* const& pTimerKey) const {
	auto iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CMapFinder(pTimerKey));
	if (iter == m_mapTimer.end())
		return nullptr;
	return iter->second;
}