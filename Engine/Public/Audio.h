#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CAudio final : public CComponent{
public:
	static CAudio* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CAudio(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CAudio(const CAudio& rhs);
	virtual ~CAudio(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
public:
	void Set_Listener(class CTransform* const pTransform, const _float3 & vVelocity = { 0.f, 0.f ,0.f });
	void Set_Listener(class CTransform* const pCameraTransform, class CTransform* const pPlayerTransform, const _float3& vVelocity = { 0.f, 0.f ,0.f });
	void Set_3DAttribute(const _ubyte& byChannel, class CTransform* const pTransform, const _float3& vVelocity = { 0.f, 0.f ,0.f });
	void Set_3DAttribute(const _ubyte& byChannel, _fmatrix Matrix, const _float3& vVelocity = { 0.f, 0.f ,0.f });
	void Set_Volume(const _ubyte& byChannel, const _float& fVolume);
	void Set_Mode(const _ubyte& byChannel, const FMOD_MODE& iMode);
	void Set_MaxChannel(const _uint iMaxChannel);
	void Set_Distance(const _ubyte& byChannel, const _float& fMin, const _float& fMax);

	void Add_Sound(const _ubyte& byScene, const _tchar* const pSoundKey);

	void Play_Sound(const _tchar* const pSoundKey, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, class CTransform* const pTransform = nullptr, const _float3& vVelocity = { 0.f, 0.f ,0.f }, const _float2 & fDist = { 0.f, 0.f });
	void Play_Sound_Rand(const _tchar* const pSoundKey, const _uint & iRand, const _int & iChannel, const _float & fVolume, const FMOD_MODE & iMode, class CTransform* const pTransform = nullptr, const _float3 & vVelocity = { 0.f, 0.f ,0.f }, const _float2& fDist = { 0.f, 0.f });

	void Play_Sound2(const _tchar* const pSoundKey, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, _fmatrix matrix = XMMatrixIdentity(), const _float3& vVelocity = {0.f, 0.f ,0.f}, const _float2& fDist = {0.f, 0.f});
	void Play_Sound_Rand2(const _tchar* const pSoundKey, const _uint& iRand, const _int& iChannel, const _float& fVolume, const FMOD_MODE& iMode, _fmatrix matrix = XMMatrixIdentity(), const _float3& vVelocity = { 0.f, 0.f ,0.f }, const _float2& fDist = { 0.f, 0.f });

	void Stop_Sound(const _ubyte& byChannel);
	void Stop_Sound_All(void);
private:
	FMOD_SOUND* const Find_Sound(const _tchar* const& pSoundKey) const;
private:
	class CSound* m_pSound = nullptr;
	FMOD_SYSTEM* m_pSystem = nullptr;
	map<wstring, FMOD_SOUND*> m_mapSound;

	_uint m_iChannel = 10;
	_uint m_iMaxChannel = 10;
	FMOD_CHANNEL* m_pChannel[FMOD_MAX_CHANNEL_WIDTH] = { nullptr };
};
END_NAMESPACE