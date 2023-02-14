#include "pch.h"
#include "Effect_Edit.h"

CEffect_Edit* const CEffect_Edit::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEffect_Edit* pInstnace = new CEffect_Edit(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEffect_Edit::Clone(void* const& pArg) {
	CEffect_Edit* pInstnace = new CEffect_Edit(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect_Edit::CEffect_Edit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEffect_Edit::CEffect_Edit(const CEffect_Edit& rhs)
	: CObject(rhs) {
}

void CEffect_Edit::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader[0]);
	Safe_Release(m_pShader[1]);
	Safe_Release(m_pRect);

	for (auto& iter : m_vecSEffect) {
		Safe_Release(iter.pTransform);
		Safe_Release(iter.pModel);
		Safe_Release(iter.pTexture);
	}
	m_vecSEffect.clear();
}

const HRESULT CEffect_Edit::Init_Create() {
	return S_OK;
}

const HRESULT CEffect_Edit::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Component_Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Component_Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Component_Shader0"), reinterpret_cast<CComponent*&>(m_pShader[0]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), TEXT("Component_Shader1"), reinterpret_cast<CComponent*&>(m_pShader[1]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Component_Rect_Icon"), reinterpret_cast<CComponent*&>(m_pRect));
	return hr;
}

const _ubyte CEffect_Edit::Update(const _double& dTimeDelta) {
	m_fTime[0] += static_cast<_float>(dTimeDelta);
	//if (m_fEndTime[0] < m_fTime[0]) {
	//	return OBJ_ERASE;
	//}
	m_fTime[1] += static_cast<_float>(dTimeDelta);
	if (m_fEndTime[1] < m_fTime[1]) {
		if (true == m_bLoop) {
			m_fTime[1] = 0.f;
			Init_Effect();
		}
	}

	for (auto& iter : m_vecSEffect) {
		_float fTime = m_fTime[1] - iter.fTime.x;
		if (0.f > fTime || iter.fTime.y < fTime)
			continue;

		iter.vAccel.y += iter.fGravity * static_cast<_float>(dTimeDelta);
		_vector vResist = XMVector3Normalize(-XMLoadFloat3(&iter.vAccel));
		vResist *= iter.fResist * static_cast<_float>(dTimeDelta);
		XMStoreFloat3(&iter.vAccel, XMLoadFloat3(&iter.vAccel) + vResist);
		iter.pTransform->Set_Row(CTransform::ERow::POS, iter.pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&iter.vAccel) * static_cast<_float>(dTimeDelta));

		iter.pTransform->Set_Row(CTransform::ERow::POS, iter.pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&iter.vPos) * static_cast<_float>(dTimeDelta));
		iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(iter.vRot.x), dTimeDelta);
		iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::UP), XMConvertToRadians(iter.vRot.y), dTimeDelta);
		iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(iter.vRot.z), dTimeDelta);
		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.pTransform->Get_Scale()) + XMLoadFloat3(&iter.vScale) * static_cast<_float>(dTimeDelta));

		if (nullptr != iter.pModel)
			iter.pModel->Update_Anim(dTimeDelta);
		//Pos
		if (!iter.vecPos.empty()) {
			if (fTime >= iter.vecPos.back().first || iter.vecPos.size() == 1) {
				iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.vecPos.back().second));
			}
			else {
				while (fTime > iter.vecPos[iter.iFrame[0] + 1].first)
					++iter.iFrame[0];
				_vector vSour = XMLoadFloat3(&iter.vecPos[iter.iFrame[0]].second);
				_vector vDest = XMLoadFloat3(&iter.vecPos[iter.iFrame[0] + 1].second);
				_float fRatio = static_cast<_float>((fTime - iter.vecPos[iter.iFrame[0]].first) / (iter.vecPos[iter.iFrame[0] + 1].first - iter.vecPos[iter.iFrame[0]].first));
				_vector vPos = XMVectorLerp(vSour, vDest, fRatio);
				iter.pTransform->Set_Row(CTransform::ERow::POS, vPos);
			}
		}
		//Rot
		for (_uint i = 0; i < 3; ++i) {
			if (!iter.vecRot[i].empty()) {
				_vector vAxis;
				switch (i) {
				case 0:
					vAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);// iter.pTransform->Get_Row(CTransform::ERow::RIGHT);
					break;
				case 1:
					vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f); //iter.pTransform->Get_Row(CTransform::ERow::UP);
					break;
				case 2:
					vAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f); //iter.pTransform->Get_Row(CTransform::ERow::LOOK);
					break;
				}

				if (fTime >= iter.vecRot[i].back().first || iter.vecRot[i].size() == 1) {
					iter.pTransform->Rotation(vAxis, XMConvertToRadians(iter.vecRot[i].back().second));
				}
				else {
					while (fTime > iter.vecRot[i][iter.iFrame[i + 1] + 1].first)
						++iter.iFrame[1];
					_float fSour = iter.vecRot[i][iter.iFrame[i + 1]].second;
					_float fDest = iter.vecRot[i][iter.iFrame[i + 1] + 1].second;
					_float fRatio = static_cast<_float>((fTime - iter.vecRot[i][iter.iFrame[i + 1]].first) / (iter.vecRot[i][iter.iFrame[i + 1] + 1].first - iter.vecRot[i][iter.iFrame[i + 1]].first));
					_float fRot = fSour * (1.f - fRatio) + fDest * fRatio;

					iter.pTransform->Rotation(vAxis, XMConvertToRadians(fRot));
				}
			}
		}
		//Scale
		if (!iter.vecScale.empty()) {
			if (fTime >= iter.vecScale.back().first || iter.vecScale.size() == 1) {
				iter.pTransform->Set_Scale(iter.vecScale.back().second);
			}
			else {
				while (fTime > iter.vecScale[iter.iFrame[4] + 1].first)
					++iter.iFrame[4];
				_vector vSour = XMLoadFloat3(&iter.vecScale[iter.iFrame[4]].second);
				_vector vDest = XMLoadFloat3(&iter.vecScale[iter.iFrame[4] + 1].second);
				_float fRatio = static_cast<_float>((fTime - iter.vecScale[iter.iFrame[4]].first) / (iter.vecScale[iter.iFrame[4] + 1].first - iter.vecScale[iter.iFrame[4]].first));
				_vector vScale = XMVectorLerp(vSour, vDest, fRatio);
				iter.pTransform->Set_Scale(vScale);
			}
		}
	}
	return OBJ_NOEVENT;
}

