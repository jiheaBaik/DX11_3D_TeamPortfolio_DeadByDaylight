#include "Game_Instance.h"
#include "Timer_Mgr.h"
#include "Scene_Mgr.h"
#include "Font_Mgr.h"
#include "Frustum.h"
#include "Picking.h"

#include "UI.h"

IMPLEMENT_SINGLETON(CGame_Instance)

CGame_Instance::CGame_Instance(void)
	: CBase(),
	m_pGraphic(CGraphic::Get_Instance()),
	m_pInput(CInput::Get_Instance()),
	m_pNetwork(CNetwork::Get_Instance()),
	m_pSound(CSound::Get_Instance()),
	m_pTimer_Mgr(CTimer_Mgr::Get_Instance()),
	m_pTarget_Mgr(CTarget_Mgr::Get_Instance()),
	m_pCompute(CCompute::Get_Instance()),
	m_pFragment(CFragment::Get_Instance()),
	m_pScene_Mgr(CScene_Mgr::Get_Instance()),
	m_pComponent_Mgr(CComponent_Mgr::Get_Instance()),
	m_pObject_Mgr(CObject_Mgr::Get_Instance()),
	m_pEdit_Mgr(CEdit_Mgr::Get_Instance()),
	m_pFont_Mgr(CFont_Mgr::Get_Instance()),
	m_pFrustum(CFrustum::Get_Instance()),
	m_pPicking(CPicking::Get_Instance()) {
	Safe_AddRef(m_pGraphic);
	Safe_AddRef(m_pInput);
	Safe_AddRef(m_pNetwork);
	Safe_AddRef(m_pSound);
	Safe_AddRef(m_pTimer_Mgr);
	Safe_AddRef(m_pTarget_Mgr);
	Safe_AddRef(m_pCompute);
	Safe_AddRef(m_pFragment);
	Safe_AddRef(m_pScene_Mgr);
	Safe_AddRef(m_pComponent_Mgr);
	Safe_AddRef(m_pObject_Mgr);
	Safe_AddRef(m_pEdit_Mgr);
	Safe_AddRef(m_pFont_Mgr);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pPicking);
}

void CGame_Instance::Delete(void) {
	Safe_Release(m_pEdit_Mgr);
	Safe_Release(m_pPicking);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Mgr);
	Safe_Release(m_pObject_Mgr);
	Safe_Release(m_pComponent_Mgr);
	Safe_Release(m_pScene_Mgr);
	Safe_Release(m_pFragment);
	Safe_Release(m_pCompute);
	Safe_Release(m_pTarget_Mgr);
	Safe_Release(m_pTimer_Mgr);
	Safe_Release(m_pSound);
	Safe_Release(m_pNetwork);
	Safe_Release(m_pInput);
	Safe_Release(m_pGraphic);
}

void CGame_Instance::Destroy(void) {
	CGame_Instance::Destroy_Instance();
	CPicking::Destroy_Instance();
	CFrustum::Destroy_Instance();
	CFont_Mgr::Destroy_Instance();
	CEdit_Mgr::Destroy_Instance();
	CObject_Mgr::Destroy_Instance();
	CComponent_Mgr::Destroy_Instance();
	CScene_Mgr::Destroy_Instance();
	CTarget_Mgr::Destroy_Instance();
	CCompute::Destroy_Instance();
	CFragment::Destroy_Instance();
	CTimer_Mgr::Destroy_Instance();
	CSound::Destroy_Instance();
	CNetwork::Destroy_Instance();
	CInput::Destroy_Instance();
	CGraphic::Destroy_Instance();
}

