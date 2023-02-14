#include "pch.h"
#include "Weapon_Spirit.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "Effect_Blood_Free.h"
#include "Effect_Free3.h"
#include "Shadow.h"
#include "Slasher_Spirit_FPV_Trail.h"
#include "Slasher_Spirit_Trail.h"

CWeapon_Spirit* const CWeapon_Spirit::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CWeapon_Spirit* pInstance = new CWeapon_Spirit(pDevice, pContext, pNetwork);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CObject* const CWeapon_Spirit::Clone(void* const& pArg) {
	CWeapon_Spirit* pInstance = new CWeapon_Spirit(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CWeapon_Spirit::CWeapon_Spirit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CWeapon_Spirit::CWeapon_Spirit(const CWeapon_Spirit& rhs)
	: CObject(rhs) {
}

void CWeapon_Spirit::Delete(void) {
	__super::Delete();
	Safe_Release(m_pSlasher_FPV_Trail);
	Safe_Release(m_pSlasher_Trail);

	Safe_Release(m_pTransform);
	Safe_Release(m_pTransform_FPV);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pModel_FPV);
	Safe_Release(m_pCollider);
}

const HRESULT CWeapon_Spirit::Init_Create(void) {
	return S_OK;
}

const HRESULT CWeapon_Spirit::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_FPV"), reinterpret_cast<CComponent*&>(m_pTransform_FPV));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Weapon_Spirit"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Weapon_Spirit"), TEXT("Model_FPV"), reinterpret_cast<CComponent*&>(m_pModel_FPV));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));

	//m_pCollider->Set_Extents(_float3{ 0.18f, 0.17f, 0.045f });
	m_pCollider->Set_Center(_float3{ -0.06f, 0.21f, 0.f });
	m_pCollider->Set_Extents(_float3{ 0.6f, 1.0f, 0.6f });

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pSlasher_Huntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	m_pSlasher_Transform = dynamic_cast<CTransform*>(m_pSlasher_Huntress->Get_Component(TEXT("Transform")));
	m_pSlasher_Audio = dynamic_cast<CAudio*>(m_pSlasher_Huntress->Get_Component(TEXT("Audio")));

	m_pSlasher_FPV_Trail = dynamic_cast<CSlasher_Spirit_FPV_Trail*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Slasher_Spirit_FPV_Trail"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Spirit_FPV_Trail")));
	m_pSlasher_Trail = dynamic_cast<CSlasher_Spirit_Trail*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Slasher_Spirit_Trail"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Spirit_Trail")));

	return hr;
}