void CEffect_Edit::Update_Edit(void) {
	if (ImGui::CollapsingHeader("File")) {
		if (ImGui::Button("Load")) {
			Load_Effect(TEXT("../Bin/Resource/Effect.txt"));
			Create_Effect();
			Init_Effect();
		}
		if (ImGui::Button("Save")) {
			Save_Effect(TEXT("../Bin/Resource/Effect.txt"));
		}
	}
	if (ImGui::CollapsingHeader("Base")) {
		ImGui::InputFloat2("CurTime", (float*)&m_fTime);
		ImGui::Checkbox("Loop", &m_bLoop);
		ImGui::InputFloat2("EndTime", (float*)&m_fEndTime);
	}
	if (ImGui::CollapsingHeader("Create")) {
		ImGui::InputText("Model", m_sEffect.szModel, IM_ARRAYSIZE(m_sEffect.szModel));
		ImGui::InputText("Texture", m_sEffect.szTexture, IM_ARRAYSIZE(m_sEffect.szTexture));
		ImGui::Separator();

		ImGui::InputFloat2("Time", (_float*)&m_sEffect.arrTime[0]);
		ImGui::InputFloat2("~Time", (_float*)&m_sEffect.arrTime[1]);
		ImGui::Separator();

		ImGui::DragFloat3("Pos", (_float*)&m_sEffect.arrPos[0]);
		ImGui::DragFloat3("~Pos", (_float*)&m_sEffect.arrPos[1]);
		ImGui::DragFloat3("Rot", (_float*)&m_sEffect.arrRot[0]);
		ImGui::DragFloat3("~Rot", (_float*)&m_sEffect.arrRot[1]);
		ImGui::DragFloat3("Scale", (_float*)&m_sEffect.arrScale[0]);
		ImGui::DragFloat3("~Scale", (_float*)&m_sEffect.arrScale[1]);
		ImGui::Separator();

		ImGui::DragFloat3("Accel", (_float*)&m_sEffect.arrAccel[0]);
		ImGui::DragFloat3("~Accel", (_float*)&m_sEffect.arrAccel[1]);
		ImGui::InputFloat("Gravity", &m_sEffect.fGravity);
		ImGui::InputFloat("Resist", &m_sEffect.fResist);
		ImGui::Separator();

		ImGui::DragFloat3("MovePos", (_float*)&m_sEffect.arrMovePos[0]);
		ImGui::DragFloat3("~MovePos", (_float*)&m_sEffect.arrMovePos[1]);
		ImGui::DragFloat3("MoveRot", (_float*)&m_sEffect.arrMoveRot[0]);
		ImGui::DragFloat3("~MoveRot", (_float*)&m_sEffect.arrMoveRot[1]);
		ImGui::DragFloat3("MoveScale", (_float*)&m_sEffect.arrMoveScale[0]);
		ImGui::DragFloat3("~MoveScale", (_float*)&m_sEffect.arrMoveScale[1]);
		ImGui::Separator();

		if (ImGui::Button("Create1")) {
			Create_Effect(m_sEffect);
			Init_Effect();
		}
	}
	if (ImGui::CollapsingHeader("Revise")) {
		_uint i = 0;
		for (auto iter = m_vecSEffect.begin(); iter != m_vecSEffect.end(); ++iter) {
			char szNode[MAX_PATH];
			sprintf_s(szNode, "Node(%d)", i++);
			if (ImGui::TreeNode(szNode)) {
				ImGui::Text(iter->szModel);
				ImGui::Text(iter->szTexture);
				ImGui::Separator();

				ImGui::InputFloat2("Time", (_float*)&iter->arrTime[0]);
				ImGui::InputFloat2("~Time", (_float*)&iter->arrTime[1]);
				ImGui::Separator();

				ImGui::DragFloat3("Pos", (_float*)&iter->arrPos[0]);
				ImGui::DragFloat3("~Pos", (_float*)&iter->arrPos[1]);
				ImGui::DragFloat3("Rot", (_float*)&iter->arrRot[0]);
				ImGui::DragFloat3("~Rot", (_float*)&iter->arrRot[1]);
				ImGui::DragFloat3("Scale", (_float*)&iter->arrScale[0]);
				ImGui::DragFloat3("~Scale", (_float*)&iter->arrScale[1]);
				ImGui::Separator();

				ImGui::DragFloat3("Accel", (_float*)&iter->arrAccel[0]);
				ImGui::DragFloat3("~Accel", (_float*)&iter->arrAccel[1]);
				ImGui::InputFloat("Gravity", &iter->fGravity);
				ImGui::InputFloat("Resist", &iter->fResist);
				ImGui::Separator();

				ImGui::DragFloat3("MovePos", (_float*)&iter->arrMovePos[0]);
				ImGui::DragFloat3("~MovePos", (_float*)&iter->arrMovePos[1]);
				ImGui::DragFloat3("MoveRot", (_float*)&iter->arrMoveRot[0]);
				ImGui::DragFloat3("~MoveRot", (_float*)&iter->arrMoveRot[1]);
				ImGui::DragFloat3("MoveScale", (_float*)&iter->arrMoveScale[0]);
				ImGui::DragFloat3("~MoveScale", (_float*)&iter->arrMoveScale[1]);
				ImGui::Separator();

				//Pos
				int j = 0;
				if (ImGui::TreeNode("Pos")) {
					for (auto frame = iter->vecPos.begin(); frame != iter->vecPos.end(); ++frame) {
						char szFrame[MAX_PATH];
						sprintf_s(szFrame, "Frame(%d)", j++);
						if (ImGui::TreeNode(szFrame)) {
							ImGui::InputFloat("Time", &frame->first);
							ImGui::InputFloat3("Pos", (_float*)&frame->second);
							if (ImGui::Button("Erase")) {
								ImGui::TreePop();
								iter->iFrame[0] = 0;
								iter->vecPos.erase(frame);
								break;
							}
							ImGui::TreePop();
						}
					}
					if (ImGui::Button("Create"))
						iter->vecPos.emplace_back();
					ImGui::TreePop();
				}
				//Rot
				if (ImGui::TreeNode("RotX")) {
					for (auto frame = iter->vecRot[0].begin(); frame != iter->vecRot[0].end(); ++frame) {
						char szFrame[MAX_PATH];
						sprintf_s(szFrame, "Frame(%d)", j++);
						if (ImGui::TreeNode(szFrame)) {
							ImGui::InputFloat("Time", &frame->first);
							ImGui::InputFloat("Rot", &frame->second);
							if (ImGui::Button("Erase")) {
								ImGui::TreePop();
								iter->iFrame[1] = 0;
								iter->vecRot[0].erase(frame);
								break;
							}
							ImGui::TreePop();
						}
					}
					if (ImGui::Button("Create"))
						iter->vecRot[0].emplace_back();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("RotY")) {
					for (auto frame = iter->vecRot[1].begin(); frame != iter->vecRot[1].end(); ++frame) {
						char szFrame[MAX_PATH];
						sprintf_s(szFrame, "Frame(%d)", j++);
						if (ImGui::TreeNode(szFrame)) {
							ImGui::InputFloat("Time", &frame->first);
							ImGui::InputFloat("Rot", &frame->second);
							if (ImGui::Button("Erase")) {
								ImGui::TreePop();
								iter->iFrame[2] = 0;
								iter->vecRot[1].erase(frame);
								break;
							}
							ImGui::TreePop();
						}
					}
					if (ImGui::Button("Create"))
						iter->vecRot[1].emplace_back();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("RotZ")) {
					for (auto frame = iter->vecRot[2].begin(); frame != iter->vecRot[2].end(); ++frame) {
						char szFrame[MAX_PATH];
						sprintf_s(szFrame, "Frame(%d)", j++);
						if (ImGui::TreeNode(szFrame)) {
							ImGui::InputFloat("Time", &frame->first);
							ImGui::InputFloat("Rot", &frame->second);
							if (ImGui::Button("Erase")) {
								ImGui::TreePop();
								iter->iFrame[3] = 0;
								iter->vecRot[2].erase(frame);
								break;
							}
							ImGui::TreePop();
						}
					}
					if (ImGui::Button("Create"))
						iter->vecRot[2].emplace_back();
					ImGui::TreePop();
				}
				//Scale
				if (ImGui::TreeNode("Scale")) {
					for (auto frame = iter->vecScale.begin(); frame != iter->vecScale.end(); ++frame) {
						char szFrame[MAX_PATH];
						sprintf_s(szFrame, "Frame(%d)", j++);
						if (ImGui::TreeNode(szFrame)) {
							ImGui::InputFloat("Time", &frame->first);
							ImGui::InputFloat3("Scale", (_float*)&frame->second);
							if (ImGui::Button("Erase")) {
								ImGui::TreePop();
								iter->iFrame[4] = 0;
								iter->vecScale.erase(frame);
								break;
							}
							ImGui::TreePop();
						}
					}
					if (ImGui::Button("Create"))
						iter->vecScale.emplace_back();
					ImGui::TreePop();
				}
				if (ImGui::Button("Copy")) {
					ImGui::TreePop();
					Create_Effect(*iter);
					Init_Effect();
					break;
				}
				if (ImGui::Button("Erase")) {
					ImGui::TreePop();
					Safe_Release(iter->pTransform);
					Safe_Release(iter->pModel);
					Safe_Release(iter->pTexture);
					m_vecSEffect.erase(iter);
					break;
				}

				ImGui::TreePop();
			}
		}
	}
}