const HRESULT CGame_Instance::Init(const HINSTANCE& hInst, const HWND& hWnd, const CGraphic::SGraphic& sGraphic, const _ubyte& byScene, const CEdit_Mgr::EMode& eEditMode) const {
	if (nullptr == hInst || nullptr == hWnd ||
		nullptr == m_pGraphic || nullptr == m_pInput || nullptr == m_pSound ||
		nullptr == m_pComponent_Mgr || nullptr == m_pObject_Mgr ||
		nullptr == m_pEdit_Mgr)
		return E_FAIL;
	HRESULT hr = S_OK;

	srand(static_cast<_uint>(time(NULL)));

	hr |= m_pGraphic->Init(hWnd, sGraphic);
	hr |= m_pInput->Init(hInst, hWnd);
	hr |= m_pNetwork->Init();
	hr |= m_pSound->Init(byScene);
	hr |= m_pTarget_Mgr->Init(m_pGraphic->Get_Device(), m_pGraphic->Get_Context());
	hr |= m_pCompute->Init(m_pGraphic->Get_Device(), m_pGraphic->Get_Context(), sGraphic.iWidth, sGraphic.iHeight);
	hr |= m_pFragment->Init(m_pGraphic->Get_Device(), m_pGraphic->Get_Context());
	hr |= m_pComponent_Mgr->Init(byScene);
	hr |= m_pObject_Mgr->Init(byScene);
	hr |= m_pPicking->Init(hWnd, m_pGraphic->Get_Context());
	hr |= m_pFont_Mgr->Init(m_pGraphic->Get_Device(), m_pGraphic->Get_Context());
	hr |= m_pFrustum->Init();
	hr |= m_pEdit_Mgr->Init(hWnd, m_pGraphic->Get_Device(), m_pGraphic->Get_Context(), eEditMode);

	CUI::Init(static_cast<_float>(sGraphic.iWidth), static_cast<_float>(sGraphic.iHeight));

	return S_OK;
}

void CGame_Instance::Update(const _double& dTimeDelta) const {
	if (nullptr == m_pInput ||
		nullptr == m_pSound ||
		nullptr == m_pScene_Mgr ||
		nullptr == m_pObject_Mgr)
		return;

	m_pInput->Update();
	m_pNetwork->Update();
	m_pSound->Update();
	m_pEdit_Mgr->Update();

	m_pScene_Mgr->Update_Scene(dTimeDelta);
	m_pObject_Mgr->Update_Layer(dTimeDelta);

	m_pFrustum->Transform_ToWorldSpace();

	m_pScene_Mgr->Late_Update_Scene(dTimeDelta);
	m_pObject_Mgr->Late_Update_Layer(dTimeDelta);

}

const HRESULT CGame_Instance::Render(void) const {
	if (FAILED(m_pEdit_Mgr->Render()))
		return E_FAIL;
	return S_OK;
}

ID3D11Device* const CGame_Instance::Get_Graphic_Device(void) const {
	if (nullptr == m_pGraphic)
		return nullptr;
	return m_pGraphic->Get_Device();
}

ID3D11DeviceContext* const CGame_Instance::Get_Graphic_Context(void) const {
	if (nullptr == m_pGraphic)
		return nullptr;
	return m_pGraphic->Get_Context();
}

const HRESULT CGame_Instance::Set_RenderTarget(void) const {
	if (nullptr == m_pGraphic)
		return E_FAIL;
	return m_pGraphic->Set_RenderTarget();
}

const HRESULT CGame_Instance::Clear_Graphic_RenderTargetView(const _float4& fColorRGBA) const {
	if (nullptr == m_pGraphic)
		return E_FAIL;
	return m_pGraphic->Clear_RenderTargetView(fColorRGBA);
}

const HRESULT CGame_Instance::Clear_Graphic_DepthStencilView(void) const {
	if (nullptr == m_pGraphic)
		return E_FAIL;
	return m_pGraphic->Clear_DepthStencilView();
}

const HRESULT CGame_Instance::Present_Graphic(void) {
	if (nullptr == m_pGraphic)
		return E_FAIL;
	return m_pGraphic->Present();
}

const _long CGame_Instance::Get_Input_MouseMove(const CInput::EMouseMove& eMouseMove) const {
	if (nullptr == m_pInput)
		return 0;
	return m_pInput->Get_MouseMove(eMouseMove);
}

const _byte CGame_Instance::Get_Input_MouseButton(const CInput::EMouseButton& eMouseButton) const {
	if (nullptr == m_pInput)
		return 0;
	return m_pInput->Get_MouseButton(eMouseButton);
}

const _bool CGame_Instance::Get_Input_MouseButtonUp(const CInput::EMouseButton& eMouseButton) const {
	if (nullptr == m_pInput)
		return false;
	return m_pInput->Get_MouseButtonUp(eMouseButton);
}