const _ubyte CWeapon_Spirit::Update(const _double& dTimeDelta) {
	if (nullptr != m_pSlasher_Huntress && true == m_pSlasher_Huntress->Get_Control()) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

		if (CFM_Slasher::EState::ATTACK_SLASH_SWING == m_pSlasher_Huntress->Get_State()) {
			//collsion
			list<class CObject*>* CamperList = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_Scene(), TEXT("Camper_GamePlay"));
			if (nullptr != CamperList) {
				for (_int i = 0; i < CamperList->size(); i++) {
					CCamper_GamePlay* pCamper_GamePlay = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), i));
					if (nullptr != pCamper_GamePlay) {
						CCollider* pCamper_GamePlayCollider = dynamic_cast<CCollider*>(pCamper_GamePlay->Get_Component(TEXT("Collider")));
						if (true == m_bAttack && pCamper_GamePlay->Get_FM_Camper()->Get_State() != CFM_Camper::CLOSSET_IDLE && pCamper_GamePlay->Get_FM_Camper()->Get_State() != CFM_Camper::ENDGAME
							&& true == m_pCollider->Is_Collision(pCamper_GamePlayCollider)) {
							m_iColID = pCamper_GamePlay->Get_Id();
							if (2 == pCamper_GamePlay->Get_Hp()) {
								CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper_GamePlay->Get_Component(TEXT("Transform")));

								_matrix MatrixInverse = pCamperTransform->Get_Matrix_Inverse();
								_float3 vLocal; XMStoreFloat3(&vLocal, XMVector3TransformCoord(m_pSlasher_Transform->Get_Row(CTransform::ERow::POS), MatrixInverse));

								if (vLocal.z > 0)
									m_iCamperHitDir = CFM_Camper::EState::HIT_F;
								else
									m_iCamperHitDir = CFM_Camper::EState::HIT_B;

								// blood effect
								CObject* pCamBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_CamBlood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamBlood"));
								m_pSlasher_Audio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
								m_pSlasher_Audio->Play_Sound_Rand(TEXT("sfx_hatchet_special_hit_blood"), 5, -1, 0.3f, FMOD_2D);

								Update_Network("Hit_Slash");
								m_bAttack = false;
							}
							else if (1 == pCamper_GamePlay->Get_Hp()) {
								CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper_GamePlay->Get_Component(TEXT("Transform")));

								_matrix MatrixInverse = pCamperTransform->Get_Matrix_Inverse();
								_float3 vLocal; XMStoreFloat3(&vLocal, XMVector3TransformCoord(m_pSlasher_Transform->Get_Row(CTransform::ERow::POS), MatrixInverse));
								if (abs(vLocal.x) > abs(vLocal.z)) {
									if (vLocal.x > 0) {
										m_iCamperHitDir = CFM_Camper::EState::HIT_CRAWL_R;
									}
									else {
										m_iCamperHitDir = CFM_Camper::EState::HIT_CRAWL_L;
									}
								}
								else {
									if (vLocal.z > 0)
										m_iCamperHitDir = CFM_Camper::EState::HIT_CRAWL_F;
									else
										m_iCamperHitDir = CFM_Camper::EState::HIT_CRAWL_B;
								}

								// blood effect
								CObject* pCamBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_CamBlood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamBlood"));
								m_pSlasher_Audio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
								m_pSlasher_Audio->Play_Sound_Rand(TEXT("sfx_hatchet_special_hit_blood"), 5, -1, 0.3f, FMOD_2D);

								Update_Network("Hit_Slash");
								m_bAttack = false;
							}
						}
					}
				}
			}
		}
	}

	if (m_bDissolveUpDown) {
		m_bRender = true;
		if (m_fDissolveLength < 2.f) {
			m_fDissolveLength += 4.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength > 2.f) {
				m_fDissolveLength = 2.f;
			}
		}
		else if (m_fDissolveLength > 2.f) {
			m_fDissolveLength -= 4.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength < 2.f) {
				m_fDissolveLength = 2.f;
			}
		}
	}
	else {
		if (m_fDissolveLength < -0.1f) {
			m_fDissolveLength += 4.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength > -0.1f) {
				m_fDissolveLength = -0.1f;
				m_bRender = false;
			}
		}
		else if (m_fDissolveLength > -0.1f) {
			m_fDissolveLength -= 4.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength < -0.1f) {
				m_fDissolveLength = -0.1f;
				m_bRender = false;
			}
		}
	}
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_G)) {
		m_bDissolveUpDown = !m_bDissolveUpDown;
	}

	m_pCollider->Update(m_pTransform->Get_Matrix());
	Update_Network();
	return OBJ_NOEVENT;
}

void CWeapon_Spirit::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(1);
	m_pTransform_FPV->Update_Edit(2);
}

