#include "Sound.h"
#include <io.h>
#include "Transform.h"

IMPLEMENT_SINGLETON(CSound)

CSound::CSound(void)
	: CBase() {
	ZeroMemory(&m_pChannel, sizeof(FMOD_CHANNEL*) * static_cast<_ubyte>(EChannel::END));
}

void CSound::Delete(void) {
	for (_ubyte i = 0; i < m_byScene; ++i) {
		for (auto& pair : m_pMapSound[i])
			FMOD_Sound_Release(pair.second);
		m_pMapSound[i].clear();
	}
	Safe_Delete_Array(m_pMapSound);
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

const HRESULT CSound::Init(const _ubyte& byScene) {
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSystem, FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, NULL);
	FMOD_System_Set3DSettings(m_pSystem, 1.f, 0.1f, 1.f);

	m_byScene = byScene;
	m_pMapSound = new map<wstring, FMOD_SOUND*>[byScene];
	if (nullptr == m_pMapSound)
		return E_FAIL;
	return S_OK;
}

void CSound::Update(void) {
	FMOD_System_Update(m_pSystem);
}

const HRESULT CSound::Load_Sound(const _ubyte& byScene, const _tchar* const& pFilePath) {
	WIN32_FIND_DATA Find_Data;
	ZeroMemory(&Find_Data, sizeof(WIN32_FIND_DATA));

	HANDLE hFile = FindFirstFile(pFilePath, &Find_Data);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_tchar szDir[MAX_PATH] = TEXT("");
	_wsplitpath_s(pFilePath, nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	BOOL bResult = TRUE;
	while (bResult) {
		if (Find_Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (lstrcmp(Find_Data.cFileName, TEXT(".")) && lstrcmp(Find_Data.cFileName, TEXT(".."))) {
				_tchar szFilePath[MAX_PATH] = TEXT("");
				wsprintfW(szFilePath, TEXT("%s%s/*.*"), szDir, Find_Data.cFileName);
				if (FAILED(Load_Sound(byScene, szFilePath)))
					return E_FAIL;
			}
		}
		else {
			if (nullptr != wcsstr(Find_Data.cFileName, TEXT(".ogg")) ||
				nullptr != wcsstr(Find_Data.cFileName, TEXT(".wav"))) {
				_tchar szName[MAX_PATH] = TEXT("");
				_wsplitpath_s(Find_Data.cFileName, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

				_tchar szFilePath[MAX_PATH];
				wsprintfW(szFilePath, TEXT("%s%s"), szDir, Find_Data.cFileName);
				_char szFilePathA[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, szFilePath, -1, szFilePathA, MAX_PATH, NULL, NULL);

				FMOD_SOUND* pSound = nullptr;
				FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFilePathA, FMOD_3D_LINEARROLLOFF, 0, &pSound);
				if (FMOD_OK != eRes)
					return E_FAIL;
				m_pMapSound[byScene].emplace(szName, pSound);
			}
		}
		bResult = FindNextFile(hFile, &Find_Data);
	}
	FindClose(hFile);
	return S_OK;
}

FMOD_SYSTEM* const CSound::Get_System(void) {
	return m_pSystem;
}

FMOD_SOUND* const CSound::Get_Sound(const _ubyte& byScene, const _tchar* const& pSoundKey) {
	return Find_Sound(byScene, pSoundKey);
}

const _byte CSound::Get_Scene(void) {
	return m_byScene;
}

const HRESULT CSound::Clear_Sound(const _ubyte& byScene) const {
	if (byScene >= m_byScene || nullptr == m_pMapSound)
		return E_FAIL;
	for (auto& pair : m_pMapSound[byScene])
		FMOD_Sound_Release(pair.second);
	m_pMapSound[byScene].clear();
	return S_OK;
}

FMOD_SOUND* const CSound::Find_Sound(const _ubyte& byScene, const _tchar* const& pSoundKey) const {
	if (byScene >= m_byScene || nullptr == m_pMapSound)
		return nullptr;
	auto iter = find_if(m_pMapSound[byScene].begin(), m_pMapSound[byScene].end(), [&](map<wstring, FMOD_SOUND*>::value_type sour) {
		return sour.first == pSoundKey;
		});
	if (m_pMapSound[byScene].end() == iter)
		return nullptr;
	return iter->second;
}