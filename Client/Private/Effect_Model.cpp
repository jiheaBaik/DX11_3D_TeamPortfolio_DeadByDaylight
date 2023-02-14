#include "pch.h"
#include "Effect_Model.h"

CEffect_Model* const CEffect_Model::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEffect_Model* pInstnace = new CEffect_Model(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEffect_Model::Clone(void* const& pArg) {
	CEffect_Model* pInstnace = new CEffect_Model(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect_Model::CEffect_Model(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEffect_Model::CEffect_Model(const CEffect_Model& rhs)
	: CObject(rhs) {
}

void CEffect_Model::Delete(void) {
	__super::Delete();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pCollider);
	for (auto& iter : m_vecEffect) {
		Safe_Release(iter.pModel1);
		Safe_Release(iter.pModel2);
		Safe_Release(iter.pModel3);
		Safe_Release(iter.pModel4);
		Safe_Release(iter.pTempTex);
		Safe_Release(iter.pDestTex);
		Safe_Release(iter.pTransform);
	}
	m_vecEffect.clear();
}

const HRESULT CEffect_Model::Init_Create(void) {
	return S_OK;
}

const HRESULT CEffect_Model::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelEffect"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return hr;
}

const _ubyte CEffect_Model::Update(const _double& dTimeDelta) {
	if (m_bCreate) {
		if (m_bStart) {
			m_dMainTime += dTimeDelta;
			for (auto& iter : m_vecEffect) {
				if (m_bRepeat && (m_dMainTime > m_fTime)) {
					for (auto& iter : m_vecEffect) {
						if (!m_bProgressLoop) {
							iter.fTime = 0.f;
							iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
							if (iter.bRot[0])
								iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
							if (iter.bRot[1])
								iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
							if (iter.bRot[2])
								iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));
							iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
							iter.fAlphaTime = 0.f;
							iter.fAlpha = 0.1f;
							iter.fUV.x = 0;
							iter.fUV.y = iter.fsetUVY;
							iter.iSetFrame = 0;
							iter.vAccel = iter.vSetAccel;
							iter.fResist.x = iter.fResist.y;
							iter.fGa = 0.f;
							iter.bRender = false;
							for (auto& arr : iter.iKeyFrame)
								arr = 0;
						}
					}
					m_dMainTime = 0.;
				}
				else if (!m_bRepeat && (m_dMainTime > m_fTime))
					m_dMainTime = static_cast<_double>(m_fTime);

				if (m_dMainTime < iter.fCreateTime) {
					iter.fAlphaTime = 0.f;
					//iter.fAlpha = 0.1f;
					iter.fUV.x = 0;
					iter.fUV.y = iter.fsetUVY;
					iter.iSetFrame = 0;
					iter.fTime = 0.f;
					iter.bRender = false;
					for (auto& arr : iter.iKeyFrame)
						arr = 0;
					continue;
				}

				if (m_dMainTime >= iter.fCreateTime) {
					_float ratioTime;
					iter.bRender = true;
					iter.fTime += static_cast<_float>(dTimeDelta);
					iter.fFrameTime += static_cast<_float>(dTimeDelta);
					ratioTime = iter.fTime;
					if (iter.fTime > iter.fDeleteTime) {
						//iter.fTime = iter.fDeleteTime;
						iter.bRender = false;
					}
					if (m_bProgressLoop) {
						if (iter.fTime >= iter.fDeleteTime) {
							if (m_bRepeat) {
								iter.fTime = 0.f;
								iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
								if (iter.bRot[0])
									iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
								if (iter.bRot[1])
									iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
								if (iter.bRot[2])
									iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));
								iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
								iter.fAlphaTime = 0.f;
								iter.fAlpha = 0.1f;
								iter.fUV.x = 0;
								iter.fUV.y = iter.fsetUVY;
								iter.iSetFrame = 0;
								iter.vAccel = iter.vSetAccel;
								iter.fResist.x = iter.fResist.y;
								iter.fGa = 0.f;
								iter.bRender = false;
								for (auto& arr : iter.iKeyFrame)
									arr = 0;
							}
							else {
								ratioTime = m_fTime;
								iter.fTime = m_fTime;
							}
						}
					}
					if (iter.bMove) {
						_float fRatio = ratioTime / (iter.fDeleteTime);
						if (isnan(fRatio))
							fRatio = 0.f;
						if (isinf(fRatio))
							fRatio = 0.f;
						_vector vPos = iter.pTransform->Get_Row(CTransform::ERow::POS);
						if (!iter.bDirType)
							vPos = XMVectorLerp(XMLoadFloat3(&iter.fPos), XMLoadFloat3(&iter.fEndPos), fRatio);
						else
							vPos += XMLoadFloat3(&iter.fDir) * static_cast<_float>(dTimeDelta);

						iter.pTransform->Set_Row(CTransform::ERow::POS, vPos);

						_float3 vRot;
						vRot.x = fRatio * iter.fEndRot.x + (1 - fRatio) * iter.fRot.x;
						vRot.y = fRatio * iter.fEndRot.y + (1 - fRatio) * iter.fRot.y;
						vRot.z = fRatio * iter.fEndRot.z + (1 - fRatio) * iter.fRot.z;

						/*_float3 scale = iter.pTransform->Get_Scale();
						_vector right = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
						_vector up = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::UP));
						_vector look = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::LOOK));
						_matrix matrixRotX = XMMatrixRotationAxis(right, XMConvertToRadians(vRot.x));
						_matrix matrixRotY = XMMatrixRotationAxis(up, XMConvertToRadians(vRot.y));
						_matrix matrixRotZ = XMMatrixRotationAxis(look, XMConvertToRadians(vRot.z));
						_matrix matrixRot = matrixRotX * matrixRotY * matrixRotZ;


						iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRot));
						iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRot));
						iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRot));*/

						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(iter.fEndRot.x), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::UP), XMConvertToRadians(iter.fEndRot.y), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(iter.fEndRot.z), dTimeDelta);

						if (iter.vecFrameScale.empty()) {
							_vector vScale = XMVectorLerp(XMLoadFloat3(&iter.fScale), XMLoadFloat3(&iter.fEndScale), fRatio);
							iter.pTransform->Set_Scale(vScale);
						}
						else {
							if ((iter.fTime >= iter.vecFrameScale.back().first) || iter.vecFrameScale.size() == 1) {
								iter.fScale = iter.vecFrameScale.back().second;
							}
							else {
								while (iter.fTime > iter.vecFrameScale[iter.iKeyFrame[3] + 1].first)
									++iter.iKeyFrame[3];
								_vector vSour = XMLoadFloat3(&iter.vecFrameScale[iter.iKeyFrame[3]].second);
								_vector vDest = XMLoadFloat3(&iter.vecFrameScale[iter.iKeyFrame[3] + 1].second);
								_float fScaleRatio = static_cast<_float>((iter.fTime - iter.vecFrameScale[iter.iKeyFrame[3]].first) / (iter.vecFrameScale[iter.iKeyFrame[3] + 1].first - iter.vecFrameScale[iter.iKeyFrame[3]].first));
								_vector vScale = XMVectorLerp(vSour, vDest, fScaleRatio);
								iter.pTransform->Set_Scale(vScale);
							}
						}
					}

				
					if (iter.bPhysics) {
						_float fRatio = ratioTime / (iter.fDeleteTime);
						if (isnan(fRatio))
							fRatio = 0.f;
						if (isinf(fRatio))
							fRatio = 0.f;

						_vector pos = iter.pTransform->Get_Row(CTransform::ERow::POS);

						iter.fResist.x *= iter.fResistP;
						_float resist = 1 - (iter.fResist.x / iter.fResist.y);
						_float resistratio = (fRatio + resist) / 2;
						if (isnan(resist))
							resist = 1.f;
						_vector ResiAccel = (XMLoadFloat3(&iter.vAccel)) * resist; 
						_vector vAccel = XMLoadFloat3(&iter.vAccel);
						vAccel -= ResiAccel; 
						pos += vAccel * static_cast<_float>(dTimeDelta);
						iter.pTransform->Set_Row(CTransform::ERow::POS, pos);

						_float3 vRot;
						vRot.x = fRatio * iter.fEndRot.x + (1 - fRatio) * iter.fRot.x;
						vRot.y = fRatio * iter.fEndRot.y + (1 - fRatio) * iter.fRot.y;
						vRot.z = fRatio * iter.fEndRot.z + (1 - fRatio) * iter.fRot.z;
						if (iter.bRot[0])
							iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vRot.x));
						if (iter.bRot[1])
							iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vRot.y));
						if (iter.bRot[2])
							iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(vRot.z));

						_vector vScale = XMVectorLerp(XMLoadFloat3(&iter.fScale), XMLoadFloat3(&iter.fEndScale), resistratio);
						iter.pTransform->Set_Scale(vScale);
					}
					if (iter.bGravity) {
						iter.fGa += iter.fGravity * static_cast<_float>(dTimeDelta);
						_float3 vPos;
						XMStoreFloat3(&vPos, iter.pTransform->Get_Row(CTransform::ERow::POS));
						vPos.y -= iter.fGa;
						iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos));
					}

					_float	fTime, fratio;
					fTime = iter.fFrameSpeed;
					if (iter.vecFrameAlpha.empty()) {
						if (iter.iSetFrame >= (_uint)iter.iOtherFrame)
							fTime = iter.fOtherSpeed;

						if (iter.iSetFrame >= (_uint)iter.iAlphaFrame)
							iter.fAlphaTime += static_cast<_float>(dTimeDelta);

						if (iter.fAlphaTime < iter.fDissolveTime) {
							fratio = iter.fAlphaTime / iter.fDissolveTime;
							if (fratio > 1.f)
								fratio = 1.f;
							if (!iter.bAlphaReverse)
								iter.fAlpha = iter.fAlphaSet * (fratio)+iter.fDefaultAlpha * (1 - fratio);
							else
								iter.fAlpha = iter.fAlphaSet * (fratio)+1.1f * (1 - fratio);
							if (isnan(iter.fAlpha))
								iter.fAlpha = 0.1f;
						}
					}
					else {
						if (iter.iSetFrame >= (_uint)iter.iOtherFrame)
							fTime = iter.fOtherSpeed;

						if (iter.iSetFrame >= (_uint)iter.iAlphaFrame)
							iter.fAlphaTime += static_cast<_float>(dTimeDelta);

						if ((iter.fTime >= iter.vecFrameAlpha.back().first) || iter.vecFrameAlpha.size() == 1) {
							iter.fAlpha = iter.vecFrameAlpha.back().second;
						}
						else {
							while (iter.fTime > iter.vecFrameAlpha[iter.iKeyFrame[0] + 1].first)
								++iter.iKeyFrame[0];
							_float vSour = iter.vecFrameAlpha[iter.iKeyFrame[0]].second;
							_float vDest = iter.vecFrameAlpha[iter.iKeyFrame[0] + 1].second;
							_float falphaRatio = static_cast<_float>((iter.fTime - iter.vecFrameAlpha[iter.iKeyFrame[0]].first) / (iter.vecFrameAlpha[iter.iKeyFrame[0] + 1].first - iter.vecFrameAlpha[iter.iKeyFrame[0]].first));
							_float vAlpha = (1 - falphaRatio) * vSour + falphaRatio * vDest;
							iter.fAlpha = vAlpha;
						}
					}

					if (m_bRepeat) {
						if (iter.fFrameTime > fTime) {
							iter.fFrameTime = 0.f;
							++iter.iSetFrame;
							++iter.fUV.x;
							if (iter.fUV.x >= iter.fDivide.x) {
								++iter.fUV.y;
								if (iter.fUV.y >= iter.fDivide.y) {
									iter.fUV.y = iter.fDivide.y - 1;
									iter.fUV.x = iter.fDivide.x - 1;
								}
								else {
									iter.fUV.x = 0.f;
								}
							}
						}
					}
					else {
						if (iter.fFrameTime > fTime) {
							iter.fFrameTime = 0.f;
							++iter.iSetFrame;
							++iter.fUV.x;
							if (iter.fUV.x >= iter.fDivide.x) {
								++iter.fUV.y;
								if (iter.fUV.y >= iter.fDivide.y) {
									iter.fUV.y = iter.fDivide.y - 1;
									iter.fUV.x = iter.fDivide.x - 1;
								}
								else {
									iter.fUV.x = 0;
								}
							}
						}
					}

					if (iter.iFrame > 0) {
						iter.iFrame = 0;
						++iter.fUV.x;
						if (iter.fUV.x >= iter.fDivide.x) {
							++iter.fUV.y;
							if (iter.fUV.y >= iter.fDivide.y) {
								iter.fUV.y = iter.fsetUVY;
								iter.iSetFrame = 0;
								iter.fAlphaTime = 0.f;
							}
							iter.fUV.x = 0;
						}
					}
					if (iter.bFix) {
						iter.fUV.x = iter.fFixUV.x;
						iter.fUV.y = iter.fFixUV.y;
					}
				}
			}
		}
	}
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CEffect_Model::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	if (ImGui::CollapsingHeader("Create")) {
		ImGui::InputText("Model1", m_sEffectModel.szModel1, IM_ARRAYSIZE(m_sEffectModel.szModel1)); ImGui::SameLine();
		if (ImGui::Button("Change_Model1")) {
			if (m_vecEffect.back().pModel1 != nullptr) {
				Safe_Release(m_vecEffect.back().pModel1);
				string str;
				wstring wstr;
				str = m_sEffectModel.szModel1;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Model_") + wstr;
				strcpy_s(m_vecEffect.back().szModel1, m_sEffectModel.szModel1);
				m_vecEffect.back().pModel1 = dynamic_cast<CModel*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Model2", m_sEffectModel.szModel2, IM_ARRAYSIZE(m_sEffectModel.szModel2)); ImGui::SameLine();
		if (ImGui::Button("Change_Model2")) {
			if (m_vecEffect.back().pModel2 != nullptr) {
				Safe_Release(m_vecEffect.back().pModel2);
				string str;
				wstring wstr;
				str = m_sEffectModel.szModel2;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Model_") + wstr;
				strcpy_s(m_vecEffect.back().szModel2, m_sEffectModel.szModel2);
				m_vecEffect.back().pModel2 = dynamic_cast<CModel*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Model3", m_sEffectModel.szModel3, IM_ARRAYSIZE(m_sEffectModel.szModel3)); ImGui::SameLine();
		if (ImGui::Button("Change_Model3")) {
			if (m_vecEffect.back().pModel3 != nullptr) {
				Safe_Release(m_vecEffect.back().pModel3);
				string str;
				wstring wstr;
				str = m_sEffectModel.szModel2;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Model_") + wstr;
				strcpy_s(m_vecEffect.back().szModel3, m_sEffectModel.szModel3);
				m_vecEffect.back().pModel3 = dynamic_cast<CModel*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Model4", m_sEffectModel.szModel4, IM_ARRAYSIZE(m_sEffectModel.szModel4)); ImGui::SameLine();
		if (ImGui::Button("Change_Model4")) {
			if (m_vecEffect.back().pModel4 != nullptr) {
				Safe_Release(m_vecEffect.back().pModel4);
				string str;
				wstring wstr;
				str = m_sEffectModel.szModel4;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Model_") + wstr;
				strcpy_s(m_vecEffect.back().szModel4, m_sEffectModel.szModel4);
				m_vecEffect.back().pModel4 = dynamic_cast<CModel*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Temp", m_sEffectModel.szTemp, IM_ARRAYSIZE(m_sEffectModel.szTemp)); ImGui::SameLine();
		if (ImGui::Button("Change_tem")) {
			if (m_vecEffect.back().pTempTex != nullptr) {
				Safe_Release(m_vecEffect.back().pTempTex);
				string str;
				wstring wstr;
				str = m_sEffectModel.szTemp;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szTemp, m_sEffectModel.szTemp);
				m_vecEffect.back().pTempTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Dest", m_sEffectModel.szDest, IM_ARRAYSIZE(m_sEffectModel.szDest)); ImGui::SameLine();
		if (ImGui::Button("Change_des")) {
			if (m_vecEffect.back().pDestTex != nullptr) {
				Safe_Release(m_vecEffect.back().pDestTex);
				string str;
				wstring wstr;
				str = m_sEffectModel.szDest;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szDest, m_sEffectModel.szDest);
				m_vecEffect.back().pDestTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		if (ImGui::Button("Create_Effect"))
			Create_Effect(m_sEffectModel);
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Rect")) {
		ImGui::DragFloat("SizeX", &m_fSizeX);
		ImGui::DragFloat("SizeY", &m_fSizeY);

		ImGui::DragFloat("PosX", &m_fX, 1.f, -1280.f, 1280.f);
		ImGui::DragFloat("PosY", &m_fY, 1.f, -720.f, 720.f);
	}
	if (ImGui::CollapsingHeader("Effecttime"))
		ImGui::DragFloat("EndTime", &m_fTime);
	if (ImGui::Button("Play")) {
		m_bStart = !m_bStart;
	}ImGui::SameLine();
	ImGui::Checkbox("LookCam", &m_bLookCam);

	ImGui::Separator();


	ImGui::InputText("Save&Load_Path", m_szSave, IM_ARRAYSIZE(m_szSave));
	string str{ m_szSave };
	wstring swtr{ str.begin(), str.end() };
	if (ImGui::Button("Save")) {
		SaveEffect(swtr.c_str());
	}ImGui::SameLine();
	if (ImGui::Button("Load"))
		LoadEffect(swtr.c_str());

	if (ImGui::Checkbox("Repeat", &m_bRepeat)) {};
	ImGui::Separator();

	_uint i = 0;
	for (auto& iter : m_vecEffect) {
		char szNode[MAX_PATH];
		sprintf_s(szNode, "Node(%d)", i++);
		if (m_bCreate) {
			if (ImGui::TreeNode(szNode)) {
				ImGui::Checkbox("isRenderNow", &iter.bRender);
				ImGui::DragFloat("CreateTime", &iter.fCreateTime, 0.005f, 0.0f);
				ImGui::DragFloat("ProgressTime", &iter.fDeleteTime, 0.01f, 0.f, m_fTime); ImGui::SameLine();
				if (ImGui::Button("AllProgressTime")) {
					for (auto& node : m_vecEffect)
						node.fDeleteTime = iter.fDeleteTime;
				}
				ImGui::DragFloat("time", &iter.fTime);
				ImGui::DragFloat("FrameTime", &iter.fFrameTime);
				ImGui::DragFloat("AlphaTime", &iter.fAlphaTime);
				ImGui::DragFloat2("UV", (_float*)&iter.fUV);
				ImGui::DragFloat2("FixUV", (_float*)&iter.fFixUV);
				ImGui::DragFloat3("CurPos", (_float*)&iter.pTransform->Get_Row(CTransform::ERow::POS));
				if (ImGui::CollapsingHeader("Transform")) {
					if (ImGui::DragFloat3("Pos", (_float*)&iter.fPos, 0.001f))
						iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

					_float3 scale = iter.pTransform->Get_Scale();
					_vector right = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
					_vector up = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::UP));
					_vector look = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::LOOK));
					_matrix matrixRotX = XMMatrixRotationAxis(right, XMConvertToRadians(iter.fRot.x));
					_matrix matrixRotY = XMMatrixRotationAxis(up, XMConvertToRadians(iter.fRot.y));
					_matrix matrixRotZ = XMMatrixRotationAxis(look, XMConvertToRadians(iter.fRot.z));
					if (ImGui::DragFloat("Rotx", &iter.fRot.x, 0.1f)) {}ImGui::SameLine();
					if (ImGui::Button("Rot_X")) {
						iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRotX));
						iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRotX));
						iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRotX));
						iter.pTransform->Set_Scale(XMLoadFloat3(&scale));
					}

					if (ImGui::DragFloat("Roty", &iter.fRot.y, 0.1f)) {}ImGui::SameLine();
					if (ImGui::Button("Rot_Y")) {
						iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRotY));
						iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRotY));
						iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRotY));
						iter.pTransform->Set_Scale(XMLoadFloat3(&scale));
					}

					if (ImGui::DragFloat("Rotz", &iter.fRot.z, 0.1f)) {}ImGui::SameLine();
					if (ImGui::Button("Rot_Z")) {
						iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRotZ));
						iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRotZ));
						iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRotZ));
						iter.pTransform->Set_Scale(XMLoadFloat3(&scale));
					}

					if (ImGui::Button("InitRot")) {
						iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
						iter.pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
						iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
						iter.pTransform->Set_Scale(XMLoadFloat3(&scale));
						iter.fRot = _float3(0.f, 0.f, 0.f);
					}
					
					ImGui::DragFloat3("Scale", (_float*)&iter.fScale, 0.001f);
					if (ImGui::Button("AllScale")) {
						_float3 endscale = iter.fScale;
						for (auto& iter : m_vecEffect)
							iter.fScale = endscale;
					}
				}
				if (ImGui::CollapsingHeader("EndTransform")) {
					if (ImGui::Checkbox("Move", &iter.bMove)) {}
					if (iter.bMove) {
						ImGui::DragFloat3("EndPos", (_float*)&iter.fEndPos,0.001f);
						ImGui::DragFloat3("EndRot", (_float*)&iter.fEndRot, 0.01f);
						ImGui::DragFloat3("EndScale", (_float*)&iter.fEndScale, 0.001f);
						int d = 0;
						if (ImGui::TreeNode("ScaleKey")) {
							for (auto frame = iter.vecFrameScale.begin(); frame != iter.vecFrameScale.end(); ++frame) {
								char szFrame[MAX_PATH];
								sprintf_s(szFrame, "Frame(%d)", d++);
								if (ImGui::TreeNode(szFrame)) {
									if (ImGui::InputFloat("Time", &frame->first)) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											_float fir = frame->first;
											for (auto& node : m_vecEffect) {
												node.vecFrameScale[d - 1].first = fir;
											}
										}
									}
									if (ImGui::InputFloat3("Scale", (_float*)&frame->second)) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											_float3 sec = frame->second;
											for (auto& node : m_vecEffect) {
												node.vecFrameScale[d - 1].second = sec;
											}
										}
									}

									if (ImGui::Button("Erase")) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											for (auto& node : m_vecEffect) {
												if (&node == &iter)
													continue;
												node.vecFrameScale.erase(node.vecFrameScale.begin() + d - 1);
											}
										}
										else {
											ImGui::TreePop();
											iter.iKeyFrame[3] = 0;
											iter.vecFrameScale.erase(frame);
											break;
										}
									}
									ImGui::TreePop();
								}
							}
							if (ImGui::Button("Create")) {
								if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
									for (auto& node : m_vecEffect)
										node.vecFrameScale.emplace_back();
								}
								else
									iter.vecFrameScale.emplace_back();
							}
							ImGui::TreePop();
						}
						ImGui::Checkbox("DirType", &iter.bDirType);
						ImGui::DragFloat3("Dir", (_float*)&iter.fDir, 0.001f);
					}
				}
				if (ImGui::CollapsingHeader("Physics")) {
					ImGui::Checkbox("PhysicsMove", &iter.bPhysics);
					if (iter.bPhysics) {
						if (ImGui::DragFloat3("Accel", (_float*)&iter.vSetAccel)) {
							iter.vAccel = iter.vSetAccel;
						}
						ImGui::DragFloat2("Resist", (_float*)&iter.fResist);
						ImGui::DragFloat("ResistPower", (_float*)&iter.fResistP, 0.001f, 0.f, 1.f);

						ImGui::DragFloat3("EndRot", (_float*)&iter.fEndRot);
						ImGui::DragFloat3("EndScale", (_float*)&iter.fEndScale);
					}
					ImGui::Checkbox("isGravity", &iter.bGravity);
					if (iter.bGravity)
						ImGui::DragFloat("Gravity", &iter.fGravity);
				}
				if (ImGui::CollapsingHeader("Shader")) {
					ImGui::DragFloat("Speed", &iter.fFrameSpeed);
					ImGui::DragFloat2("UV", (_float*)&iter.fDivide, 1.f, 0.f, 10.f); ImGui::SameLine();
					ImGui::Checkbox("Fix", &iter.bFix);
					if (iter.bFix)
						ImGui::DragFloat2("FixUV", (_float*)&iter.fFixUV);
					ImGui::DragInt("SelectFrame", &iter.iOtherFrame);
					ImGui::DragFloat("SelectSpeed", &iter.fOtherSpeed);
					ImGui::DragInt("SelectAlphaFrame", &iter.iAlphaFrame);
					ImGui::DragFloat("SelectAlphaSet", &iter.fAlphaSet);
					ImGui::Checkbox("AlphaReverse", &iter.bAlphaReverse);
					ImGui::DragFloat("DissolveTime", (_float*)&iter.fDissolveTime);
					int j = 0;
					if (ImGui::TreeNode("alphaKey")) {
						for (auto frame = iter.vecFrameAlpha.begin(); frame != iter.vecFrameAlpha.end(); ++frame) {
							char szFrame[MAX_PATH];
							sprintf_s(szFrame, "Frame(%d)", j++);
							if (ImGui::TreeNode(szFrame)) {
								if (ImGui::InputFloat("Time", &frame->first)) {
									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
										_float fir = frame->first;
										for (auto& node : m_vecEffect) {
											node.vecFrameAlpha[j - 1].first = fir;
										}
									}
								}
								if (ImGui::InputFloat("alpha", (_float*)&frame->second)) {
									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
										_float sec = frame->second;
										for (auto& node : m_vecEffect) {
											node.vecFrameAlpha[j - 1].second = sec;
										}
									}
								}

								if (ImGui::Button("Erase")) {
									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
										_float fir = iter.vecFrameAlpha.begin()->first;
										_float sec = iter.vecFrameAlpha.begin()->second;
										for (auto& node : m_vecEffect) {
											for (auto frame = node.vecFrameAlpha.begin(); frame != node.vecFrameAlpha.end(); ++frame) {
												frame->first = fir;
												frame->second = sec;
											}
										}
									}
									else {
										ImGui::TreePop();
										iter.iKeyFrame[0] = 0;
										iter.vecFrameAlpha.erase(frame);
										break;
									}
								}
								ImGui::TreePop();
							}
						}
						if (ImGui::Button("Create")) {
							if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
								for (auto& node : m_vecEffect)
									node.vecFrameAlpha.emplace_back();
							}
							else
								iter.vecFrameAlpha.emplace_back();
						}
						ImGui::TreePop();
					}
					ImGui::DragFloat4("Color", (_float*)&iter.fColor,0.001f); ImGui::SameLine();
					ImGui::Checkbox("SetColor", &iter.bSetColor);
				}
				if (ImGui::CollapsingHeader("Smoke")) {
					ImGui::DragFloat3("ShaderScale", (_float*)&iter.fShadeScale, 0.1f);
					ImGui::DragFloat3("ScrollSpeed", (_float*)&iter.fScrollSpeeds, 0.1f);
					ImGui::DragFloat("SmokeAlpha", (_float*)&iter.fSmokeAlpha, 0.05f);
					ImGui::DragFloat2("Distortion1", (_float*)&iter.fDistortion1, 0.1f);
					ImGui::DragFloat2("Distortion2", (_float*)&iter.fDistortion2, 0.1f);
					ImGui::DragFloat2("Distortion3", (_float*)&iter.fDistortion3, 0.1f);
					ImGui::DragFloat2("Distortion4", (_float*)&iter.fDistortion4, 0.1f);
					ImGui::DragFloat("DiscardAlpha", (_float*)&iter.fDiscardAlpha, 0.05f, 0.f, 1.1f);
				}
				
				const char* iTexture[] = { "Pass_Default", "Pass_Frame","Pass_UnNormalFrame" ,"Pass_OtherTexFrame","Pass_OtherTexSetColorFrame", "Pass_BloodSplainRG", "Pass_BloodSplainR",
										"Pass_AlphatestColorBlood","Pass_BloodParticle","Pass_FootBlood", "Pass_FootBlood_N","Pass_ColorAMesh","Pass_RainDrop"};
				_int pass = static_cast<_int>(iter.iShaderPass);
				ImGui::Combo("TexturePass", &pass, iTexture, IM_ARRAYSIZE(iTexture));
				iter.iShaderPass = static_cast<_uint>(pass);
				if (ImGui::Button("AllPass")) {
					for (auto& node : m_vecEffect)
						node.iShaderPass = iter.iShaderPass;
				}
				
				const char* items[] = { "PRIORITY", "NONALPHABLEND", "DECAL", "DISTORTION", "SHADOW", "SHADOW_BAKE", "NONLIGHT", "GLOW", "ALPHABLEND", "PENETRATE", "UI" };
				ImGui::Combo("RENDERER", &m_iRenderIdx, items, IM_ARRAYSIZE(items));

				if (ImGui::Button("Delete")) {
					Safe_Release(iter.pTransform);
					Safe_Release(iter.pModel1);
					Safe_Release(iter.pModel2);
					Safe_Release(iter.pModel3);
					Safe_Release(iter.pModel4);
					Safe_Release(iter.pTempTex);
					Safe_Release(iter.pDestTex);
					m_vecEffect.erase(m_vecEffect.begin() + i - 1);
				}

				ImGui::TreePop();
			}
		}
	}
}

