#pragma once

#define CONSOLE_PRINT(szMessage, value)					\
	cout << #szMessage << value << endl;
#define CONSOLE_CLEAR									\
	system("cls");

#define CONVERT_MULTI_TO_WIDE(pMultiBuf, pWideBuf)					\
MultiByteToWideChar(CP_ACP, 0, pMultiBuf, (int)strlen(pMultiBuf) + 1, pWideBuf, _MAX_PATH);

#define CONVERT_STR_TO_WIDE(str, pWideBuf)							\
MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length() + 1, pWideBuf, _MAX_PATH);	

#define CONVERT_WIDE_TO_MULTI(pWideBuf, pMultiBuf)					 \
WideCharToMultiByte(CP_ACP, 0, pWideBuf, -1, pMultiBuf, _MAX_PATH, 0, 0);

#define CONVERT_WSTR_TO_MULTI(wstr, pMultiBuf)						\
WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pMultiBuf, _MAX_PATH, 0, 0);	
