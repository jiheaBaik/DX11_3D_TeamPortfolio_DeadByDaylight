#include "pch.h"
#include "Game_Main.h"
#include "Loading.h"

#include "Dock.h"
#include "Asset.h"
#include "Inspector.h"
#include "Hierarchy.h"
#include "NavMesh.h"
#include "View.h"
#include "Post.h"

#include "Camera_Edit.h"
#include "Light_Free.h"
#include "Light_Free_NoSave.h"
#include "Shadow.h"
#include "Terrain.h"
#include "Sky.h"
#include "Animator.h"
#include "Effect_Edit.h"
#include "Effect_Texture.h"
#include "Effect_Model.h"
#include "Reflect.h"
#include "Camera_Reflect.h"
#include "Setting_BGM.h"

#include "UI_Loading_Back.h"
#include "UI_Loading_Bone.h"
#include "UI_Loading_Bone2.h"
#include "UI_Loading_Icon_Circle.h"
#include "UI_Loading_ToPlay_Slasher.h"
#include "UI_Loading_ToPlay_Camper.h"

#include "Cube.h"
#include "Distor.h"
#include "Trail.h"
#include "Metal.h"
#include "GTest.h"
#include "Collid.h"

CGame_Main* const CGame_Main::Create(void) {
	CGame_Main* pInstnace = new CGame_Main;
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CGame_Main::CGame_Main(void)
	: CBase(),
	m_pGame_Instance(CGame_Instance::Get_Instance()) {
	Safe_AddRef(m_pGame_Instance);
}

void CGame_Main::Delete(void) {
	Safe_Release(m_pInfo);
	CInfo::Destroy_Instance();
	CInven::Destroy_Instance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pNetwork);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGame_Instance);
}

