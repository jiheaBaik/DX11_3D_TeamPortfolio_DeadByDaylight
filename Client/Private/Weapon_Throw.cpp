#include "pch.h"
#include "Weapon_Throw.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "Effect_Blood_Free.h"
#include "Effect_Free3.h"
#include "Throw_Trail.h"
#include "UI_Play_Score.h"

CWeapon_Throw* const CWeapon_Throw::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CWeapon_Throw* pInstance = new CWeapon_Throw(pDevice, pContext, pNetwork);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CObject* const CWeapon_Throw::Clone(void* const& pArg) {
	CWeapon_Throw* pInstance = new CWeapon_Throw(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CWeapon_Throw::CWeapon_Throw(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CWeapon_Throw::CWeapon_Throw(const CWeapon_Throw& rhs)
	: CObject(rhs) {
}

void CWeapon_Throw::Delete(void) {
	__super::Delete();
	Safe_Release(m_pNavi);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
	Safe_Release(m_pAudio);
}

const HRESULT CWeapon_Throw::Init_Create(void) {
	return S_OK;
}

const HRESULT CWeapon_Throw::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Weapon_Throw"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));
	m_pCollider->Set_Center(_float3{ -0.06f, 0.f, 0.f });
	m_pCollider->Set_Extents(_float3{ 0.4f, 0.4f, 0.4f });

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Navi"), TEXT("Navi"), reinterpret_cast<CComponent*&>(m_pNavi));

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pSlasher_Huntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	m_pSlasher_Transform = dynamic_cast<CTransform*>(m_pSlasher_Huntress->Get_Component(TEXT("Transform")));

	m_pThrow_Trail = dynamic_cast<CThrow_Trail*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Throw_Trail"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Throw_Trail")));

	return hr;
}

