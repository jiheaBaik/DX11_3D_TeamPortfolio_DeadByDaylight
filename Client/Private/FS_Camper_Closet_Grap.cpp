#include "pch.h"
#include "FS_Camper_Closet_Grap.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Slasher_Huntress.h"

CFS_Camper_Closet_Grap::CFS_Camper_Closet_Grap(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Closet_Grap::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Closet_Grap::Init(_char* ModelName) {

	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_pCamper_GamePlay->m_bFlag = true;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), m_pCamper_GamePlay->m_iTarObjID);
		m_pSlasher = dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), m_pCamper_GamePlay->m_iTarObjID);
		m_pSlasher = dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pTarObject);
	}
	_vector vRight = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::RIGHT);
	_vector vDist = XMVector3Normalize(vRight) * 0.22f;
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) + vDist);
	/*m_pSlasherTarnsform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
	m_pSlasherTarnsform2 = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform_GrabLocker")));
	XMMATRIX SlasherMatrix = m_pSlasherTarnsform2->Get_Matrix() * m_pSlasherTarnsform->Get_Matrix();
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, SlasherMatrix.r[3]);*/

	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);
	return S_OK;
}

const _ubyte  CFS_Camper_Closet_Grap::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {

		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(30))
		{
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.5f, FMOD_2D);
				break;
			}
		}

		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(70))
		{
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.5f, FMOD_2D);
				break;
			}
		}

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
			return CFM_Camper::EState::CARRY_IDLE;
	}
	else {

		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(30))
		{
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			}
		}
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(70))
		{
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			}
		}

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_TT_Carry_Idle");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_TT_Carry_Idle");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_TT_Carry_Idle");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|DF_DSkeleton_REF");
				break;
			}
		}
	}

	/*CModel* pHuntressModel = dynamic_cast<CModel*>(m_pSlasher->Get_Component(TEXT("Model_FPV")));
	CTransform* pHuntressTransform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
	_matrix SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pHuntressTransform->Get_Matrix();
	m_pCamper_GamePlay->m_pTransform->Set_Matrix(SocketMatrix);*/


	/*m_pSlasherTarnsform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
	m_pSlasherTarnsform2 = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform_GrabLocker")));
	XMMATRIX SlasherMatrix = m_pSlasherTarnsform2->Get_Matrix() * m_pSlasherTarnsform->Get_Matrix();
	_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = SlasherMatrix.r[3] * 0.2f + XMLoadFloat3(&m_fRootPrePos) * 0.8f;
	XMStoreFloat3(&m_fRootPrePos, vLerpPos);


	_vector vRootPos;
	_float4x4 vSocket;
	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
	vRootPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vRootPos);*/

	if (!dynamic_cast<CSlasher_Huntress*>(m_pSlasher)->Get_Slasher()) {
		if (15. <= m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc()) {

			CModel* pHuntressModel = dynamic_cast<CModel*>(m_pSlasher->Get_Component(TEXT("Model_FPV")));
			CTransform* pHuntressTransform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
			_matrix SocketMatrix;
			SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CarryLT_01") * pHuntressTransform->Get_Matrix();
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);
		}

	}
	else {
		_vector vRootPos;
		_float4x4 vSocket;
		CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
		_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
		matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
		vRootPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vRootPos);
	}

	return CFM_Camper::EState::END;
}
