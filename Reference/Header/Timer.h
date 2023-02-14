#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CTimer final : public CBase {
public:
	static CTimer* const Create(void);
private:
	explicit CTimer(void);
	virtual ~CTimer(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	void Update(void);
public:
	const _double& Get_TimeDelta(void) const { return m_dTimeDelta; }
private:
	LARGE_INTEGER m_CurTime;
	LARGE_INTEGER m_PreTime;
	LARGE_INTEGER m_OriginTime;
	LARGE_INTEGER m_CpuTick;

	_double m_dTimeDelta = 0.;
};
END_NAMESPACE