const HRESULT CGame_Main::Init(void) {
	HRESULT hr = S_OK;

	m_pDevice = m_pGame_Instance->Get_Graphic_Device();
	Safe_AddRef(m_pDevice);
	m_pContext = m_pGame_Instance->Get_Graphic_Context();
	Safe_AddRef(m_pContext);
	m_pNetwork = m_pGame_Instance->Get_Network();
	Safe_AddRef(m_pNetwork);

	//Component
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), CTransform::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform"), CRectTransform::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::EType::AABB));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::EType::OBB));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::EType::SPHERE));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), CBuffer_Rect::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Cube"), CBuffer_Cube::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Terrain"), CBuffer_Terrain::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Trail"), CBuffer_Trail::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), CRenderer::Create(m_pDevice, m_pContext));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Tex.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_TexCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/TexCube.hlsl"), VTXCUBETEX_DECLARATION::Input_Element_Desc, VTXCUBETEX_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_TexNor"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/TexNor.hlsl"), VTXNORTEX_DECLARATION::Input_Element_Desc, VTXNORTEX_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Model.hlsl"), VTXMODEL_DECLARATION::Input_Element_Desc, VTXMODEL_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelInst"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/ModelInst.hlsl"), VTXMODELINST_DECLARATION::Input_Element_Desc, VTXMODELINST_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/ModelAnim.hlsl"), VTXANIMMODEL_DECLARATION::Input_Element_Desc, VTXANIMMODEL_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_UI"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/UI.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Effect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Effect.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelEffect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/ModelEffect.hlsl"), VTXMODEL_DECLARATION::Input_Element_Desc, VTXMODEL_DECLARATION::iNumElement));
	hr |= m_pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), CAudio::Create(m_pDevice, m_pContext));

	//Texture + Model + Sound
	hr |= Load_Texture(m_pDevice, m_pContext, EScene::STATIC, TEXT("../Bin/Resource/00.Static/Texture/*.*"));
	hr |= Load_Model(m_pDevice, m_pContext, EScene::STATIC, TEXT("../Bin/Resource/00.Static/Model/*.*"));
	hr |= m_pGame_Instance->Load_Sound(static_cast<_ubyte>(EScene::STATIC), TEXT("../Bin/Resource/00.Static/Sound/*.*"));

	//Edit
	if (CEdit_Mgr::EMode::PLAY != m_pGame_Instance->Get_Edit_Mode() ){
		hr |= m_pGame_Instance->Add_Edit(TEXT("Debug"), CDebug::Create(m_pDevice, m_pContext));
			hr |= m_pGame_Instance->Add_Edit(TEXT("Asset"), CAsset::Create(m_pDevice, m_pContext));
			hr |= m_pGame_Instance->Add_Edit(TEXT("Inspector"), CInspector::Create(m_pDevice, m_pContext));
			hr |= m_pGame_Instance->Add_Edit(TEXT("Hierarchy"), CHierarchy::Create(m_pDevice, m_pContext));
			hr |= m_pGame_Instance->Add_Edit(TEXT("View"), CView::Create(m_pDevice, m_pContext));
			hr |= m_pGame_Instance->Add_Edit(TEXT("Post"), CPost::Create(m_pDevice, m_pContext));
			hr |= m_pGame_Instance->Add_Edit(TEXT("NavMesh"), CNavMesh::Create(m_pDevice, m_pContext));
	}

	//Object
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Camera_Edit"), CCamera_Edit::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Light_Free"), CLight_Free::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Light_Free_NoSave"), CLight_Free_NoSave::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Shadow"), CShadow::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Terrain"), CTerrain::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Sky"), CSky::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Animator"), CAnimator::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Effect_Edit"), CEffect_Edit::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Effect_Texture"), CEffect_Texture::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Effect_Model"), CEffect_Model::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Reflect"), CReflect::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Camera_Reflect"), CCamera_Reflect::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Setting_BGM"), CSetting_BGM::Create(m_pDevice, m_pContext, m_pNetwork));
	
	//Loading
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Loading_Back"), CUI_Loading_Back::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Loading_Bone"), CUI_Loading_Bone::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Loading_Bone2"), CUI_Loading_Bone2::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Loading_Icon_Circle"), CUI_Loading_Icon_Circle::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("UI_Loading_ToPlay_Slasher"), CUI_Loading_ToPlay_Slasher::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("UI_Loading_ToPlay_Camper"), CUI_Loading_ToPlay_Camper::Create(m_pDevice, m_pContext, m_pNetwork));

	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Cube"), CCube::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Distor"), CDistor::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Trail"), CTrail::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Metal"), CMetal::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("GTest"), CGTest::Create(m_pDevice, m_pContext, m_pNetwork));
	hr |= m_pGame_Instance->Add_Object_Prototype(TEXT("Collid"), CCollid::Create(m_pDevice, m_pContext, m_pNetwork));
	
	// Add Font
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic8"), TEXT("../Bin/Resource/Font/Gothic8.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic10"), TEXT("../Bin/Resource/Font/Gothic10.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic12"), TEXT("../Bin/Resource/Font/Gothic12.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic14"), TEXT("../Bin/Resource/Font/Gothic14.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic16"), TEXT("../Bin/Resource/Font/Gothic16.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic18"), TEXT("../Bin/Resource/Font/Gothic18.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic20"), TEXT("../Bin/Resource/Font/Gothic20.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic22"), TEXT("../Bin/Resource/Font/Gothic22.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic24"), TEXT("../Bin/Resource/Font/Gothic24.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic26"), TEXT("../Bin/Resource/Font/Gothic26.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic28"), TEXT("../Bin/Resource/Font/Gothic28.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic30"), TEXT("../Bin/Resource/Font/Gothic30.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("Gothic32"), TEXT("../Bin/Resource/Font/Gothic32.spritefont"));
	hr |= m_pGame_Instance->Add_Font(TEXT("GothicSemilight32"), TEXT("../Bin/Resource/Font/GothicSemilight32.spritefont"));

	m_pRenderer = dynamic_cast<CRenderer*>(m_pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer")));
	m_pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, m_pNetwork, EScene::LOGO));

	m_pInfo = CInfo::Get_Instance();
	Safe_AddRef(m_pInfo);

	g_bChangeScene = false;
	return hr;
}

void CGame_Main::Update(const _double& dTimeDelta) {
	m_pGame_Instance->Update(dTimeDelta);
	m_pInfo->Update();
#ifdef _DEBUG
	m_dAccTime += dTimeDelta;
#endif // _DEBUG
}

const HRESULT CGame_Main::Render(void) {
	HRESULT hr = S_OK;
	hr |= m_pGame_Instance->Set_RenderTarget();
	hr |= m_pGame_Instance->Clear_Graphic_RenderTargetView(_float4{ 0.f, 0.f, 1.f, 1.f });
	hr |= m_pGame_Instance->Clear_Graphic_DepthStencilView();
	hr |= m_pRenderer->Render();
#ifdef _DEBUG
	++m_iFPS;
	if (1.0 <= m_dAccTime) {
		wsprintf(m_szFPS, TEXT("%d FPS"), m_iFPS);
		m_iFPS = 0;
		m_dAccTime = 0.0;
	}
	m_pGame_Instance->Render_Font(TEXT("Gothic10"), m_szFPS, _float2(5.f, 5.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
#endif // _DEBUG
	hr |= m_pGame_Instance->Render();
	hr |= m_pGame_Instance->Present_Graphic();

	return hr;
}