const _ubyte CWeapon_Throw::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (true == m_pSlasher_Huntress->Get_Control()) {
		if (true == m_bThrow) {
			if (false == m_bLook) {
				m_bLook = true;
				//m_pTransform->LookAt(m_pTransform->Get_Row(CTransform::ERow::POS) + XMVector3Normalize(XMLoadFloat3(&m_vThrowDir)));
				m_pTransform->Set_Row(CTransform::ERow::RIGHT, m_pSlasher_Transform->Get_Row(CTransform::ERow::RIGHT));
				m_pTransform->Set_Row(CTransform::ERow::UP, m_pSlasher_Transform->Get_Row(CTransform::ERow::UP));
				m_pTransform->Set_Row(CTransform::ERow::LOOK, m_pSlasher_Transform->Get_Row(CTransform::ERow::LOOK));

				XMStoreFloat3(&m_vRotDir, m_pTransform->Get_Row(CTransform::ERow::RIGHT));

				m_pAudio->Play_Sound(TEXT("sfx_hatchet_throw_loop_03"), 8, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			m_pAudio->Set_3DAttribute(8, m_pTransform);

			m_pTransform->Translate(XMLoadFloat3(&m_vThrowDir), 20.f, dTimeDelta);
			m_pTransform->Rotate(XMLoadFloat3(&m_vRotDir), XMConvertToRadians(180.f), dTimeDelta * 2.f);
			//m_pTransform->Translate(pSlasherTransform->Get_Row(CTransform::ERow::LOOK), 30.f, dTimeDelta);
			//m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(180.f), dTimeDelta * 3.f);

			list<class CObject*>* CamperList = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_Scene(), TEXT("Camper_GamePlay"));
			if (nullptr != CamperList) {
				for (_int i = 0; i < CamperList->size(); i++) {
					CCamper_GamePlay* pCamper_GamePlay = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), i));
					if (nullptr != pCamper_GamePlay) {
						CCollider* pCamper_GamePlayCollider = dynamic_cast<CCollider*>(pCamper_GamePlay->Get_Component(TEXT("Collider")));
						if (pCamper_GamePlay->Get_FM_Camper()->Get_State() != CFM_Camper::CLOSSET_IDLE && pCamper_GamePlay->Get_FM_Camper()->Get_State() != CFM_Camper::ENDGAME
							&& true == m_pCollider->Is_Collision(pCamper_GamePlayCollider)) {
							m_iColID = pCamper_GamePlay->Get_Id();
							if (2 == pCamper_GamePlay->Get_Hp()) {
								CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper_GamePlay->Get_Component(TEXT("Transform")));
								_vector vCamperLook = pCamperTransform->Get_Row(CTransform::ERow::LOOK);
								_float fDot = ((XMVectorGetX(XMVector3Dot(vCamperLook, m_pTransform->Get_Row(CTransform::ERow::POS) - pCamperTransform->Get_Row(CTransform::ERow::POS)))));

								if ((fDot) >= 0.f) {
									m_iCamperHitDir = CFM_Camper::EState::HIT_F;
								}
								else if ((fDot) < 0.f)
									m_iCamperHitDir = CFM_Camper::EState::HIT_B;

								pCamper_GamePlay->Get_FM_Camper()->Set_State((CFM_Camper::EState)m_iCamperHitDir);
								pCamper_GamePlay->Call_BloodEffect(0); // ÀÌÆåÆ® È£Ãâ

								// yeon effect
								CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
								if (pBloodEffect == nullptr)
									break;
								CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
								if (pTransform == nullptr)
									break;
								pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 0.f, 0.f, 0.f));

								m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
								m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_special_hit_blood"), 5, -1, 0.3f, FMOD_2D);

								Update_Network("Hit_Throw");
								m_bThrow = false;

								//hwang - score
								CInfo* pInfo = CInfo::Get_Instance();
								if (pInfo->Get_Player() == true) {
									CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
									CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
									if (pScore != nullptr && pSlasher != nullptr) {
										pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::DEVIOUSNESS_PRECISE_SHOT));
										pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::DEVIOUSNESS), 300);
									}
								}
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
								pCamper_GamePlay->Get_FM_Camper()->Set_State((CFM_Camper::EState)m_iCamperHitDir);
								pCamper_GamePlay->Call_BloodEffect(1); // ÀÌÆåÆ® È£Ãâ

								// yeon effect
								CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
								if (pBloodEffect == nullptr)
									break;
								CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
								if (pTransform == nullptr)
									break;
								pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 0.f, 0.f, 0.f));

								m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
								m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_special_hit_blood"), 5, -1, 0.3f, FMOD_2D);

								Update_Network("Hit_Throw");
								m_bThrow = false;

								//hwang - score
								CInfo* pInfo = CInfo::Get_Instance();
								if (pInfo->Get_Player() == true) {
									CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
									if (pScore != nullptr) {
										pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::DEVIOUSNESS_PRECISE_SHOT));
										pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::DEVIOUSNESS), 300);
									}
								}
							}
						}
					}
				}
			}

			m_vAccel.y += -4.f * static_cast<_float>(dTimeDelta);
			_vector vResist = XMVector3Normalize(XMVectorSetY(-XMLoadFloat3(&m_vAccel), 0.f));
			vResist *= 5.f * static_cast<_float>(dTimeDelta);
			XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vResist);
			m_pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_vAccel) * static_cast<_float>(dTimeDelta));

			// collision
			_float fDist = 0.f;
			for (auto& iter : m_pNavi->Get_DivCell(m_pTransform)) {
				if (!m_pCollider->Is_Collision(iter))
					break;
				// yeon effect
				CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_HuntressWeaponThrow"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_HuntressWeaponThrow")));
				if (pEffect == nullptr)
					break;
				CTransform* pTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
				if (pTransform == nullptr)
					break;
				pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS));
				m_bThrow = false;
				Update_Network("Hit_Wall");
			}

			// ¹Ù´Ú Ãæµ¹
			_float3 vPos2 = {};
			XMStoreFloat3(&vPos2, m_pTransform->Get_Row(CTransform::ERow::POS));

			if (vPos2.y <= 0.1f) {
				CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_HuntressWeaponThrow"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_HuntressWeaponThrow")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS));

				m_bThrow = false;
				//m_pAudio->Play_Sound(TEXT("sfx_hatchet_break_01"), -1, 1.f, FMOD_2D);
				Update_Network("Hit_Wall");
			}
		}
		else if (false == m_bThrow) {
			m_pAudio->Stop_Sound(8);
			CModel* pModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model_FPV")));
			_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_WeaponLT_01");

			_matrix SocketMatrix;
			SocketMatrix = BoneMatrix * m_pSlasher_Transform->Get_Matrix();

			/*m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
			m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
			m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);*/
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, m_pSlasher_Transform->Get_Row(CTransform::ERow::RIGHT));
			m_pTransform->Set_Row(CTransform::ERow::UP, m_pSlasher_Transform->Get_Row(CTransform::ERow::UP));
			m_pTransform->Set_Row(CTransform::ERow::LOOK, m_pSlasher_Transform->Get_Row(CTransform::ERow::LOOK));
			m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] + XMVectorSet(0.f, 0.35f, 0.f, 0.f));

			m_bLook = false;

			m_vAccel.y = 0.f;
		}
	}
	else {
		if (true == m_bThrow) {
			if (false == m_bLook) {
				m_bLook = true;
				//m_pTransform->LookAt(m_pTransform->Get_Row(CTransform::ERow::POS) + XMVector3Normalize(XMLoadFloat3(&m_vThrowDir)));
				m_pTransform->Set_Row(CTransform::ERow::RIGHT, m_pSlasher_Transform->Get_Row(CTransform::ERow::RIGHT));
				m_pTransform->Set_Row(CTransform::ERow::UP, m_pSlasher_Transform->Get_Row(CTransform::ERow::UP));
				m_pTransform->Set_Row(CTransform::ERow::LOOK, m_pSlasher_Transform->Get_Row(CTransform::ERow::LOOK));

				XMStoreFloat3(&m_vRotDir, m_pTransform->Get_Row(CTransform::ERow::RIGHT));

				m_pAudio->Play_Sound(TEXT("sfx_hatchet_throw_loop_03"), 8, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			m_pAudio->Set_3DAttribute(8, m_pTransform);

			m_pTransform->Translate(XMLoadFloat3(&m_vThrowDir), 30.f, dTimeDelta);
			m_pTransform->Rotate(XMLoadFloat3(&m_vRotDir), XMConvertToRadians(180.f), dTimeDelta * 3.f);
			//m_pTransform->Rotate(XMLoadFloat3(&m_vLook), XMConvertToRadians(-180.f), dTimeDelta * 3.f);
			//m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(180.f), dTimeDelta * 3.f);

			m_vAccel.y += -4.f * static_cast<_float>(dTimeDelta);
			_vector vResist = XMVector3Normalize(XMVectorSetY(-XMLoadFloat3(&m_vAccel), 0.f));
			vResist *= 5.f * static_cast<_float>(dTimeDelta);
			XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vResist);
			m_pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_vAccel) * static_cast<_float>(dTimeDelta));
		}
		else if (false == m_bThrow) {
			m_pAudio->Stop_Sound(8);
			CModel* pModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));
			_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_WeaponLT_01");

			_matrix SocketMatrix;
			SocketMatrix = BoneMatrix * m_pSlasher_Transform->Get_Matrix();

			/*m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
			m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
			m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);*/
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, m_pSlasher_Transform->Get_Row(CTransform::ERow::RIGHT));
			m_pTransform->Set_Row(CTransform::ERow::UP, m_pSlasher_Transform->Get_Row(CTransform::ERow::UP));
			m_pTransform->Set_Row(CTransform::ERow::LOOK, m_pSlasher_Transform->Get_Row(CTransform::ERow::LOOK));
			m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

			m_bLook = false;

			m_vAccel.y = 0.f;
		}
	}

	m_pCollider->Update(m_pTransform->Get_Matrix());
	Update_Network();

	if (true == m_bThrow)
		m_dTimeDelta += dTimeDelta;
	else
		m_dTimeDelta = 0.;

	if (m_dTimeDelta > 2.0) {
		m_bThrow = false;
		m_dTimeDelta = 0.;
	}

	return OBJ_NOEVENT;
}

