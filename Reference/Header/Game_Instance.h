#pragma once
#include "Graphic.h"
#include "Input.h"
#include "Network.h"
#include "Sound.h"
#include "Target_Mgr.h"
#include "Compute.h"
#include "Fragment.h"
#include "Component_Mgr.h"
#include "Object_Mgr.h"
#include "Edit_Mgr.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CGame_Instance final : public CBase{
	DECLARE_SINGLETON(CGame_Instance)
private:
	explicit CGame_Instance(void);
	virtual ~CGame_Instance(void) override = default;
	virtual void Delete(void) override;
public:
	static void Destroy(void);
public:
	const HRESULT Init(const HINSTANCE & hInst, const HWND & hWnd, const CGraphic::SGraphic & sGraphic, const _ubyte & byScene, const CEdit_Mgr::EMode & eEditMode) const;
	void Update(const _double & dTimeDelta) const;
	const HRESULT Render(void) const;
public: /* Graphic */
	ID3D11Device* const Get_Graphic_Device(void) const;
	ID3D11DeviceContext* const Get_Graphic_Context(void) const;
	const HRESULT Set_RenderTarget(void) const;
	const HRESULT Clear_Graphic_RenderTargetView(const _float4 & fColorRGBA) const;
	const HRESULT Clear_Graphic_DepthStencilView(void) const;
	const HRESULT Present_Graphic(void);
public: /* Input */
	const _long Get_Input_MouseMove(const CInput::EMouseMove & eMouseMove) const;
	const _byte Get_Input_MouseButton(const CInput::EMouseButton & eMouseButton) const;
	const _bool Get_Input_MouseButtonUp(const CInput::EMouseButton & eMouseButton) const;
	const _bool Get_Input_MouseButtonDown(const CInput::EMouseButton & eMouseButton) const;
	const _byte Get_Input_KeyboardKey(const _ubyte & byKey) const;
	const _bool Get_Input_KeyboardKeyUp(const _ubyte & byKey) const;
	const _bool Get_Input_KeyboardKeyDown(const _ubyte & byKey) const;
public: /* Network */
	class CNetwork* const Get_Network(void) const;
public: /* Sound */
	const HRESULT Load_Sound(const _byte& byScene, const _tchar* const pFilePath);
	//void Play_Sound(const TCHAR * pSoundKey, const CSound::EChannel & eChannel, const _float & fVolume, _bool b3DSoundMod);
	//void Play_Sound(const TCHAR * pSoundKey, const _float & fVolume, _bool b3DSoundMode);
	//void Play_Sound_NoStop(const TCHAR * pSoundKey, const CSound::EChannel & eChannel, const _float & fVolume, _bool b3DSoundMode);
	//void Set_Sound_Volume(const CSound::EChannel & eChannel, const _float & fVolume);
	//void Stop_Sound(const CSound::EChannel & eChannel);
	//void Stop_Sound_All(void);
private:
public: /* Timer */
	void Update_Timer(const _tchar* const& pTimerKey);
	const _double Get_Timer_TimeDelta(const _tchar* const& pTimerKey) const;
	const HRESULT Add_Timer(const _tchar* const& pTimerKey) const;
public: /* Scene */
	const _ubyte Get_LoadingScene(void) const;
	void Set_LoadingScene(const _ubyte & byLoadingScene);
	const _ubyte Get_Scene(void) const;
	class CScene* const Get_Scene2(void);
	const HRESULT Change_Scene(const _ubyte & byScene, class CScene* const& pScene) const;
	const HRESULT Clear_Scene(void) const;
public: /* Component */
	map<wstring, class CComponent*>* Get_Component_Prototype(void);
	class CComponent* Get_Component_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey)const;
	const HRESULT Add_Component_Prototype(const _ubyte & byScene, const _tchar* const& pPrototypeKey, class CComponent* const& pComponent) const;
	class CComponent* CreateGet_Component_Clone(const _ubyte & byScene, const _tchar* const& pPrototypeKey, void* const& pArg = nullptr) const;
	void Clear_Component_Prototype(const _ubyte & byScene) const;
public: /* Object */
	const HRESULT Add_Object_Prototype(const _tchar* const& pPrototypeKey, class CObject* const& pObject);
	map<wstring, class CObject*>* Get_Object_MapPrototype(void) const;
	const HRESULT Create_Object_Clone(const _tchar* const& pPrototypeKey, const _ubyte & byScene, const _tchar* const& pLayerKey, void* const& pArg = nullptr);
	class CObject* const CreateGet_Object_Clone(const _tchar* const& pPrototypeKey, const _ubyte & byScene, const _tchar* const& pLayerKey, void* const& pArg = nullptr);
	map<wstring, class CLayer*>* const Get_Object_LayerClone(const _ubyte & byScene) const;
	list<class CObject*>* const Get_Object_ListClone(const _ubyte & byScene, const _tchar* const& pLayerKey) const;
	class CObject* const Get_Object_Clone(const _ubyte & byScene, const _tchar* const& pLayerKey, const _uint & iIndex = 0) const;
	class CObject* const Get_Object_CloneID(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iID = 0) const;
	class CObject* const Get_Object_CloneControl(const _ubyte& byScene, const _tchar* const& pLayerKey) const;
	const HRESULT Clear_Object_Clone(const _ubyte & byScene) const;
	const HRESULT Delete_Object_Clone(class CObject* const& pObject) const;
public: /* Edit */
	const CEdit_Mgr::EMode Get_Edit_Mode(void) const;
	void Set_Edit_Mode(const CEdit_Mgr::EMode & eMode) const;
	const HRESULT Add_Edit(const _tchar* const& pEditKey, class CEdit* const& pEdit);
	class CEdit* const Get_Edit(const _tchar* const& pEditKey);
public: /* Font */
	const HRESULT Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2 & vPos, _fvector vColor, const _float& fScale = 1.f, const _float & fRotation = 0.f);
	const HRESULT Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2 & vPos, _fvector vColor, const _float & fBorder, _fvector vBorderColor, const _float & fRotation = 0.f);
	const HRESULT Add_Font(const _tchar* const& pFontKey, const _tchar* const& pFilePath);
public: /* Frustum */
	const _bool IsIn_Frustum_World(_fvector vPoint, const _float & fRange);
public: /* Picking */
	const _bool Is_Picking(class CObject* const& pObject, class CCamera* const& pCamera, _float* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CObject* const& pObject, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CBuffer_Terrain* const& pBuffer, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CNavi* const& pNavi, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CCell* const& pCell, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CTransform* const& pTransform, class CBuffer* pBuffer, _float3* const& pOut = nullptr);
private:
	class CGraphic* m_pGraphic = nullptr;
	class CInput* m_pInput = nullptr;
	class CNetwork* m_pNetwork = nullptr;
	class CSound* m_pSound = nullptr;
	class CTimer_Mgr* m_pTimer_Mgr = nullptr;
private:
	class CTarget_Mgr* m_pTarget_Mgr = nullptr;
	class CCompute* m_pCompute = nullptr;
	class CFragment* m_pFragment = nullptr;
private:
	class CScene_Mgr* m_pScene_Mgr = nullptr;
	class CComponent_Mgr* m_pComponent_Mgr = nullptr;
	class CObject_Mgr* m_pObject_Mgr = nullptr;
private:
	class CEdit_Mgr* m_pEdit_Mgr = nullptr;
private:
	class CFont_Mgr* m_pFont_Mgr = nullptr;
	class CFrustum* m_pFrustum = nullptr;
	class CPicking* m_pPicking = nullptr;
};
END_NAMESPACE