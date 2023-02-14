#pragma once
#include "pch.h"

static const _tchar* Open_FileDialog(const _tchar* const& pFilePath = nullptr)
{
	_tchar szFilter[] = TEXT("��� ����\0*.*\0txt ����\0*.txt\0dds ����\0*.dds\0fbx ����\0*.fbx\0");
	_tchar szFilePath[_MAX_PATH] = TEXT("");
	if (nullptr == pFilePath)
	{
		GetCurrentDirectory(_MAX_PATH, szFilePath);
		PathRemoveFileSpec(szFilePath);
		lstrcat(szFilePath, TEXT("\\Bin\\Resource\\00.Sample\\*.*"));
	}
	else
		lstrcpy(szFilter, pFilePath);

	OPENFILENAME ofn;
	::memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = szFilter;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrFile = szFilePath;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrInitialDir = nullptr;
	if (0 != ::GetOpenFileName(&ofn)) {
		HANDLE	hFile = CreateFile(szFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
			return nullptr;
		DWORD	dwByte = 0;
		while (true) {
			//ReadFile(hFile, &a, sizeof(a), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		CloseHandle(hFile);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.IniFilename = NULL;

		return ofn.lpstrFile;
	}

	return nullptr;
}

static const HRESULT Load_Texture(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EScene& eScene, const _tchar* const& pFilePath) {
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
				if (FAILED(Load_Texture(pDevice, pContext, eScene, szFilePath)))
					return E_FAIL;
			}
		}
		else {
			_tchar szKey[MAX_PATH] = TEXT("");
			_tchar szFilePath[MAX_PATH] = TEXT("");
			_uint iTexture = 1;
			if (nullptr != wcschr(Find_Data.cFileName, TEXT('-'))) {
				_tchar* pContext = nullptr;
				_tchar* pFileName = wcstok_s(Find_Data.cFileName, TEXT("-"), &pContext);

				iTexture = 0;
				WIN32_FIND_DATA Find_Data2;
				ZeroMemory(&Find_Data2, sizeof(WIN32_FIND_DATA));
				HANDLE hFile2 = FindFirstFile(pFilePath, &Find_Data2);
				if (INVALID_HANDLE_VALUE == hFile2)
					return E_FAIL;
				BOOL bResult2 = TRUE;
				while (bResult2) {
					if (nullptr != wcsstr(Find_Data2.cFileName, pFileName))
						++iTexture;
					bResult2 = FindNextFile(hFile2, &Find_Data2);
				}
				FindClose(hFile2);

				_tchar szExt[MAX_PATH] = TEXT("");
				if (nullptr != pContext)
					_wsplitpath_s(pContext, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

				wsprintfW(szKey, TEXT("Texture_%s"), pFileName);
				wsprintfW(szFilePath, TEXT("%s%s%s%s"), szDir, pFileName, TEXT("-%d"), szExt);
			}
			else {
				_tchar szName[MAX_PATH] = TEXT("");
				_wsplitpath_s(Find_Data.cFileName, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);
				wsprintfW(szKey, TEXT("Texture_%s"), szName);
				wsprintfW(szFilePath, TEXT("%s%s"), szDir, Find_Data.cFileName);
			}

			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			if (FAILED(pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(eScene), szKey, CTexture::Create(pDevice, pContext, szFilePath, iTexture))))
				return E_FAIL;
			for (_uint i = 0; i < iTexture - 1; ++i)
				bResult = FindNextFile(hFile, &Find_Data);
		}
		bResult = FindNextFile(hFile, &Find_Data);
	}
	FindClose(hFile);
	return S_OK;
}

static const HRESULT Load_Model(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EScene& eScene, const _tchar* const& pFilePath) {
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
				if (FAILED(Load_Model(pDevice, pContext, eScene, szFilePath)))
					return E_FAIL;
			}
		}
		else {
			if (nullptr != wcsstr(Find_Data.cFileName, TEXT(".fbx"))) {
				_tchar szName[MAX_PATH] = TEXT("");
				_wsplitpath_s(Find_Data.cFileName, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

				_tchar szKey[MAX_PATH] = TEXT("");
				wsprintfW(szKey, TEXT("Model_%s"), szName);
				_tchar szFilePath[MAX_PATH] = TEXT("");
				wsprintfW(szFilePath, TEXT("%sinfo.txt"), szDir);

				ifstream json_file(szFilePath);
				Json::Value root;
				json_file >> root;

				_matrix PivotMatrix = XMMatrixScaling(root["Scale"]["x"].asFloat(), root["Scale"]["y"].asFloat(), root["Scale"]["z"].asFloat())
					* XMMatrixRotationY(XMConvertToRadians(root["Rot"]["y"].asFloat()));

				char szDirA[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, szDir, -1, szDirA, MAX_PATH, NULL, NULL);
				char szNameA[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, Find_Data.cFileName, -1, szNameA, MAX_PATH, NULL, NULL);

				CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
				if (FAILED(pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(eScene), szKey, CModel::Create(pDevice, pContext, static_cast<CModel::EType>(root["EModel"].asUInt()), root["Inst"].asInt(), szDirA, szNameA, PivotMatrix))))
					return E_FAIL;

				json_file.close();
			}
		}
		bResult = FindNextFile(hFile, &Find_Data);
	}
	FindClose(hFile);
	return S_OK;
}