void CWeapon_Throw::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(1);

	ImGui::DragFloat("Roughness", &m_fRoughness, 0.001f);
	ImGui::DragFloat("Refract", &m_fRefract, 0.001f);
	ImGui::DragFloat("Metalness", &m_fMetalness, 0.001f);
	ImGui::DragFloat("Metalness2", &m_fMetalness2, 0.001f);
}

void CWeapon_Throw::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CWeapon_Throw::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		if (true == m_bThrow) {
			m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
			m_pShader->Set_RawValue("g_fRefract", &m_fRefract, sizeof(_float));
			m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
			m_pShader->Set_RawValue("g_fMetalness2", &m_fMetalness2, sizeof(_float));
			m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
				m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel->Render(i, m_pShader, 0);
			}
		}
		break;
	}
										 break;
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const _bool& CWeapon_Throw::Get_Throw(void) {
	return m_bThrow;
}

void CWeapon_Throw::Set_Throw(const _bool& bThrow) {
	m_bThrow = bThrow;
}

const _float3& CWeapon_Throw::Get_Dir(void) {
	return m_vThrowDir;
}

void CWeapon_Throw::Set_Dir(const _float3& vDir) {
	m_vThrowDir = vDir;
}

const _float3& CWeapon_Throw::Get_RotDir(void) {
	return m_vRotDir;
}

