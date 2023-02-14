#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CSound final : public CBase {
	DECLARE_SINGLETON(CSound)
public:
	enum class EChannel : _ubyte { BGM, CH0, CH1, CH2, CH3, END };
private:
	explicit CSound(void);
	virtual ~CSound(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const _ubyte& byScene);
	void Update(void);
public:
	const HRESULT Load_Sound(const _ubyte& byScene, const _tchar* const& pFilePath);
public:
	FMOD_SYSTEM* const Get_System(void);
	FMOD_SOUND* const Get_Sound(const _ubyte& byScene, const _tchar* const& pSoundKey);
	const _byte Get_Scene(void);

	const HRESULT Clear_Sound(const _ubyte& byScene) const;
protected:
	FMOD_SOUND* const Find_Sound(const _ubyte& byScene, const _tchar* const& pSoundKey) const;
private:
	FMOD_SYSTEM* m_pSystem = nullptr;
	map<wstring, FMOD_SOUND*>* m_pMapSound;
private:
	_ubyte m_byScene = 0;

	FMOD_CHANNEL* m_pChannel[static_cast<_ubyte>(EChannel::END)];
	EChannel m_eChannel = EChannel::CH0;
};
END_NAMESPACE