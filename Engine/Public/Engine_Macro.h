#pragma once

#define OBJ_NOEVENT 0
#define OBJ_ERASE 1
#define OBJ_SCENE 2

#define ANIM_NOEVENT 0
#define ANIM_END 1

#define BEGIN_NAMESPACE(Namespace) namespace Namespace {
#define END_NAMESPACE }

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)

#ifdef EXPORT_DLL
#define DECLSPEC_DLL _declspec(dllexport)
#else
#define DECLSPEC_DLL _declspec(dllimport)
#endif

#define MSG_BOX(Message) MessageBox(0, TEXT(Message), TEXT("Error"), MB_OK)

#define NO_COPY(ClassName)								\
private:												\
	explicit ClassName(const ClassName&) = delete;		\
	ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public:													\
	static ClassName* const& Get_Instance(void);		\
	static const unsigned long Destroy_Instance(void);	\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* const& ClassName::Get_Instance(void) {		\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
const unsigned long ClassName::Destroy_Instance(void) {	\
	if (nullptr == m_pInstance)							\
		return 0;										\
	unsigned long dwRefCnt = 0;							\
	dwRefCnt = m_pInstance->Release();					\
	if(0 == dwRefCnt)									\
		m_pInstance = nullptr;							\
	return dwRefCnt;									\
}