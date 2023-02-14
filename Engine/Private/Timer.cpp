#include "Timer.h"

CTimer* const CTimer::Create(void) {
	CTimer* pInstnace = new CTimer;
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CTimer::CTimer(void)
	: CBase() {
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_PreTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

void CTimer::Delete(void) {
}

const HRESULT CTimer::Init(void) {
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_PreTime);
	QueryPerformanceCounter(&m_OriginTime);
	QueryPerformanceFrequency(&m_CpuTick);
	return S_OK;
}

void CTimer::Update(void) {
	QueryPerformanceCounter(&m_CurTime);
	if (m_CurTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart) {
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurTime;
	}
	m_dTimeDelta = static_cast<_double>(m_CurTime.QuadPart - m_PreTime.QuadPart) / m_CpuTick.QuadPart;
	m_PreTime = m_CurTime;
}