void CEffect_Model::Late_Update(const _double& dTimeDelta) {
	if (nullptr == m_pRenderer)
		return;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	_vector vCameraPos = pCamera->Get_Pos();
	for (auto& iter : m_vecEffect) {
		if (m_bLookCam) {
			_float3 scale = m_pTransform->Get_Scale();
			_vector vLook = XMVector3Normalize((m_pTransform->Get_Matrix()).r[3] - vCameraPos);
			XMVectorSetY(vLook, 0.f);
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, vRight);
			m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
			m_pTransform->Set_Row(CTransform::ERow::LOOK, vLook);
			m_pTransform->Set_Scale(XMLoadFloat3(&scale));
		}
	}

	m_pRenderer->Add_Object(static_cast<CRenderer::EOrder>(m_iRenderIdx), this);
}

const HRESULT CEffect_Model::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	pCamera->Set_RawValue(m_pShader);
	//m_pCollider->Render(pCamera);
	for (auto& iter : m_vecEffect) {
		if (iter.bRender) {
			switch (eOrder) {
			case CRenderer::EOrder::ALPHABLEND: {
				if (m_pShader == nullptr)
					return hr;
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= m_pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_bReverse", &iter.bAlphaReverse, sizeof(_bool));
				hr |= m_pShader->Set_RawValue("g_bSetColor", &iter.bSetColor, sizeof(_bool));
				if (m_bCreate) {
					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel1->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel1->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2 != nullptr) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel2->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel2->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel3 != nullptr) {
						for (_uint j = 0; j < iter.pModel3->Get_NumMesh(); ++j) {
							iter.pModel3->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel3->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel3->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel4 != nullptr) {
						for (_uint j = 0; j < iter.pModel4->Get_NumMesh(); ++j) {
							iter.pModel4->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel4->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel4->Render(j, m_pShader, iter.iShaderPass);
						}
					}

					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
											  break;
			case CRenderer::EOrder::GLOW: {
				if (m_pShader == nullptr)
					return hr;
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= m_pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_bReverse", &iter.bAlphaReverse, sizeof(_bool));
				hr |= m_pShader->Set_RawValue("g_bSetColor", &iter.bSetColor, sizeof(_bool));
				if (m_bCreate) {
					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel1->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel1->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2 != nullptr) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel2->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel2->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel3 != nullptr) {
						for (_uint j = 0; j < iter.pModel3->Get_NumMesh(); ++j) {
							iter.pModel3->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel3->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel3->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel4 != nullptr) {
						for (_uint j = 0; j < iter.pModel4->Get_NumMesh(); ++j) {
							iter.pModel4->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel4->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel4->Render(j, m_pShader, iter.iShaderPass);
						}
					}

					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
											  break;
			case CRenderer::EOrder::NONALPHABLEND: {
				if (m_pShader == nullptr)
					return hr;
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= m_pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_bReverse", &iter.bAlphaReverse, sizeof(_bool));
				hr |= m_pShader->Set_RawValue("g_bSetColor", &iter.bSetColor, sizeof(_bool));
				if (m_bCreate) {
					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel1->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel1->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel2->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel2->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel3) {
						for (_uint j = 0; j < iter.pModel3->Get_NumMesh(); ++j) {
							iter.pModel3->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel3->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel3->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel4) {
						for (_uint j = 0; j < iter.pModel4->Get_NumMesh(); ++j) {
							iter.pModel4->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel4->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel4->Render(j, m_pShader, iter.iShaderPass);
						}
					}

					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
												 break;
			case CRenderer::EOrder::NONLIGHT: {
				if (m_pShader == nullptr)
					return hr;
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= m_pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= m_pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= m_pShader->Set_RawValue("g_bReverse", &iter.bAlphaReverse, sizeof(_bool));
				hr |= m_pShader->Set_RawValue("g_bSetColor", &iter.bSetColor, sizeof(_bool));
				if (m_bCreate) {
					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel1->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel1->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel2->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel2->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel3) {
						for (_uint j = 0; j < iter.pModel3->Get_NumMesh(); ++j) {
							iter.pModel3->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel3->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel3->Render(j, m_pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel4) {
						for (_uint j = 0; j < iter.pModel4->Get_NumMesh(); ++j) {
							iter.pModel4->Set_Resource(j, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
							iter.pModel4->Set_Resource(j, aiTextureType_NORMALS, m_pShader, "g_NormalTexture");
							iter.pModel4->Render(j, m_pShader, iter.iShaderPass);
						}
					}

					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
											break;
			}
		}
	}
	return hr;
}

const HRESULT CEffect_Model::Create_Effect(EffectModel sEffect) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	EffectModel seffecttex;
	HRESULT hr = S_OK;

	string m1{ sEffect.szModel1 };
	string m2{ sEffect.szModel2 };
	string m3{ sEffect.szModel3 };
	string m4{ sEffect.szModel4 };
	string tmp{ sEffect.szTemp };
	string dst{ sEffect.szDest };
	wstring wm1{ m1.begin(), m1.end() };
	wstring wm2{ m2.begin(), m2.end() };
	wstring wm3{ m3.begin(), m3.end() };
	wstring wm4{ m4.begin(), m4.end() };
	wstring wtmp{ tmp.begin(), tmp.end() };
	wstring wdst{ dst.begin(), dst.end() };
	wm1 = TEXT("Model_") + wm1;
	wm2 = TEXT("Model_") + wm2;
	wm3 = TEXT("Model_") + wm3;
	wm4 = TEXT("Model_") + wm4;
	wtmp = TEXT("Texture_") + wtmp;
	wdst = TEXT("Texture_") + wdst;

	wsprintf(m_TransformKey[m_iEffectIdx], TEXT("Transform%d"), m_iEffectIdx);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), m_TransformKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pTransform));
	wsprintf(m_Model1Key[m_iEffectIdx], TEXT("Model1_%d"), m_iEffectIdx);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), wm1.c_str(), m_Model1Key[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pModel1));
	wsprintf(m_Model2Key[m_iEffectIdx], TEXT("Model2_%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), wm2.c_str(), m_Model2Key[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pModel2));
	wsprintf(m_Model3Key[m_iEffectIdx], TEXT("Model3_%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), wm3.c_str(), m_Model3Key[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pModel3));
	wsprintf(m_Model4Key[m_iEffectIdx], TEXT("Model4_%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), wm4.c_str(), m_Model4Key[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pModel4));
	wsprintf(m_TempKey[m_iEffectIdx], TEXT("TempTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wtmp.c_str(), m_TempKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pTempTex));
	wsprintf(m_DestKey[m_iEffectIdx], TEXT("DestTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wdst.c_str(), m_DestKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pDestTex));

	++m_iEffectIdx;

	if (hr != E_FAIL) {
		m_bCreate = true;
		_char* mo1 = sEffect.szModel1;
		_char* mo2 = sEffect.szModel2;
		_char* mo3 = sEffect.szModel3;
		_char* mo4 = sEffect.szModel4;
		_char* temp = sEffect.szTemp;
		_char* dest = sEffect.szDest;
		m_sEffectModel = seffecttex;
		strcpy_s(m_sEffectModel.szModel1, mo1);
		strcpy_s(m_sEffectModel.szModel2, mo2);
		strcpy_s(m_sEffectModel.szModel3, mo3);
		strcpy_s(m_sEffectModel.szModel4, mo4);
		strcpy_s(m_sEffectModel.szTemp, temp);
		strcpy_s(m_sEffectModel.szDest, dest);
		m_vecEffect.push_back(m_sEffectModel);
	}
	else {
		Safe_Release(seffecttex.pModel1);
		Safe_Release(seffecttex.pModel2);
		Safe_Release(seffecttex.pModel3);
		Safe_Release(seffecttex.pModel4);
		Safe_Release(seffecttex.pTempTex);
		Safe_Release(seffecttex.pDestTex);
		Safe_Release(seffecttex.pTransform);
	}
	return hr;
}

void CEffect_Model::SaveEffect(const _tchar* const& pFilePath) {
	Json::Value root;
	root["EndTime"] = m_fTime;
	root["Loop"] = m_bRepeat;
	root["ProgressLoop"] = m_bProgressLoop;
	root["sizeX"] = m_fSizeX;
	root["sizeY"] = m_fSizeY;
	root["PosX"] = m_fX;
	root["PosY"] = m_fY;
	root["LookCam"] = m_bLookCam;

	for (auto& iter : m_vecEffect) {
		Json::Value node;

		node["Model"][0] = string{ iter.szModel1 };
		node["Model"][1] = string{ iter.szModel2 };
		node["Model"][2] = string{ iter.szModel3 };
		node["Model"][3] = string{ iter.szModel4 };
		node["Texture"][0] = string{ iter.szTemp };
		node["Texture"][1] = string{ iter.szDest };

		node["CreateTime"] = iter.fCreateTime;
		node["DeleteTime"] = iter.fDeleteTime;
		node["Pos"]["x"] = iter.fPos.x;
		node["Pos"]["y"] = iter.fPos.y;
		node["Pos"]["z"] = iter.fPos.z;
		node["EndPos"]["x"] = iter.fEndPos.x;
		node["EndPos"]["y"] = iter.fEndPos.y;
		node["EndPos"]["z"] = iter.fEndPos.z;
		node["DirType"] = iter.bDirType;
		node["Dir"]["x"] = iter.fDir.x;
		node["Dir"]["y"] = iter.fDir.y;
		node["Dir"]["z"] = iter.fDir.z;

		node["Rot"]["x"] = iter.fRot.x;
		node["Rot"]["y"] = iter.fRot.y;
		node["Rot"]["z"] = iter.fRot.z;
		node["EndRot"]["x"] = iter.fEndRot.x;
		node["EndRot"]["y"] = iter.fEndRot.y;
		node["EndRot"]["z"] = iter.fEndRot.z;

		node["Scale"]["x"] = iter.fScale.x;
		node["Scale"]["y"] = iter.fScale.y;
		node["Scale"]["z"] = iter.fScale.z;
		node["EndScale"]["x"] = iter.fEndScale.x;
		node["EndScale"]["y"] = iter.fEndScale.y;
		node["EndScale"]["z"] = iter.fEndScale.z;
		node["Move"] = iter.bMove;

		node["PhysicsMove"] = iter.bPhysics;
		node["Accel"]["x"] = iter.vAccel.x;
		node["Accel"]["y"] = iter.vAccel.y;
		node["Accel"]["z"] = iter.vAccel.z;
		node["SetAccel"]["x"] = iter.vSetAccel.x;
		node["SetAccel"]["y"] = iter.vSetAccel.y;
		node["SetAccel"]["z"] = iter.vSetAccel.z;
		node["Resist"]["x"] = iter.fResist.x;
		node["Resist"]["y"] = iter.fResist.y;
		node["ResistPower"] = iter.fResistP;
		node["isGravity"] = iter.bGravity;
		node["Gravity"] = iter.fGravity;

		for (auto& iter2 : iter.vecFrameAlpha) {
			Json::Value frame;
			frame["FrameTime"] = iter2.first;
			frame["Alpha"] = iter2.second;
			node["vecAlpha"].append(frame);
		}
		for (auto& iter2 : iter.vecFrameScale) {
			Json::Value frame;
			frame["FrameTime"] = iter2.first;
			frame["Scale"]["x"] = iter2.second.x;
			frame["Scale"]["y"] = iter2.second.y;
			frame["Scale"]["z"] = iter2.second.z;
			node["vecScale"].append(frame);
		}
		node["ShaderPass"] = iter.iShaderPass;
		node["ShaderColor"]["x"] = iter.fColor.x;
		node["ShaderColor"]["y"] = iter.fColor.y;
		node["ShaderColor"]["z"] = iter.fColor.z;
		node["ShaderColor"]["w"] = iter.fColor.w;
		node["AlphaFrame"] = iter.iAlphaFrame;
		node["AlphaValue"] = iter.fAlphaSet;
		node["DissolveTime"] = iter.fDissolveTime;
		node["RenderIndex"] = m_iRenderIdx;
		node["ShaderScale"]["x"] = iter.fShadeScale.x;
		node["ShaderScale"]["y"] = iter.fShadeScale.y;
		node["ShaderScale"]["z"] = iter.fShadeScale.z;
		node["ScrollSpeed"]["x"] = iter.fScrollSpeeds.x;
		node["ScrollSpeed"]["y"] = iter.fScrollSpeeds.y;
		node["ScrollSpeed"]["z"] = iter.fScrollSpeeds.z;
		node["Distortion"][0]["x"] = iter.fDistortion1.x;
		node["Distortion"][0]["y"] = iter.fDistortion1.y;
		node["Distortion"][1]["x"] = iter.fDistortion2.x;
		node["Distortion"][1]["y"] = iter.fDistortion2.y;
		node["Distortion"][2]["x"] = iter.fDistortion3.x;
		node["Distortion"][2]["y"] = iter.fDistortion3.y;
		node["Distortion"][3]["x"] = iter.fDistortion4.x;
		node["Distortion"][3]["y"] = iter.fDistortion4.y;
		node["DiscardAlpha"] = iter.fDiscardAlpha;
		node["ReverseAlpha"] = iter.bAlphaReverse;
		node["SetColor"] = iter.bSetColor;
		node["Divide"]["x"] = iter.fDivide.x;
		node["Divide"]["y"] = iter.fDivide.y;
		node["Fix"] = iter.bFix;
		node["FixUV"]["x"] = iter.fFixUV.x;
		node["FixUV"]["y"] = iter.fFixUV.y;
		node["FrameSpeed"] = iter.fFrameSpeed;
		node["OtherSpeedFrame"] = iter.iOtherFrame;
		node["OtherSpeed"] = iter.fOtherSpeed;

		node["Right"]["x"] = XMVectorGetX(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
		node["Right"]["y"] = XMVectorGetY(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
		node["Right"]["z"] = XMVectorGetZ(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
		node["Up"]["x"] = XMVectorGetX(iter.pTransform->Get_Row(CTransform::ERow::UP));
		node["Up"]["y"] = XMVectorGetY(iter.pTransform->Get_Row(CTransform::ERow::UP));
		node["Up"]["z"] = XMVectorGetZ(iter.pTransform->Get_Row(CTransform::ERow::UP));
		node["Look"]["x"] = XMVectorGetX(iter.pTransform->Get_Row(CTransform::ERow::LOOK));
		node["Look"]["y"] = XMVectorGetY(iter.pTransform->Get_Row(CTransform::ERow::LOOK));
		node["Look"]["z"] = XMVectorGetZ(iter.pTransform->Get_Row(CTransform::ERow::LOOK));

		root["Node"].append(node);
	}

	wstring wname(&pFilePath[0]);
	string path = "../Bin/Resource/Effect/testeffect/";
	string pathdot = ".txt";
	wstring wpath = { path.begin(),path.end() };
	wstring wpathdot = { pathdot.begin(),pathdot.end() };
	wstring wfullpath = wpath + wname + wpathdot;

	ofstream ofs(wfullpath.c_str());
	Json::StyledWriter sw;
	string str = sw.write(root);
	ofs << str;
	ofs.close();
}

void CEffect_Model::LoadEffect(const _tchar* const& pFilePath) {
	wstring wname(&pFilePath[0]);
	string path = "../Bin/Resource/Effect/testeffect/";
	string pathdot = ".txt";
	wstring wpath = { path.begin(),path.end() };
	wstring wpathdot = { pathdot.begin(),pathdot.end() };
	wstring wfullpath = wpath + wname + wpathdot;

	ifstream ifs(wfullpath.c_str());
	if (ifs.fail())
		return;	Json::Value root;
	ifs >> root;
	ifs.close();

	m_fTime = root["EndTime"].asFloat();
	m_bRepeat = root["Loop"].asBool();
	m_fSizeX = root["sizeX"].asFloat();
	m_fSizeY = root["sizeY"].asFloat();
	m_fX = root["PosX"].asFloat();
	m_fY = root["PosY"].asFloat();
	m_bLookCam = root["LookCam"].asBool();

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	EffectModel iter;
	for (auto& node : root["Node"]) {
		iter = EffectModel{};
		strcpy_s(iter.szModel1, node["Model"][0].asString().c_str());
		strcpy_s(iter.szModel2, node["Model"][1].asString().c_str());
		strcpy_s(iter.szModel3, node["Model"][2].asString().c_str());
		strcpy_s(iter.szModel4, node["Model"][3].asString().c_str());
		strcpy_s(iter.szTemp, node["Texture"][0].asString().c_str());
		strcpy_s(iter.szDest, node["Texture"][1].asString().c_str());

		iter.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
		string str;
		wstring wstr;
		str = iter.szModel1;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Model_") + wstr;
		iter.pModel1 = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str(), nullptr));
		str = iter.szModel2;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Model_") + wstr;
		iter.pModel2 = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str(), nullptr));
		str = iter.szModel3;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Model_") + wstr;
		iter.pModel3 = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str(), nullptr));
		str = iter.szModel4;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Model_") + wstr;
		iter.pModel4 = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str(), nullptr));
		str = iter.szTemp;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Texture_") + wstr;
		iter.pTempTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
		str = iter.szDest;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Texture_") + wstr;
		iter.pDestTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));


		// time
		iter.fCreateTime = node["CreateTime"].asFloat();
		iter.fDeleteTime = node["DeleteTime"].asFloat();

		// pos
		iter.fPos.x = node["Pos"]["x"].asFloat();
		iter.fPos.y = node["Pos"]["y"].asFloat();
		iter.fPos.z = node["Pos"]["z"].asFloat();
		iter.fEndPos.x = node["EndPos"]["x"].asFloat();
		iter.fEndPos.y = node["EndPos"]["y"].asFloat();
		iter.fEndPos.z = node["EndPos"]["z"].asFloat();
		iter.bDirType = node["DirType"].asBool();
		iter.fDir.x = node["Dir"]["x"].asFloat();
		iter.fDir.y = node["Dir"]["y"].asFloat();
		iter.fDir.z = node["Dir"]["z"].asFloat();
		// rot
		iter.fRot.x = node["Rot"]["x"].asFloat();
		iter.fRot.y = node["Rot"]["y"].asFloat();
		iter.fRot.z = node["Rot"]["z"].asFloat();
		iter.fEndRot.x = node["EndRot"]["x"].asFloat();
		iter.fEndRot.y = node["EndRot"]["y"].asFloat();
		iter.fEndRot.z = node["EndRot"]["z"].asFloat();
		// scale
		iter.fScale.x = node["Scale"]["x"].asFloat();
		iter.fScale.y = node["Scale"]["y"].asFloat();
		iter.fScale.z = node["Scale"]["z"].asFloat();
		iter.fEndScale.x = node["EndScale"]["x"].asFloat();
		iter.fEndScale.y = node["EndScale"]["y"].asFloat();
		iter.fEndScale.z = node["EndScale"]["z"].asFloat();
		//physics
		iter.bPhysics = node["PhysicsMove"].asBool();
		iter.vAccel.x = node["Accel"]["x"].asFloat();
		iter.vAccel.y = node["Accel"]["y"].asFloat();
		iter.vAccel.z = node["Accel"]["z"].asFloat();
		iter.vSetAccel.x = node["SetAccel"]["x"].asFloat();
		iter.vSetAccel.y = node["SetAccel"]["y"].asFloat();
		iter.vSetAccel.z = node["SetAccel"]["z"].asFloat();
		iter.fResist.x = node["Resist"]["x"].asFloat();
		iter.fResist.y = node["Resist"]["y"].asFloat();
		iter.fResistP = node["ResistPower"].asFloat();
		iter.bGravity = node["isGravity"].asBool();
		iter.fGravity = node["Gravity"].asFloat();

		//Alpha
		for (auto& frame : node["vecAlpha"]) {
			pair<_float, _float> iter2;
			iter2.first = frame["FrameTime"].asFloat();
			iter2.second = frame["Alpha"].asFloat();
			iter.vecFrameAlpha.push_back(iter2);
		}
		for (auto& frame : node["vecScale"]) {
			pair<_float, _float3> iter2;
			iter2.first = frame["FrameTime"].asFloat();
			iter2.second.x = frame["Scale"]["x"].asFloat();
			iter2.second.y = frame["Scale"]["y"].asFloat();
			iter2.second.z = frame["Scale"]["z"].asFloat();
			iter.vecFrameScale.push_back(iter2);
		}

		iter.fDivide.x = node["Divide"]["x"].asFloat();
		iter.fDivide.y = node["Divide"]["y"].asFloat();
		iter.bFix = node["Fix"].asBool();
		iter.fFixUV.x = node["FixUV"]["x"].asFloat();
		iter.fFixUV.y = node["FixUV"]["y"].asFloat();
		iter.fFrameSpeed = node["FrameSpeed"].asFloat();
		iter.iOtherFrame = node["OtherSpeedFrame"].asInt();
		iter.fOtherSpeed = node["OtherSpeed"].asFloat();
		iter.bMove = node["Move"].asBool();
		iter.iShaderPass = node["ShaderPass"].asInt();
		iter.fColor.x = node["ShaderColor"]["x"].asFloat();
		iter.fColor.y = node["ShaderColor"]["y"].asFloat();
		iter.fColor.z = node["ShaderColor"]["z"].asFloat();
		iter.fColor.w = node["ShaderColor"]["w"].asFloat();
		iter.iAlphaFrame = node["AlphaFrame"].asInt();
		iter.fAlphaSet = node["AlphaValue"].asFloat();
		iter.fDissolveTime = node["DissolveTime"].asFloat();
		m_iRenderIdx = node["RenderIndex"].asInt();
		iter.fShadeScale.x = node["ShaderScale"]["x"].asFloat();
		iter.fShadeScale.y = node["ShaderScale"]["y"].asFloat();
		iter.fShadeScale.z = node["ShaderScale"]["z"].asFloat();
		iter.fScrollSpeeds.x = node["ScrollSpeed"]["x"].asFloat();
		iter.fScrollSpeeds.y = node["ScrollSpeed"]["y"].asFloat();
		iter.fScrollSpeeds.z = node["ScrollSpeed"]["z"].asFloat();
		iter.fDistortion1.x = node["Distortion"][0]["x"].asFloat();
		iter.fDistortion1.y = node["Distortion"][0]["y"].asFloat();
		iter.fDistortion2.x = node["Distortion"][1]["x"].asFloat();
		iter.fDistortion2.y = node["Distortion"][1]["y"].asFloat();
		iter.fDistortion3.x = node["Distortion"][2]["x"].asFloat();
		iter.fDistortion3.y = node["Distortion"][2]["y"].asFloat();
		iter.fDistortion4.x = node["Distortion"][3]["x"].asFloat();
		iter.fDistortion4.y = node["Distortion"][3]["y"].asFloat();
		iter.fDiscardAlpha = node["DiscardAlpha"].asFloat();
		iter.bAlphaReverse = node["ReverseAlpha"].asBool();
		iter.bSetColor = node["SetColor"].asBool();
		
		iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVectorSet(node["Right"]["x"].asFloat(), node["Right"]["y"].asFloat(), node["Right"]["z"].asFloat(), 0.f));
		iter.pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(node["Up"]["x"].asFloat(), node["Up"]["y"].asFloat(), node["Up"]["z"].asFloat(), 0.f));
		iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVectorSet(node["Look"]["x"].asFloat(), node["Look"]["y"].asFloat(), node["Look"]["z"].asFloat(), 0.f));
		
		iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));

		m_bCreate = true;
		m_bStart = true;
		m_vecEffect.emplace_back(iter);
	}
}

void CEffect_Model::Update_Network(const string& pkey) {
}

void CEffect_Model::Update_Network(void) {
}
