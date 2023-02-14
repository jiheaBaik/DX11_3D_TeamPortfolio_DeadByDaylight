#pragma once
#include "Engine_Define.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CBase abstract {
protected:
	explicit CBase(void);
	virtual ~CBase(void) = default;
	virtual void Delete(void) PURE;
public:
	const _ulong& AddRef(void);
	const _ulong& Get_Ref(void);
	const _ulong Release(void);
private:
	_ulong m_dwRefCnt = 0;
};
END_NAMESPACE