const _bool CGame_Instance::Get_Input_MouseButtonDown(const CInput::EMouseButton& eMouseButton) const {
	if (nullptr == m_pInput)
		return false;
	return m_pInput->Get_MouseButtonDown(eMouseButton);
}

const _byte CGame_Instance::Get_Input_KeyboardKey(const _ubyte& byKey) const {
	if (nullptr == m_pInput)
		return 0;
	return m_pInput->Get_KeyboardKey(byKey);
}

const _bool CGame_Instance::Get_Input_KeyboardKeyUp(const _ubyte& byKey) const {
	if (nullptr == m_pInput)
		return false;
	return m_pInput->Get_KeyboardKeyUp(byKey);
}

const _bool CGame_Instance::Get_Input_KeyboardKeyDown(const _ubyte& byKey) const {
	if (nullptr == m_pInput)
		return false;
	return m_pInput->Get_KeyboardKeyDown(byKey);
}

CNetwork* const CGame_Instance::Get_Network(void) const {
	if (nullptr == m_pNetwork)
		return nullptr;
	return m_pNetwork;
}

const HRESULT CGame_Instance::Load_Sound(const _byte& byScene, const _tchar* const pFilePath) {
	if (nullptr == m_pSound)
		return E_FAIL;
	return m_pSound->Load_Sound(byScene, pFilePath);
}

//void CGame_Instance::Play_Sound(const TCHAR* pSoundKey, const CSound::EChannel& eChannel, const _float& fVolume, _bool b3DSoundMode = false) {
//	if (nullptr == m_pSound)
//		return;
//	m_pSound->Play(pSoundKey, eChannel, fVolume, b3DSoundMode);
//}
//
//void CGame_Instance::Play_Sound(const TCHAR* pSoundKey, const _float& fVolume, _bool b3DSoundMode = false) {
//	if (nullptr == m_pSound)
//		return;
//	m_pSound->Play(pSoundKey, fVolume, b3DSoundMode);
//}
//
//void CGame_Instance::Play_Sound_NoStop(const TCHAR* pSoundKey, const CSound::EChannel& eChannel, const _float& fVolume, _bool b3DSoundMode = false) {
//	if (nullptr == m_pSound)
//		return;
//	m_pSound->Play_NoStop(pSoundKey, eChannel, fVolume, b3DSoundMode);
//}
//
//void CGame_Instance::Set_Sound_Volume(const CSound::EChannel& eChannel, const _float& fVolume) {
//	if (nullptr == m_pSound)
//		return;
//	m_pSound->Set_Volume(eChannel, fVolume);
//}
//
//void CGame_Instance::Stop_Sound(const CSound::EChannel& eChannel) {
//	if (nullptr == m_pSound)
//		return;
//	m_pSound->Stop(eChannel);
//}
//
//void CGame_Instance::Stop_Sound_All(void) {
//	if (nullptr == m_pSound)
//		return;
//	m_pSound->Stop_All();
//}

void CGame_Instance::Update_Timer(const _tchar* const& pTimerKey) {
	if (nullptr == m_pTimer_Mgr)
		return;
	m_pTimer_Mgr->Update_Timer(pTimerKey);
}

const _double CGame_Instance::Get_Timer_TimeDelta(const _tchar* const& pTimerKey) const {
	if (nullptr == m_pTimer_Mgr)
		return 0.;
	return m_pTimer_Mgr->Get_Timer_TimeDelta(pTimerKey);
}

const HRESULT CGame_Instance::Add_Timer(const _tchar* const& pTimerKey) const {
	if (nullptr == m_pTimer_Mgr)
		return E_FAIL;
	return m_pTimer_Mgr->Add_Timer(pTimerKey);
}

const _ubyte CGame_Instance::Get_LoadingScene(void) const {
	if (nullptr == m_pScene_Mgr)
		return 0;
	return m_pScene_Mgr->Get_LoadingScene();
}

void CGame_Instance::Set_LoadingScene(const _ubyte& byLoadingScene) {
	if (nullptr == m_pScene_Mgr)
		return;
	m_pScene_Mgr->Set_LoadingScene(byLoadingScene);
}