void CEffect_Edit::Late_Update(const _double& dTimeDelta) {
	for (auto& iter : m_vecSEffect) {
		if (!strcmp(iter.szModel, "Effect_Splat2")) {
			iter.pTransform->LookAt(XMVectorSet(0.f, 0.f, 0.f, 0.f));
		}
	}
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	CTransform* pTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Transform")));
	_matrix Matrix = m_pTransform->Get_Matrix_Inverse();
	_vector vPos = pTransform->Get_Row(CTransform::ERow::POS);
	vPos = XMVector3TransformCoord(vPos, Matrix);
	for (auto& iter : m_vecSEffect) {
		if (nullptr == iter.pTexture)
			continue;
		_vector vPos2 = iter.pTransform->Get_Row(CTransform::ERow::POS);
		_vector vDir = vPos2 - vPos;
		iter.pTransform->LookAt(vPos2 + vDir);
	}
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CEffect_Edit::Render(const CRenderer::EOrder& eRenderer) {
	if (m_fEndTime[0] < m_fTime[0]) {
		return S_OK;
	}

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	pCamera->Set_RawValue(m_pShader[0]);
	pCamera->Set_RawValue(m_pShader[1]);

	for (auto& iter : m_vecSEffect) {
		_float fTime = m_fTime[1] - iter.fTime.x;
		if (0.f > fTime || iter.fTime.y < fTime)
			continue;

		if (iter.pModel != nullptr) {
			_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
			m_pShader[0]->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
			for (_uint j = 0; j < iter.pModel->Get_NumMesh(); ++j) {
				iter.pModel->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader[0], "g_DiffuseTexture");
				iter.pModel->Render(j, m_pShader[0], 2);
			}
		}
		if (iter.pTexture != nullptr) {
			_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
			m_pShader[1]->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
			iter.pTexture->Set_Resource(m_pShader[1], "g_DiffuseTexture", 0);
			m_pRect->Render(m_pShader[1], 3);
		}
	}
	return S_OK;
}

