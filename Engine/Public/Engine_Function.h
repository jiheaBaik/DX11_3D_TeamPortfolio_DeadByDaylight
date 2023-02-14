#pragma once

namespace Engine {
	template<typename T>
	void Safe_Delete(T& pPointer) {
		if (nullptr == pPointer)
			return;
		delete pPointer;
		pPointer = nullptr;
	}

	template<typename T>
	void Safe_Delete_Array(T& pPointer) {
		if (nullptr == pPointer)
			return;
		delete[] pPointer;
		pPointer = nullptr;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance) {
		if (nullptr == pInstance)
			return 0;
		unsigned long dwRefCnt = 0;
		dwRefCnt = pInstance->AddRef();
		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance) {
		if (nullptr == pInstance)
			return 0;
		unsigned long dwRefCnt = 0;
		dwRefCnt = pInstance->Release();
		if (0 == dwRefCnt)
			pInstance = nullptr;
		return dwRefCnt;
	}

	class CMapFinder {
	private:
		const wchar_t* m_pStr;
	public:
		CMapFinder(const wchar_t* pStr)
			: m_pStr(pStr) {
		}
	public:
		template<typename T>
		bool operator()(T& pair) {
			return !lstrcmp(m_pStr, pair.first);
		}
	};

	static POINT Get_MousePt(const HWND& _hWnd) {
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(_hWnd, &ptMouse);
		return ptMouse;
	}
}