const _ubyte CGame_Instance::Get_Scene(void) const {
	if (nullptr == m_pScene_Mgr)
		return 0;
	return m_pScene_Mgr->Get_Scene();
}

CScene* const CGame_Instance::Get_Scene2(void) {
	if (nullptr == m_pScene_Mgr)
		return nullptr;
	return m_pScene_Mgr->Get_Scene2();
}

const HRESULT CGame_Instance::Change_Scene(const _ubyte& byScene, CScene* const& pScene) const {
	if (nullptr == m_pScene_Mgr || nullptr == pScene)
		return E_FAIL;
	return m_pScene_Mgr->Change_Scene(byScene, pScene);
}

const HRESULT CGame_Instance::Clear_Scene(void) const {
	if (nullptr == m_pScene_Mgr)
		return E_FAIL;
	return m_pScene_Mgr->Clear_Scene();
}

map<wstring, class CComponent*>* CGame_Instance::Get_Component_Prototype(void) {
	if (nullptr == m_pComponent_Mgr)
		return nullptr;
	return &m_pComponent_Mgr->Get_Prototype();
}

CComponent* CGame_Instance::Get_Component_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey) const {
	if (nullptr == m_pComponent_Mgr)
		return nullptr;
	return m_pComponent_Mgr->Get_Prototype(byScene, pPrototypeKey);
}

const HRESULT CGame_Instance::Add_Component_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey, CComponent* const& pComponent) const {
	if (nullptr == m_pComponent_Mgr || nullptr == pComponent)
		return E_FAIL;
	return m_pComponent_Mgr->Add_Prototype(byScene, pPrototypeKey, pComponent);
}

CComponent* CGame_Instance::CreateGet_Component_Clone(const _ubyte& byScene, const _tchar* const& pPrototypeKey, void* const& pArg) const {
	if (nullptr == m_pComponent_Mgr)
		return nullptr;
	return m_pComponent_Mgr->CreateGet_Clone(byScene, pPrototypeKey, pArg);
}

void CGame_Instance::Clear_Component_Prototype(const _ubyte& byScene) const {
	if (nullptr == m_pComponent_Mgr)
		return;
	m_pComponent_Mgr->Clear_Prototype(byScene);
}

const HRESULT CGame_Instance::Add_Object_Prototype(const _tchar* const& pPrototypeKey, CObject* const& pObject) {
	if (nullptr == m_pObject_Mgr || nullptr == pObject)
		return E_FAIL;
	return m_pObject_Mgr->Add_Prototype(pPrototypeKey, pObject);
}


map<wstring, class CObject*>* CGame_Instance::Get_Object_MapPrototype(void) const {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return &m_pObject_Mgr->Get_MapPrototype();
}

const HRESULT CGame_Instance::Create_Object_Clone(const _tchar* const& pPrototypeKey, const _ubyte& byScene, const _tchar* const& pLayerKey, void* const& pArg) {
	if (nullptr == m_pObject_Mgr)
		return E_FAIL;
	return m_pObject_Mgr->Create_Clone(pPrototypeKey, byScene, pLayerKey, pArg);
}

CObject* const CGame_Instance::CreateGet_Object_Clone(const _tchar* const& pPrototypeKey, const _ubyte& byScene, const _tchar* const& pLayerKey, void* const& pArg) {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return m_pObject_Mgr->CreateGet_Clone(pPrototypeKey, byScene, pLayerKey, pArg);
}

map<wstring, class CLayer*>* const CGame_Instance::Get_Object_LayerClone(const _ubyte& byScene) const {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return m_pObject_Mgr->Get_LayerClone(byScene);
}

list<class CObject*>* const CGame_Instance::Get_Object_ListClone(const _ubyte& byScene, const _tchar* const& pLayerKey) const {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return m_pObject_Mgr->Get_ListClone(byScene, pLayerKey);
}

CObject* const CGame_Instance::Get_Object_Clone(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iIndex) const {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return m_pObject_Mgr->Get_Clone(byScene, pLayerKey, iIndex);
}

CObject* const CGame_Instance::Get_Object_CloneID(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iID) const {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return m_pObject_Mgr->Get_CloneID(byScene, pLayerKey, iID);
}