void CEffect_Edit::Create_Effect(SEffect& sEffect) {
	SEffect sEffect2 = sEffect;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	sEffect2.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
	string str{ sEffect2.szModel };
	wstring wstr{ str.begin(), str.end() };
	wstr = TEXT("Model_") + wstr;
	sEffect2.pModel = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	str = sEffect2.szTexture;
	wstr.assign(str.begin(), str.end());
	wstr = TEXT("Texture_") + wstr;
	sEffect2.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));

	m_vecSEffect.emplace_back(sEffect2);
}

void CEffect_Edit::Create_Effect(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	for (auto& iter : m_vecSEffect) {
		iter.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
		string str{ iter.szModel };
		wstring wstr{ str.begin(), str.end() };
		wstr = TEXT("Model_") + wstr;
		iter.pModel = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
		str = iter.szTexture;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Texture_") + wstr;
		iter.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	}
}

void CEffect_Edit::Init_Effect(void) {
	for (auto& iter : m_vecSEffect) {
		iter.fTime.x = iter.arrTime[0].x;
		if (9999.f != iter.arrTime[1].x)
			iter.fTime.x = iter.arrTime[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrTime[1].x - iter.arrTime[0].x)));
		iter.fTime.y = iter.arrTime[0].y;
		if (9999.f != iter.arrTime[1].y)
			iter.fTime.y = iter.arrTime[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrTime[1].y - iter.arrTime[0].y)));

		//Pos
		_float3 vPos = iter.arrPos[0];
		if (9999.f != iter.arrPos[1].x)
			vPos.x = iter.arrPos[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrPos[1].x - iter.arrPos[0].x)));
		if (9999.f != iter.arrPos[1].y)
			vPos.y = iter.arrPos[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrPos[1].y - iter.arrPos[0].y)));
		if (9999.f != iter.arrPos[1].z)
			vPos.z = iter.arrPos[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrPos[1].z - iter.arrPos[0].z)));
		iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos));

		//Rot
		iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
		_float fRot = iter.arrRot[0].x;
		if (9999.f != iter.arrRot[1].x)
			fRot = iter.arrRot[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrRot[1].x - iter.arrRot[0].x)));
		iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(fRot), 1.f);

		fRot = iter.arrRot[0].y;
		if (9999.f != iter.arrRot[1].y)
			fRot = iter.arrRot[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrRot[1].y - iter.arrRot[0].y)));
		iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::UP), XMConvertToRadians(fRot), 1.f);

		fRot = iter.arrRot[0].z;
		if (9999.f != iter.arrRot[1].z)
			fRot = iter.arrRot[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrRot[1].z - iter.arrRot[0].z)));
		iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(fRot), 1.f);

		//Scale
		_float3 vScale = iter.arrScale[0];
		if (9999.f != iter.arrScale[1].x)
			vScale.x = iter.arrScale[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrScale[1].x - iter.arrScale[0].x)));
		if (9999.f != iter.arrScale[1].y)
			vScale.y = iter.arrScale[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrScale[1].y - iter.arrScale[0].y)));
		if (9999.f != iter.arrScale[1].z)
			vScale.z = iter.arrScale[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrScale[1].z - iter.arrScale[0].z)));
		iter.pTransform->Set_Scale(vScale);


		iter.vAccel = iter.arrAccel[0];
		if (9999.f != iter.arrAccel[1].x)
			iter.vAccel.x = iter.arrAccel[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrAccel[1].x - iter.arrAccel[0].x)));
		if (9999.f != iter.arrAccel[1].y)
			iter.vAccel.y = iter.arrAccel[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrAccel[1].y - iter.arrAccel[0].y)));
		if (9999.f != iter.arrAccel[1].z)
			iter.vAccel.z = iter.arrAccel[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrAccel[1].z - iter.arrAccel[0].z)));

		//Pos
		iter.vPos = iter.arrMovePos[0];
		if (9999.f != iter.arrMovePos[1].x)
			iter.vPos.x = iter.arrMovePos[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMovePos[1].x - iter.arrMovePos[0].x)));
		if (9999.f != iter.arrMovePos[1].y)
			iter.vPos.y = iter.arrMovePos[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMovePos[1].y - iter.arrMovePos[0].y)));
		if (9999.f != iter.arrMovePos[1].z)
			iter.vPos.z = iter.arrMovePos[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMovePos[1].z - iter.arrMovePos[0].z)));
		//Rot
		iter.vRot = iter.arrMoveRot[0];
		if (9999.f != iter.arrMoveRot[1].x)
			iter.vRot.x = iter.arrMoveRot[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMoveRot[1].x - iter.arrMoveRot[0].x)));
		if (9999.f != iter.arrMoveRot[1].y)
			iter.vRot.y = iter.arrMoveRot[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMoveRot[1].y - iter.arrMoveRot[0].y)));
		if (9999.f != iter.arrMoveRot[1].z)
			iter.vRot.z = iter.arrMoveRot[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMoveRot[1].z - iter.arrMoveRot[0].z)));
		//Pos
		iter.vScale = iter.arrMoveScale[0];
		if (9999.f != iter.arrMoveScale[1].x)
			iter.vScale.x = iter.arrMoveScale[0].x + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMoveScale[1].x - iter.arrMoveScale[0].x)));
		if (9999.f != iter.arrMoveScale[1].y)
			iter.vScale.y = iter.arrMoveScale[0].y + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMoveScale[1].y - iter.arrMoveScale[0].y)));
		if (9999.f != iter.arrMoveScale[1].z)
			iter.vScale.z = iter.arrMoveScale[0].z + (float)(rand()) / ((float)(RAND_MAX / (iter.arrMoveScale[1].z - iter.arrMoveScale[0].z)));

		if (nullptr != iter.pModel) {
			iter.pModel->Set_Anim_TimeAcc(0.);
		}
		for (auto& arr : iter.iFrame)
			arr = 0;
	}
}

