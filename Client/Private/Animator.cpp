#include "pch.h"
#include "Animator.h"
#include "Effect_Blood_Free.h"

CAnimator* const CAnimator::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork) {
	CAnimator* pInstnace = new CAnimator(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CAnimator::Clone(void* const& pArg) {
	CAnimator* pInstnace = new CAnimator(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CAnimator::CAnimator(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CAnimator::CAnimator(const CAnimator& rhs)
	: CObject(rhs) {
}

void CAnimator::Delete(void) {
	__super::Delete();

	if (m_sModel1 != nullptr) {
		Safe_Release(m_sModel1->m_pTransform);
		Safe_Release(m_sModel1->m_pRenderer);
		Safe_Release(m_sModel1->m_pShader);
		Safe_Release(m_sModel1->m_pModel);
		Safe_Delete(m_sModel1);
	}
	Safe_Release(m_pTransformm);
	Safe_Release(m_pDissolveAlphaTexture);
	Safe_Release(m_pDissolveColorTexture);
	Safe_Release(m_pDissolveShapeTexture);
}

const HRESULT CAnimator::Init_Create(void) {
	return S_OK;
}

const HRESULT CAnimator::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransformm));

	m_sModel1 = new NEWMODEL;

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform1"), reinterpret_cast<CComponent*&>(m_sModel1->m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_sModel1->m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_sModel1->m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Cheryl"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_sModel1->m_pModel));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Dissolve_border_gradient_02"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pDissolveColorTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_DissolveTexturesEntity"), TEXT("Texture1"), reinterpret_cast<CComponent*&>(m_pDissolveShapeTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Fxt_verticalGradient1"), TEXT("Texture2"), reinterpret_cast<CComponent*&>(m_pDissolveAlphaTexture));

	m_sModel1->m_iAnimCount = m_sModel1->m_pModel->Get_AnimCount();
	m_sModel1->m_iBoneCount = static_cast<_int>(m_sModel1->m_pModel->Get_Bone().size());

	for (_uint i = 0; i < m_sModel1->m_iAnimCount; i++)
		m_sModel1->m_vecAnimName.push_back(m_sModel1->m_pModel->Get_Anim_IdxName(i));
	for (_uint i = 0; i < m_sModel1->m_iBoneCount; i++)
		m_sModel1->m_vecBoneName.push_back(m_sModel1->m_pModel->Get_bone_Name(i));


	m_fRootOffsetMatrix = m_sModel1->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = { 10.f,0.f,10.f }; // 플레이어 생성포지션, 상태시작, 애니메이션 시작위치

	return hr;
}

const _ubyte CAnimator::Update(const _double& dTimeDelta) {
	_vector vPos;
	_float4x4 vSocket;
	_matrix vEffectPos1, vEffectPos2;
	if (m_sModel1 != nullptr) {
		XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_sModel1->m_pModel->Get_RootBoneMatrix("joint_Char")/* * (m_sModel1->m_pTransform->Get_Matrix())*/));
		vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
		m_sModel1->m_pTransform->Set_Row(CTransform::ERow::POS, vPos);
		if (m_sModel1->m_pModel->Update_Anim(m_sModel1->m_dAnimSpeed * dTimeDelta, m_bRepeat) == 1)
			XMStoreFloat3(&m_fRootPrePos, vPos);
	}
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_float time = static_cast<_float>(m_sModel1->m_pModel->Get_Anim_TimeAcc());
	// 65 몸쪽한번(joint_TorsoB_01) 머리한번(joint_FacialGroup), 머리쪽 계속 (joint_FacialGroup)
	// 163 머리에 달린피, 몸쪽피
	// 235 가슴쪽 위로튀는 피
	// 265 가슴쪽 ~ 316 까지 무기쪽 계속 나오는피
	// 316 뺄때나는피
	if (!strcmp(m_sModel1->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_HK_Mori01")) {
		//95 �ڿ��� Į�� - �ڷ� 2��
		//100 ���� �ѹ��� �Ǻм� - ������ 2��
		//135���� �� ������
		//181 ���� ��
		//225 �� ��
		//246 �� ��
		vEffectPos1 = XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_sModel1->m_pModel->Get_BoneMatrix("joint_TorsoC_01") * (m_sModel1->m_pTransform->Get_Matrix());
		vEffectPos2 = XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_sModel1->m_pModel->Get_BoneMatrix("joint_TorsoB_01") * (m_sModel1->m_pTransform->Get_Matrix());
		if (m_sModel1->m_pModel->Get_Anim_KeyTimeAcc(95)) {
			CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos1);
			pTransform->Rotate(pTransform->Get_Row(CTransform::ERow::RIGHT), 90.f, 1.f);
			CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos1);
			pTransform->Rotate(pTransform->Get_Row(CTransform::ERow::RIGHT), 90.f, 1.f);
			//m_sModel1->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_sModel1->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
		if (m_sModel1->m_pModel->Get_Anim_KeyTimeAcc(100)) {
			CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos1);
			pTransform->Rotate(pTransform->Get_Row(CTransform::ERow::RIGHT), -90.f, 1.f);
			CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos1);
			pTransform->Rotate(pTransform->Get_Row(CTransform::ERow::RIGHT), -90.f, 1.f);
			//m_sModel1->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_sModel1->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
		if (m_sModel1->m_pModel->Get_Anim_KeyTimeAcc(135)) {
			CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos1);
			CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos1);
			//m_sModel1->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_sModel1->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
		if (m_sModel1->m_pModel->Get_Anim_KeyTimeAcc(181)) {
			CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos2);
			CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos2);
			//m_sModel1->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_sModel1->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
		if (m_sModel1->m_pModel->Get_Anim_KeyTimeAcc(225)) {
			CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos2);
			CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos2);
			//m_sModel1->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_sModel1->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
		if (m_sModel1->m_pModel->Get_Anim_KeyTimeAcc(246)) {
			CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos2);
			CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
			pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
			pTransform->Set_Matrix(vEffectPos2);
			//m_sModel1->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_sModel1->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
	}


	ImGui::Separator();
	return OBJ_NOEVENT;
}

