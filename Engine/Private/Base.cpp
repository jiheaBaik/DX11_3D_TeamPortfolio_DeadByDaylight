#include "Base.h"

CBase::CBase(void) {
	AddRef();
}

const _ulong& CBase::AddRef(void) {
	return ++m_dwRefCnt;
}

const _ulong& CBase::Get_Ref(void) {
	return m_dwRefCnt;
}

const _ulong CBase::Release(void) {
	if (0 == --m_dwRefCnt) {
		Delete();
		delete this;
		return 0;
	}
	return m_dwRefCnt;
}