void CWeapon_Spirit::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pSlasher_Huntress->Get_FM_Slasher()->Get_State() != CFM_Slasher::EState::ATTACK_SLASH_WIPE) {
		CModel* pModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));
		CModel* pModel_FPV = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model_FPV")));
		_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_WeaponRT_01");
		_matrix BoneMatrix_FPV = pModel_FPV->Get_BoneMatrix("joint_WeaponRT_01");

		_matrix SocketMatrix;
		_matrix SocketMatrix_FPV;

		SocketMatrix = BoneMatrix * m_pSlasher_Transform->Get_Matrix();
		SocketMatrix_FPV = BoneMatrix_FPV * m_pSlasher_Transform->Get_Matrix();

		m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
		m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
		m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
		m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

		m_pTransform_FPV->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix_FPV.r[0]);
		m_pTransform_FPV->Set_Row(CTransform::ERow::UP, SocketMatrix_FPV.r[1]);
		m_pTransform_FPV->Set_Row(CTransform::ERow::LOOK, -SocketMatrix_FPV.r[2]);
		m_pTransform_FPV->Set_Row(CTransform::ERow::POS, SocketMatrix_FPV.r[3]);
	}

	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::SHADOW, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CWeapon_Spirit::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		/*CTexture* pDissolveAlpha = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Dissolve_border_gradient_Blue")));
		pDissolveAlpha->Set_Resource(m_pShader, "g_DissolveAlpha", 0);
		CTexture* pDissolvTexture = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Dissolve_spiderthingBlurred")));
		pDissolvTexture->Set_Resource(m_pShader, "g_DissolveTexture", 0);
		m_pShader->Set_RawValue("g_DissolveLength", &m_fDissolveLength, sizeof(_float));*/
		CTexture* pDissolve = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_DissolveTexturesEntity")));
		CTexture* pAlpha = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Fxt_verticalGradient1")));
		pDissolve->Set_Resource(m_pShader, "g_DissolveTexture", 0);
		pAlpha->Set_Resource(m_pShader, "g_DissolveAlpha", 0);
		m_pShader->Set_RawValue("g_DissolveLength", &m_fDissolveLength, sizeof(_float));
		_float3 m_fDissolveColor = { 8.f,0.f,0.f };
		m_pShader->Set_RawValue("g_DissolveColor", &m_fDissolveColor, sizeof(_float3));

		if (true == m_pSlasher_Huntress->Get_Control()) {
			if (true == m_bRender) {
				m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform_FPV->Get_Float4x4_Transpose(), sizeof(_float4x4));
				for (_uint i = 0; i < m_pModel_FPV->Get_NumMesh(); ++i) {
					m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
					if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
						m_pModel->Render(i, m_pShader, 12);
					else
						m_pModel->Render(i, m_pShader, 12);
				}
			}
		}
		else {
			if (true == m_bRender) {
				m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
					m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
					if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
						m_pModel->Render(i, m_pShader, 0);
					else
						m_pModel->Render(i, m_pShader, 8);
				}
			}
		}
		break;
	}
										 break;
	case CRenderer::EOrder::SHADOW: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CShadow* pShadow = dynamic_cast<CShadow*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow")));
		pShadow->Set_RawValue(m_pShader);

		if (true == m_pSlasher_Huntress->Get_Control()) {
			m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform_FPV->Get_Float4x4_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel_FPV->Get_NumMesh(); ++i) {
				m_pModel_FPV->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel_FPV->Render(i, m_pShader, 3);
			}
		}
		else {
			m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
				m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel->Render(i, m_pShader, 3);
			}
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const _bool& CWeapon_Spirit::Get_Attack(void) {
	return m_bAttack;
}

void CWeapon_Spirit::Set_Attack(const _bool& bAttack) {
	m_bAttack = bAttack;
}

void CWeapon_Spirit::Render_On(void) {
	m_bDissolveUpDown = true;
}

void CWeapon_Spirit::Render_Off(void) {
	m_bDissolveUpDown = false;
}

void CWeapon_Spirit::Update_Network(const string& pKey) {
	Json::Value root;
	if ("Hit_Slash" == pKey) {
		root["Hit_ID"] = static_cast<_int>(m_iColID);
		root["Hit_Dir"] = static_cast<_uint>(m_iCamperHitDir);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
}

void CWeapon_Spirit::Update_Network(void) {
	list<Json::Value>& root = m_pNetwork->Get_Recv(m_iId);
	for (auto iter = root.begin(); iter != root.end();) {
		string key = (*iter)["Key"].asString();
		if ("Hit_Slash" == key) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), static_cast<_int>((*iter)["Hit_ID"].asInt())));
			m_iCamperHitDir = static_cast<_uint>((*iter)["Hit_Dir"].asInt());
			if (2 == pCamper->Get_Hp())
				pCamper->Get_FM_Camper()->Set_State((CFM_Camper::EState)m_iCamperHitDir);
			else if (1 == pCamper->Get_Hp())
				pCamper->Get_FM_Camper()->Set_State((CFM_Camper::EState)m_iCamperHitDir);

			// test
			m_pSlasher_Audio->Play_Sound_Rand(TEXT("sfx_hatchet_special_hit_blood"), 5, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });

			iter = root.erase(iter);
		}
		else {
			++iter;
		}
	}
}