void CAnimator::Update_Edit(void) {
	m_pTransformm->Update_Edit(0);
	if (m_sModel1 != nullptr)
		m_sModel1->m_pTransform->Update_Edit(0);

	ImGui::DragFloat("Dissolve", &m_fDissolveLength, 0.01f, 0.f, 10.f);

	ImGui::Separator();

	DeleteModel();
	ImGui::Separator();

	AnimNameIdx();
	BoneNameIdx();

}

void CAnimator::Late_Update(const _double& dTimeDelta) {
	if (m_sModel1 != nullptr)
		m_sModel1->m_pRenderer->Add_Object(CRenderer::EOrder::ALPHABLEND, this);
}

const HRESULT CAnimator::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		if (m_sModel1 != nullptr) {
			CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
			pCamera->Set_RawValue(m_sModel1->m_pShader);

			if (nullptr != m_pDissolveAlphaTexture)
				m_pDissolveAlphaTexture->Set_Resource(m_sModel1->m_pShader, "g_DissolveAlpha", 0);
			if (nullptr != m_pDissolveColorTexture)
				m_pDissolveColorTexture->Set_Resource(m_sModel1->m_pShader, "g_TempTexture", 0);
			if (nullptr != m_pDissolveShapeTexture)
				m_pDissolveShapeTexture->Set_Resource(m_sModel1->m_pShader, "g_DissolveTexture", 0);


			m_sModel1->m_pShader->Set_RawValue("g_DissolveLength", &m_fDissolveLength, sizeof(_float));

			_matrix Matrix = m_pTransformm->Get_Matrix() * m_sModel1->m_pTransform->Get_Matrix();
			m_sModel1->m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
			for (_uint i = 0; i < m_sModel1->m_pModel->Get_NumMesh(); ++i) {
				m_sModel1->m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_sModel1->m_pShader, "g_DiffuseTexture");
				m_sModel1->m_pModel->Render(i, m_sModel1->m_pShader, 1);
			}
		}
	}
										 break;
	case CRenderer::EOrder::ALPHABLEND: {
		if (m_sModel1 != nullptr) {
			CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
			pCamera->Set_RawValue(m_sModel1->m_pShader);

			if (nullptr != m_pDissolveAlphaTexture)
				m_pDissolveAlphaTexture->Set_Resource(m_sModel1->m_pShader, "g_DissolveAlpha", 0);
			if (nullptr != m_pDissolveColorTexture)
				m_pDissolveColorTexture->Set_Resource(m_sModel1->m_pShader, "g_TempTexture", 0);
			if (nullptr != m_pDissolveShapeTexture)
				m_pDissolveShapeTexture->Set_Resource(m_sModel1->m_pShader, "g_DissolveTexture", 0);


			m_sModel1->m_pShader->Set_RawValue("g_DissolveLength", &m_fDissolveLength, sizeof(_float));

			_matrix Matrix = m_pTransformm->Get_Matrix() * m_sModel1->m_pTransform->Get_Matrix();
			m_sModel1->m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
			for (_uint i = 0; i < m_sModel1->m_pModel->Get_NumMesh(); ++i) {
				m_sModel1->m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_sModel1->m_pShader, "g_DiffuseTexture");
				m_sModel1->m_pModel->Render(i, m_sModel1->m_pShader, 1);
			}
		}
	}
										 break;
	}
	return S_OK;
}

HRESULT CAnimator::DeleteModel() {
	_bool bCreate = false;
	if (ImGui::Button("DeleteModel")) {};
	if (ImGui::IsItemClicked(0)) bCreate = true;

	if (!bCreate)
		return S_OK;

	if (m_sModel1 != nullptr) {
		Safe_Release(m_sModel1->m_pTransform);
		Safe_Release(m_sModel1->m_pRenderer);
		Safe_Release(m_sModel1->m_pShader);
		Safe_Release(m_sModel1->m_pModel);
		Safe_Delete(m_sModel1);
	}
	return S_OK;
}