void CEffect_Edit::Save_Effect(const _tchar* const& pFilePath) {
	Json::Value root;
	root["EndTime"][0] = m_fEndTime[0];
	root["EndTime"][1] = m_fEndTime[1];
	root["Loop"] = m_bLoop;

	for (auto& iter : m_vecSEffect) {
		Json::Value node;

		node["Model"] = string{ iter.szModel };
		node["Texture"] = string{ iter.szTexture };

		node["Time"][0]["x"] = iter.arrTime[0].x;
		node["Time"][0]["y"] = iter.arrTime[0].y;
		node["Time"][1]["x"] = iter.arrTime[1].x;
		node["Time"][1]["y"] = iter.arrTime[1].y;

		node["Pos"][0]["x"] = iter.arrPos[0].x;
		node["Pos"][0]["y"] = iter.arrPos[0].y;
		node["Pos"][0]["z"] = iter.arrPos[0].z;
		node["Pos"][1]["x"] = iter.arrPos[1].x;
		node["Pos"][1]["y"] = iter.arrPos[1].y;
		node["Pos"][1]["z"] = iter.arrPos[1].z;

		node["Rot"][0]["x"] = iter.arrRot[0].x;
		node["Rot"][0]["y"] = iter.arrRot[0].y;
		node["Rot"][0]["z"] = iter.arrRot[0].z;
		node["Rot"][1]["x"] = iter.arrRot[1].x;
		node["Rot"][1]["y"] = iter.arrRot[1].y;
		node["Rot"][1]["z"] = iter.arrRot[1].z;

		node["Scale"][0]["x"] = iter.arrScale[0].x;
		node["Scale"][0]["y"] = iter.arrScale[0].y;
		node["Scale"][0]["z"] = iter.arrScale[0].z;
		node["Scale"][1]["x"] = iter.arrScale[1].x;
		node["Scale"][1]["y"] = iter.arrScale[1].y;
		node["Scale"][1]["z"] = iter.arrScale[1].z;

		node["Accel"][0]["x"] = iter.arrAccel[0].x;
		node["Accel"][0]["y"] = iter.arrAccel[0].y;
		node["Accel"][0]["z"] = iter.arrAccel[0].z;
		node["Accel"][1]["x"] = iter.arrAccel[1].x;
		node["Accel"][1]["y"] = iter.arrAccel[1].y;
		node["Accel"][1]["z"] = iter.arrAccel[1].z;
		node["Gravity"] = iter.fGravity;
		node["Resist"] = iter.fResist;

		node["MovePos"][0]["x"] = iter.arrMovePos[0].x;
		node["MovePos"][0]["y"] = iter.arrMovePos[0].y;
		node["MovePos"][0]["z"] = iter.arrMovePos[0].z;
		node["MovePos"][1]["x"] = iter.arrMovePos[1].x;
		node["MovePos"][1]["y"] = iter.arrMovePos[1].y;
		node["MovePos"][1]["z"] = iter.arrMovePos[1].z;

		node["MoveRot"][0]["x"] = iter.arrMoveRot[0].x;
		node["MoveRot"][0]["y"] = iter.arrMoveRot[0].y;
		node["MoveRot"][0]["z"] = iter.arrMoveRot[0].z;
		node["MoveRot"][1]["x"] = iter.arrMoveRot[1].x;
		node["MoveRot"][1]["y"] = iter.arrMoveRot[1].y;
		node["MoveRot"][1]["z"] = iter.arrMoveRot[1].z;

		node["MoveScale"][0]["x"] = iter.arrMoveScale[0].x;
		node["MoveScale"][0]["y"] = iter.arrMoveScale[0].y;
		node["MoveScale"][0]["z"] = iter.arrMoveScale[0].z;
		node["MoveScale"][1]["x"] = iter.arrMoveScale[1].x;
		node["MoveScale"][1]["y"] = iter.arrMoveScale[1].y;
		node["MoveScale"][1]["z"] = iter.arrMoveScale[1].z;

		for (auto& iter2 : iter.vecPos) {
			Json::Value frame;
			frame["Time"] = iter2.first;
			frame["Pos"]["x"] = iter2.second.x;
			frame["Pos"]["y"] = iter2.second.y;
			frame["Pos"]["z"] = iter2.second.z;
			node["vecPos"].append(frame);
		}
		for (auto& iter2 : iter.vecRot[0]) {
			Json::Value frame;
			frame["Time"] = iter2.first;
			frame["Rot"] = iter2.second;
			node["vecRot"][0].append(frame);
		}
		for (auto& iter2 : iter.vecRot[1]) {
			Json::Value frame;
			frame["Time"] = iter2.first;
			frame["Rot"] = iter2.second;
			node["vecRot"][1].append(frame);
		}
		for (auto& iter2 : iter.vecRot[2]) {
			Json::Value frame;
			frame["Time"] = iter2.first;
			frame["Rot"] = iter2.second;
			node["vecRot"][2].append(frame);
		}
		for (auto& iter2 : iter.vecScale) {
			Json::Value frame;
			frame["Time"] = iter2.first;
			frame["Scale"]["x"] = iter2.second.x;
			frame["Scale"]["y"] = iter2.second.y;
			frame["Scale"]["z"] = iter2.second.z;
			node["vecScale"].append(frame);
		}

		root["Node"].append(node);
	}

	ofstream ofs(pFilePath);
	Json::StyledWriter sw;
	string str = sw.write(root);
	ofs << str;
	ofs.close();
}