void CWeapon_Throw::Set_RotDir(const _float3& vRotDir) {
	m_vRotDir = vRotDir;
}

void CWeapon_Throw::Update_Network(const string& pKey) {
	Json::Value root;
	if ("Hit_Throw" == pKey) {
		root["Hit_ID"] = static_cast<_int>(m_iColID);
		root["Hit_Dir"] = static_cast<_uint>(m_iCamperHitDir);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Hit_Wall" == pKey) {
		root["Hit_Wall"] = static_cast<_bool>(m_bThrow);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_Dir" == pKey) {
		root["Dir"]["x"] = m_vThrowDir.x;
		root["Dir"]["y"] = m_vThrowDir.y;
		root["Dir"]["z"] = m_vThrowDir.z;
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
}

void CWeapon_Throw::Update_Network(void) {
	list<Json::Value>& root = m_pNetwork->Get_Recv(m_iId);
	for (auto iter = root.begin(); iter != root.end();) {
		string key = (*iter)["Key"].asString();
		if ("Hit_Throw" == key) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), static_cast<_int>((*iter)["Hit_ID"].asInt())));
			m_iCamperHitDir = static_cast<_uint>((*iter)["Hit_Dir"].asInt());
			if (2 == pCamper->Get_Hp())
				pCamper->Get_FM_Camper()->Set_State((CFM_Camper::EState)m_iCamperHitDir);
			else if (1 == pCamper->Get_Hp())
				pCamper->Get_FM_Camper()->Set_State((CFM_Camper::EState)m_iCamperHitDir);
			m_bThrow = false;
			// yeon effect
			CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
			if (pBloodEffect == nullptr)
				break;
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
			if (pTransform == nullptr)
				break;
			pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 0.f, 0.f, 0.f));

			// test
			m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_special_hit_blood"), 5, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });

			iter = root.erase(iter);
		}
		else if ("Hit_Wall" == key) {
			m_bThrow = static_cast<_bool>((*iter)["Hit_Wall"].asBool());
			// yeon effect
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_HuntressWeaponThrow"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_HuntressWeaponThrow")));
			if (pEffect == nullptr)
				break;
			CTransform* pTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
			if (pTransform == nullptr)
				break;
			pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS));

			// test
			//m_pAudio->Play_Sound(TEXT("sfx_hatchet_break_01"), -1, 1.f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

			iter = root.erase(iter);
		}
		else if ("Set_Dir" == key) {
			m_vThrowDir.x = (*iter)["Dir"]["x"].asFloat();
			m_vThrowDir.y = (*iter)["Dir"]["y"].asFloat();
			m_vThrowDir.z = (*iter)["Dir"]["z"].asFloat();
			iter = root.erase(iter);
		}
		else {
			++iter;
		}
	}
}