HRESULT CAnimator::DirectDeleteModel() {
	if (m_sModel1 != nullptr) {
		Safe_Release(m_sModel1->m_pTransform);
		Safe_Release(m_sModel1->m_pRenderer);
		Safe_Release(m_sModel1->m_pShader);
		Safe_Release(m_sModel1->m_pModel);
		Safe_Delete(m_sModel1);
	}
	return S_OK;
}

void CAnimator::AnimNameIdx() {
	if (m_sModel1 == nullptr)
		return;
	// anime list
	if (ImGui::TreeNode("Animation Index1")) {
		for (_uint n = 0; n < m_sModel1->m_iAnimCount; n++) {
			const bool is_selected = (m_sModel1->m_iAnimCuridx == n);
			if (ImGui::Selectable(m_sModel1->m_vecAnimName[n], is_selected))
				m_sModel1->m_iAnimCuridx = n;

			m_sModel1->m_pModel->Set_Anim(m_sModel1->m_iAnimCuridx);
		}
		ImGui::TreePop();
	}

	ImGui::Checkbox(("Repeat"), &m_bRepeat);
	//progress bar - key frame
	m_sModel1->m_fAnimDuration = static_cast<_float>(m_sModel1->m_pModel->Get_Anim_Duration());
	m_sModel1->m_fAnimKeyFrame = static_cast<_float>(m_sModel1->m_pModel->Get_Anim_TimeAcc());
	ImGui::SliderFloat("AnimationKeyFrame Progress1", &m_sModel1->m_fAnimKeyFrame, 0.0f, m_sModel1->m_fAnimDuration);

	//progress bar - anime speed	
	ImGui::SliderFloat("AnimationSpeed Progress1", &m_sModel1->m_fAnimeSpeed, 0.0f, 10.f);
	m_sModel1->m_dAnimSpeed = static_cast<_double>(m_sModel1->m_fAnimeSpeed);


	if (ImGui::Button("Set")) {};
	if (ImGui::IsItemClicked(0)) {
		m_sModel1->m_fAnimeSpeed = 1.f;
	}

	// play, stop
	if (ImGui::Button("Play")) {}ImGui::SameLine();
	if (ImGui::IsItemClicked(0)) {
		m_sModel1->m_bAnimPlay = !m_sModel1->m_bAnimPlay;
		m_sModel1->m_bAnimStop = false;
	}

	if (ImGui::Button("Stop")) {};
	if (ImGui::IsItemClicked(0)) {
		m_sModel1->m_bAnimStop = true;
		m_dRootTime = 0.;
		m_bRootPos = false;
	}
	if (m_sModel1->m_bAnimStop) {
		m_sModel1->m_bAnimPlay = false;
		m_sModel1->m_dAnimSpeed = 0.;
		m_sModel1->m_pModel->Set_Anim_TimeAcc(0.);
		m_sModel1->m_bAnimStop = false;
	}

	if (m_sModel1->m_bAnimPlay) {
		m_sModel1->m_dAnimSpeed = static_cast<_double>(m_sModel1->m_fAnimeSpeed);
		m_sModel1->m_bAnimStop = false;
	}
	else
		m_sModel1->m_dAnimSpeed = 0.;
}

void CAnimator::BoneNameIdx() {
	if (m_sModel1 == nullptr)
		return;
	if (ImGui::BeginListBox("BoneIdx")) {
		for (_uint i = 0; i < m_sModel1->m_iBoneCount; i++) {
			const bool is_selected = (m_sModel1->m_iBoneCount == i);
			if (ImGui::Selectable(m_sModel1->m_vecBoneName[i], is_selected))
				m_sModel1->m_iBoneCuridx = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	if (ImGui::TreeNode("Bone Material")) {
		vector<_matrix> vecBoneMatrix;
		for (_uint i = 0; i < m_sModel1->m_iBoneCount; i++)
			vecBoneMatrix.push_back(m_sModel1->m_pModel->Get_BoneMatrix(m_sModel1->m_vecBoneName[i]));

		if (ImGui::BeginTable("Material", 4)) {
			for (_uint i = 0; i < m_sModel1->m_iBoneCount; i++) {
				/*const bool is_selected = (m_sModel1->m_iBoneCuridx == i);
				if (ImGui::Selectable(m_sModel1->m_vecBoneName[i], is_selected)) {
					m_sModel1->m_iBoneCuridx = i;
					m_vecBoneName.push_back(m_sModel1->m_vecBoneName[m_sModel1->m_iBoneCuridx]);
				}*/
				if (ImGui::Button(m_sModel1->m_pModel->Get_bone_Name(i))) {};
				if (ImGui::IsItemClicked(0)) m_vecBoneName.push_back(m_sModel1->m_vecBoneName[m_sModel1->m_iBoneCuridx]);
				_float4x4 fBonefloat4x4;
				XMStoreFloat4x4(&fBonefloat4x4, vecBoneMatrix[i]);
				for (_uint row = 0; row < 4; row++) {
					ImGui::TableNextRow();
					for (int column = 0; column < 4; column++) {
						ImGui::TableSetColumnIndex(column);
						_float cn = (_float)fBonefloat4x4.m[row][column];
						ImGui::Text("%f", cn);
					}
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