void CEffect_Edit::Load_Effect(const _tchar* const& pFilePath) {
	for (auto& iter : m_vecSEffect) {
		Safe_Release(iter.pTransform);
		Safe_Release(iter.pModel);
		Safe_Release(iter.pTexture);
	}
	m_vecSEffect.clear();

	ifstream ifs(pFilePath);
	Json::Value root;
	ifs >> root;
	ifs.close();

	m_fEndTime[0] = root["EndTime"][0].asFloat();
	m_fEndTime[1] = root["EndTime"][1].asFloat();
	m_bLoop = root["Loop"].asBool();

	SEffect iter;
	for (auto& node : root["Node"]) {
		iter = SEffect{};
		strcpy_s(iter.szModel, node["Model"].asString().c_str());
		strcpy_s(iter.szTexture, node["Texture"].asString().c_str());

		iter.arrTime[0].x = node["Time"][0]["x"].asFloat();
		iter.arrTime[0].y = node["Time"][0]["y"].asFloat();
		iter.arrTime[1].x = node["Time"][1]["x"].asFloat();
		iter.arrTime[1].y = node["Time"][1]["y"].asFloat();

		iter.arrPos[0].x = node["Pos"][0]["x"].asFloat();
		iter.arrPos[0].y = node["Pos"][0]["y"].asFloat();
		iter.arrPos[0].z = node["Pos"][0]["z"].asFloat();
		iter.arrPos[1].x = node["Pos"][1]["x"].asFloat();
		iter.arrPos[1].y = node["Pos"][1]["y"].asFloat();
		iter.arrPos[1].z = node["Pos"][1]["z"].asFloat();

		iter.arrRot[0].x = node["Rot"][0]["x"].asFloat();
		iter.arrRot[0].y = node["Rot"][0]["y"].asFloat();
		iter.arrRot[0].z = node["Rot"][0]["z"].asFloat();
		iter.arrRot[1].x = node["Rot"][1]["x"].asFloat();
		iter.arrRot[1].y = node["Rot"][1]["y"].asFloat();
		iter.arrRot[1].z = node["Rot"][1]["z"].asFloat();

		iter.arrScale[0].x = node["Scale"][0]["x"].asFloat();
		iter.arrScale[0].y = node["Scale"][0]["y"].asFloat();
		iter.arrScale[0].z = node["Scale"][0]["z"].asFloat();
		iter.arrScale[1].x = node["Scale"][1]["x"].asFloat();
		iter.arrScale[1].y = node["Scale"][1]["y"].asFloat();
		iter.arrScale[1].z = node["Scale"][1]["z"].asFloat();

		iter.arrAccel[0].x = node["Accel"][0]["x"].asFloat();
		iter.arrAccel[0].y = node["Accel"][0]["y"].asFloat();
		iter.arrAccel[0].z = node["Accel"][0]["z"].asFloat();
		iter.arrAccel[1].x = node["Accel"][1]["x"].asFloat();
		iter.arrAccel[1].y = node["Accel"][1]["y"].asFloat();
		iter.arrAccel[1].z = node["Accel"][1]["z"].asFloat();
		iter.fGravity = node["Gravity"].asFloat();
		iter.fResist = node["Resist"].asFloat();

		iter.arrMovePos[0].x = node["MovePos"][0]["x"].asFloat();
		iter.arrMovePos[0].y = node["MovePos"][0]["y"].asFloat();
		iter.arrMovePos[0].z = node["MovePos"][0]["z"].asFloat();
		iter.arrMovePos[1].x = node["MovePos"][1]["x"].asFloat();
		iter.arrMovePos[1].y = node["MovePos"][1]["y"].asFloat();
		iter.arrMovePos[1].z = node["MovePos"][1]["z"].asFloat();

		iter.arrMoveRot[0].x = node["MoveRot"][0]["x"].asFloat();
		iter.arrMoveRot[0].y = node["MoveRot"][0]["y"].asFloat();
		iter.arrMoveRot[0].z = node["MoveRot"][0]["z"].asFloat();
		iter.arrMoveRot[1].x = node["MoveRot"][1]["x"].asFloat();
		iter.arrMoveRot[1].y = node["MoveRot"][1]["y"].asFloat();
		iter.arrMoveRot[1].z = node["MoveRot"][1]["z"].asFloat();

		iter.arrMoveScale[0].x = node["MoveScale"][0]["x"].asFloat();
		iter.arrMoveScale[0].y = node["MoveScale"][0]["y"].asFloat();
		iter.arrMoveScale[0].z = node["MoveScale"][0]["z"].asFloat();
		iter.arrMoveScale[1].x = node["MoveScale"][1]["x"].asFloat();
		iter.arrMoveScale[1].y = node["MoveScale"][1]["y"].asFloat();
		iter.arrMoveScale[1].z = node["MoveScale"][1]["z"].asFloat();

		//Pos
		for (auto& frame : node["vecPos"]) {
			pair<_float, _float3> iter2;
			iter2.first = frame["Time"].asFloat();
			iter2.second.x = frame["Pos"]["x"].asFloat();
			iter2.second.y = frame["Pos"]["y"].asFloat();
			iter2.second.z = frame["Pos"]["z"].asFloat();
			iter.vecPos.push_back(iter2);
		}

		//Rot
		for (auto& frame : node["vecRot"][0]) {
			pair<_float, _float> iter2;
			iter2.first = frame["Time"].asFloat();
			iter2.second = frame["Rot"].asFloat();
			iter.vecRot[0].push_back(iter2);
		}
		for (auto& frame : node["vecRot"][1]) {
			pair<_float, _float> iter2;
			iter2.first = frame["Time"].asFloat();
			iter2.second = frame["Rot"].asFloat();
			iter.vecRot[1].push_back(iter2);
		}
		for (auto& frame : node["vecRot"][2]) {
			pair<_float, _float> iter2;
			iter2.first = frame["Time"].asFloat();
			iter2.second = frame["Rot"].asFloat();
			iter.vecRot[2].push_back(iter2);
		}

		for (auto& frame : node["vecScale"]) {
			pair<_float, _float3> iter2;
			iter2.first = frame["Time"].asFloat();
			iter2.second.x = frame["Scale"]["x"].asFloat();
			iter2.second.y = frame["Scale"]["y"].asFloat();
			iter2.second.z = frame["Scale"]["z"].asFloat();
			iter.vecScale.push_back(iter2);
		}

		m_vecSEffect.emplace_back(iter);
	}
}