CObject* const CGame_Instance::Get_Object_CloneControl(const _ubyte& byScene, const _tchar* const& pLayerKey) const {
	if (nullptr == m_pObject_Mgr)
		return nullptr;
	return m_pObject_Mgr->Get_CloneControl(byScene, pLayerKey);
};

const HRESULT CGame_Instance::Clear_Object_Clone(const _ubyte& byScene) const {
	if (nullptr == m_pObject_Mgr)
		return E_FAIL;
	return m_pObject_Mgr->Clear_Clone(byScene);
}

const HRESULT CGame_Instance::Delete_Object_Clone(CObject* const& pObject) const {
	if (nullptr == m_pObject_Mgr)
		return E_FAIL;
	return m_pObject_Mgr->Delete_Clone(pObject);
}

const CEdit_Mgr::EMode CGame_Instance::Get_Edit_Mode(void) const {
	if (nullptr == m_pEdit_Mgr)
		return CEdit_Mgr::EMode::END;
	return m_pEdit_Mgr->Get_Mode();
}

void CGame_Instance::Set_Edit_Mode(const CEdit_Mgr::EMode& eMode) const {
	if (nullptr == m_pEdit_Mgr)
		return;
	m_pEdit_Mgr->Set_Mode(eMode);
}

const HRESULT CGame_Instance::Add_Edit(const _tchar* const& pEditKey, CEdit* const& pEdit) {
	if (nullptr == m_pEdit_Mgr)
		return E_FAIL;
	return m_pEdit_Mgr->Add_Edit(pEditKey, pEdit);
}

CEdit* const CGame_Instance::Get_Edit(const _tchar* const& pEditKey) {
	if (nullptr == m_pEdit_Mgr)
		return nullptr;
	return m_pEdit_Mgr->Get_Edit(pEditKey);
}

const HRESULT CGame_Instance::Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fScale, const _float& fRotation) {
	if (nullptr == m_pFont_Mgr)
		return E_FAIL;
	return m_pFont_Mgr->Render_Font(pFontKey, pText, vPos, vColor, fScale, fRotation);
}

const HRESULT CGame_Instance::Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fBorder, _fvector vBorderColor, const _float& fRotation) {
	if (nullptr == m_pFont_Mgr)
		return E_FAIL;
	return m_pFont_Mgr->Render_Font(pFontKey, pText, vPos, vColor, fBorder, vBorderColor, fRotation);
}

const HRESULT CGame_Instance::Add_Font(const _tchar* const& pFontKey, const _tchar* const& pFilePath) {
	if (nullptr == m_pFont_Mgr)
		return E_FAIL;
	return m_pFont_Mgr->Add_Font(pFontKey, pFilePath);
}

const _bool CGame_Instance::IsIn_Frustum_World(_fvector vPoint, const _float& fRange) {
	if (nullptr == m_pFrustum)
		return false;
	return m_pFrustum->IsIn_Frustum_World(vPoint, fRange);
}

const _bool CGame_Instance::Is_Picking(CObject* const& pObject, CCamera* const& pCamera, _float* const& pOut) {
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Is_Picking(pCamera, pObject, pOut);
}

const _bool CGame_Instance::Is_Picking(CCamera* const& pCamera, CObject* const& pObject, _float3* const& pOut) {
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Is_Picking(pCamera, pObject, pOut);
}

const _bool CGame_Instance::Is_Picking(CCamera* const& pCamera, CBuffer_Terrain* const& pBuffer, _float3* const& pOut) {
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Is_Picking(pCamera, pBuffer, pOut);
}

const _bool CGame_Instance::Is_Picking(CCamera* const& pCamera, CNavi* const& pNavi, _float3* const& pOut) {
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Is_Picking(pCamera, pNavi, pOut);
}

const _bool CGame_Instance::Is_Picking(CCamera* const& pCamera, CCell* const& pCell, _float3* const& pOut) {
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Is_Picking(pCamera, pCell, pOut);
}

const _bool CGame_Instance::Is_Picking(CCamera* const& pCamera, CTransform* const& pTransform, CBuffer* pBuffer, _float3* const& pOut) {
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Is_Picking(pCamera, pTransform, pBuffer, pOut);
}
