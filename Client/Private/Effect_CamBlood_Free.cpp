#include "pch.h"
#include "Effect_CamBlood_Free.h"

CEffect_CamBlood_Free* const CEffect_CamBlood_Free::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, const _tchar* const& pFilePath) {
	CEffect_CamBlood_Free* pInstnace = new CEffect_CamBlood_Free(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create(pFilePath)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEffect_CamBlood_Free::Clone(void* const& pArg) {
	CEffect_CamBlood_Free* pInstnace = new CEffect_CamBlood_Free(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect_CamBlood_Free::CEffect_CamBlood_Free(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}

CEffect_CamBlood_Free::CEffect_CamBlood_Free(const CEffect_CamBlood_Free& rhs)
	: CObject(rhs),
	m_vecEffect(rhs.m_vecEffect),
	m_ViewMatrix(rhs.m_ViewMatrix),
	m_ProjMatrix(rhs.m_ProjMatrix),
	m_fTime(rhs.m_fTime),
	m_fMainEndTime(rhs.m_fMainEndTime),
	m_iRenderIdx(rhs.m_iRenderIdx),
	m_bRepeat(rhs.m_bRepeat),
	m_bProgressLoop(rhs.m_bProgressLoop) {
}
void CEffect_CamBlood_Free::Delete(void) {
	__super::Delete();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pNORShader);
	Safe_Release(m_pRect);
	Safe_Release(m_pCollider);
	for (auto& iter : m_vecEffect) {
		Safe_Release(iter.pDiffuseTex);
		Safe_Release(iter.pNormalTex);
		Safe_Release(iter.pNoiseTex);
		Safe_Release(iter.pTempTex);
		Safe_Release(iter.pDestTex);
		Safe_Release(iter.pTransform);
	}
	m_vecEffect.clear();
}

const HRESULT CEffect_CamBlood_Free::Init_Create(const _tchar* const& pFilePath) {
	return S_OK;
}

const HRESULT CEffect_CamBlood_Free::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Effect"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Rect"), reinterpret_cast<CComponent*&>(m_pRect));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	LoadEffect();

	for (auto& iter : m_vecEffect) {
		iter.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
		iter.fPos.x = (iter.fPos.x - 1280.f) + g_iWinSizeX;
		iter.fPos.y = (iter.fPos.y + 720.f) - g_iWinSizeY;
		if (iter.bProj)
			iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
		else
			iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

		iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
		iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
		iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));
		iter.fScale.x *= (g_iWinSizeX * g_iWinSizeY) / (1300.f * 750.f);
		iter.fScale.y *= (g_iWinSizeX * g_iWinSizeY) / (1300.f * 750.f);
		iter.fScale.z *= (g_iWinSizeX * g_iWinSizeY) / (1300.f * 750.f);
		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
	}
	m_bCreate = true;
	m_bStart = true;
	m_pTarget_Depth = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"));
	return hr;
}

const _ubyte CEffect_CamBlood_Free::Update(const _double& dTimeDelta) {
	if (m_bCreate) {
		if (m_bStart) {
			m_dMainTime += dTimeDelta;
			for (auto& iter : m_vecEffect) {
				if (m_bRepeat && (m_dMainTime > m_fTime)) {
					for (auto& iter : m_vecEffect) {
						if (!m_bProgressLoop) {
							iter.fTime = 0.f;
							if (iter.bProj)
								iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
							else
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
						//iter.bRender = false;
						//iter.fAlphaTime = 0.f;
						////iter.fAlpha = 0.1f;
						//iter.fUV.x = 0;
						//iter.fUV.y = iter.fsetUVY;
						//iter.iSetFrame = 0;
						//iter.fTime = 0.f;
						//if (iter.bProj)
						//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
						//else
						//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
						//iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
					}
					m_dMainTime = 0.;
				}
				else if (!m_bRepeat && (m_dMainTime > m_fTime)) {
					m_dMainTime = static_cast<_double>(m_fTime);
					return OBJ_ERASE;
				}

				if (m_dMainTime < iter.fCreateTime) {
					iter.fAlphaTime = 0.f;
					//iter.fAlpha = 0.1f;
					iter.fUV.x = 0;
					iter.fUV.y = iter.fsetUVY;
					iter.iSetFrame = 0;
					iter.fTime = 0.f;
					for (auto& arr : iter.iKeyFrame)
						arr = 0;
					iter.bRender = false;
					continue;
				}

				if (m_dMainTime > iter.fCreateTime) {
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
								if (iter.bProj)
									iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
								else
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
								iter.bRender = true;
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

						if (iter.vecFrameDir.empty())
							vPos += XMLoadFloat3(&iter.fDir) * static_cast<_float>(dTimeDelta);
						else {
							if ((iter.fTime >= iter.vecFrameDir.back().first) || iter.vecFrameDir.size() == 1) {
								iter.fDir = iter.vecFrameDir.back().second;
							}
							else {
								while (iter.fTime > iter.vecFrameDir[iter.iKeyFrame[4] + 1].first)
									++iter.iKeyFrame[4];
								_vector vSour = XMLoadFloat3(&iter.vecFrameDir[iter.iKeyFrame[4]].second);
								_vector vDest = XMLoadFloat3(&iter.vecFrameDir[iter.iKeyFrame[4] + 1].second);
								_float fdirRatio = static_cast<_float>((iter.fTime - iter.vecFrameDir[iter.iKeyFrame[4]].first) / (iter.vecFrameDir[iter.iKeyFrame[4] + 1].first - iter.vecFrameDir[iter.iKeyFrame[4]].first));
								_vector vDir = XMVectorLerp(vSour, vDest, fdirRatio);
								XMStoreFloat3(&iter.fDir, vDir);
							}
						}


						if (iter.bProj)
							iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(XMVectorGetX(vPos) - g_iWinSizeX * 0.5f, XMVectorGetY(vPos) + g_iWinSizeY * 0.5f, 0.1f, 1.f));
						else
							iter.pTransform->Set_Row(CTransform::ERow::POS, vPos);

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

						_vector vScale = XMVectorLerp(XMLoadFloat3(&iter.fScale), XMLoadFloat3(&iter.fEndScale), fRatio);
						iter.pTransform->Set_Scale(vScale);
					}
					// 가속도(스무스정지)
					if (iter.bPhysics) {
						_float fRatio = ratioTime / (iter.fDeleteTime);
						if (isnan(fRatio))
							fRatio = 0.f;
						if (isinf(fRatio))
							fRatio = 0.f;

						_vector pos = iter.pTransform->Get_Row(CTransform::ERow::POS);

						iter.fResist.x *= iter.fResistP; // 0 - 9에 수렴
						_float resist = 1 - (iter.fResist.x / iter.fResist.y); // 0 -> 1 ( y가 vAccel값들이게하면 0에 수렴)
						_float resistratio = (fRatio + resist) / 2;
						if (isnan(resist))
							resist = 1.f;
						_vector ResiAccel = (XMLoadFloat3(&iter.vAccel)) * resist; //저항값
						_vector vAccel = XMLoadFloat3(&iter.vAccel);
						vAccel -= ResiAccel; // resiaccel이 vAccel이되면 멈춤
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
				}
			}
		}
	}
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CEffect_CamBlood_Free::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	//if (ImGui::CollapsingHeader("Create")) {
	//	ImGui::InputText("Diffuse", m_sEffectTex.szDif, IM_ARRAYSIZE(m_sEffectTex.szDif)); ImGui::SameLine();
	//	if (ImGui::Button("Change_dif")) {
	//		if (m_vecEffect.back().pDiffuseTex != nullptr) {
	//			Safe_Release(m_vecEffect.back().pDiffuseTex);
	//			string str;
	//			wstring wstr;
	//			str = m_sEffectTex.szDif;
	//			wstr.assign(str.begin(), str.end());
	//			wstr = TEXT("Texture_") + wstr;
	//			strcpy_s(m_vecEffect.back().szDif, m_sEffectTex.szDif);
	//			m_vecEffect.back().pDiffuseTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	//		}
	//	}
	//	ImGui::InputText("Normal", m_sEffectTex.szNor, IM_ARRAYSIZE(m_sEffectTex.szNor)); ImGui::SameLine();
	//	if (ImGui::Button("Change_nor")) {
	//		if (m_vecEffect.back().pNormalTex != nullptr) {
	//			Safe_Release(m_vecEffect.back().pNormalTex);
	//			string str;
	//			wstring wstr;
	//			str = m_sEffectTex.szNor;
	//			wstr.assign(str.begin(), str.end());
	//			wstr = TEXT("Texture_") + wstr;
	//			strcpy_s(m_vecEffect.back().szNor, m_sEffectTex.szNor);
	//			m_vecEffect.back().pNormalTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	//		}
	//	}
	//	ImGui::InputText("Noise", m_sEffectTex.szNoise, IM_ARRAYSIZE(m_sEffectTex.szNoise)); ImGui::SameLine();
	//	if (ImGui::Button("Change_noise")) {
	//		if (m_vecEffect.back().pNoiseTex != nullptr) {
	//			Safe_Release(m_vecEffect.back().pNoiseTex);
	//			string str;
	//			wstring wstr;
	//			str = m_sEffectTex.szNoise;
	//			wstr.assign(str.begin(), str.end());
	//			wstr = TEXT("Texture_") + wstr;
	//			strcpy_s(m_vecEffect.back().szNoise, m_sEffectTex.szNoise);
	//			m_vecEffect.back().pNoiseTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	//		}
	//	}
	//	ImGui::InputText("Temp", m_sEffectTex.szTemp, IM_ARRAYSIZE(m_sEffectTex.szTemp)); ImGui::SameLine();
	//	if (ImGui::Button("Change_tem")) {
	//		if (m_vecEffect.back().pTempTex != nullptr) {
	//			Safe_Release(m_vecEffect.back().pTempTex);
	//			string str;
	//			wstring wstr;
	//			str = m_sEffectTex.szTemp;
	//			wstr.assign(str.begin(), str.end());
	//			wstr = TEXT("Texture_") + wstr;
	//			strcpy_s(m_vecEffect.back().szTemp, m_sEffectTex.szTemp);
	//			m_vecEffect[0].pTempTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	//		}
	//	}
	//	ImGui::InputText("Dest", m_sEffectTex.szDest, IM_ARRAYSIZE(m_sEffectTex.szDest)); ImGui::SameLine();
	//	if (ImGui::Button("Change_des")) {
	//		if (m_vecEffect.back().pDestTex != nullptr) {
	//			Safe_Release(m_vecEffect.back().pDestTex);
	//			string str;
	//			wstring wstr;
	//			str = m_sEffectTex.szDest;
	//			wstr.assign(str.begin(), str.end());
	//			wstr = TEXT("Texture_") + wstr;
	//			strcpy_s(m_vecEffect.back().szDest, m_sEffectTex.szDest);
	//			m_vecEffect.back().pDestTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
	//		}
	//	}
	//	if (ImGui::Button("Create_Effect"))
	//		Create_Effect(m_sEffectTex);
	//}
	//ImGui::Separator();
	//if (ImGui::CollapsingHeader("Rect")) {
	//	ImGui::DragFloat("SizeX", &m_fSizeX);
	//	ImGui::DragFloat("SizeY", &m_fSizeY);
	//
	//	ImGui::DragFloat("PosX", &m_fX, 1.f, -1280.f, 1280.f);
	//	ImGui::DragFloat("PosY", &m_fY, 1.f, -720.f, 720.f);
	//}
	//if (ImGui::CollapsingHeader("Effecttime")) {
	//	_float time = static_cast<_float>(m_dMainTime);
	//	ImGui::DragFloat("MainTime", &time);
	//	ImGui::DragFloat("EndTime", &m_fTime);
	//}
	//if (ImGui::Button("Play"))
	//	m_bStart = !m_bStart;
	//ImGui::Separator();
	//
	//
	//ImGui::InputText("Save&Load_Path", m_szSave, IM_ARRAYSIZE(m_szSave));
	//string str{ m_szSave };
	//wstring swtr{ str.begin(), str.end() };
	//if (ImGui::Button("Save")) {
	//	SaveEffect(swtr.c_str());
	//}ImGui::SameLine();
	//if (ImGui::Button("Load"))
	//	LoadEffect(swtr.c_str());
	//
	//if (ImGui::Checkbox("Repeat", &m_bRepeat)) {};
	//ImGui::Checkbox("LoopType", &m_bProgressLoop);
	//ImGui::Separator();
	//if (ImGui::Button("Depth")) {
	//	sort(m_vecEffect.begin(), m_vecEffect.end(), [](EffectTex pSour, EffectTex pDest) {
	//		return pSour.fPos.z > pDest.fPos.z;
	//		});
	//}
	//_uint i = 0;
	//for (auto& iter : m_vecEffect) {
	//	char szNode[MAX_PATH];
	//	sprintf_s(szNode, "Node(%d)", i++);
	//	if (m_bCreate) {
	//		if (ImGui::TreeNode(szNode)) {
	//			ImGui::Checkbox("Proj", &iter.bProj); ImGui::SameLine();
	//			ImGui::Checkbox("LookCam", &iter.bLookCam); ImGui::SameLine();
	//			if (iter.bLookCam) {
	//				for (auto& node : m_vecEffect)
	//					node.bLookCam = true;
	//			}
	//			else {
	//				for (auto& node : m_vecEffect)
	//					node.bLookCam = false;
	//			}
	//			ImGui::Checkbox("isRenderNow", &iter.bRender);
	//			ImGui::DragFloat("CreateTime", &iter.fCreateTime, 0.005f, 0.0f);
	//			ImGui::DragFloat("ProgressTime", &iter.fDeleteTime, 0.01f, 0.f, m_fTime); ImGui::SameLine();
	//			if (ImGui::Button("AllProgressTime")) {
	//				for (auto& node : m_vecEffect)
	//					node.fDeleteTime = iter.fDeleteTime;
	//			}
	//			ImGui::DragFloat("time", &iter.fTime);
	//			ImGui::DragFloat("FrameTime", &iter.fFrameTime);
	//			ImGui::DragFloat("AlphaTime", &iter.fAlphaTime);
	//			ImGui::DragFloat2("UV", (_float*)&iter.fUV);
	//			ImGui::DragFloat2("FixUV", (_float*)&iter.fFixUV);
	//			ImGui::DragFloat3("CurPos", (_float*)&iter.pTransform->Get_Row(CTransform::ERow::POS));
	//			if (ImGui::CollapsingHeader("Transform")) {
	//				if (ImGui::DragFloat3("Pos", (_float*)&iter.fPos, 0.01f)) {
	//					if (iter.bProj)
	//						iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
	//					else
	//						iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
	//				}
	//				if (ImGui::DragFloat("Rotx", (_float*)&iter.fRot.x))
	//					iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x)); ImGui::SameLine();
	//				ImGui::Checkbox("rotx", &iter.bRot[0]);
	//				if (ImGui::DragFloat("Roty", (_float*)&iter.fRot.y))
	//					iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y)); ImGui::SameLine();
	//				ImGui::Checkbox("roty", &iter.bRot[1]);
	//				if (ImGui::DragFloat("Rotz", (_float*)&iter.fRot.z))
	//					iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z)); ImGui::SameLine();
	//				ImGui::Checkbox("rotz", &iter.bRot[2]);
	//
	//				if (ImGui::DragFloat3("Scale", (_float*)&iter.fScale, 0.1f, 0.1f))
	//					iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
	//			}
	//			if (ImGui::CollapsingHeader("EndTransform")) {
	//				ImGui::Checkbox("Move", &iter.bMove);
	//				if (iter.bMove) {
	//					ImGui::Checkbox("RandPos", &iter.bRandomEndPos);
	//					if (iter.bRandomEndPos) {
	//						//_vector vRPosmin, vRPosmax;
	//						//if (ImGui::DragFloat3("RandPosMin", (_float*)&iter.fRandomPosRange[0]))
	//						//	vRPosmin = XMLoadFloat3(&iter.fRandomPosRange[0]);
	//						//if (ImGui::DragFloat3("RandPosMax", (_float*)&iter.fRandomPosRange[1]))
	//						//	vRPosmax = XMLoadFloat3(&iter.fRandomPosRange[1]);
	//						//XMStoreFloat3(&iter.fEndPos, ((vRPosmax - vRPosmin) * (_float)(rand()) / RAND_MAX));
	//					}
	//					else {
	//						ImGui::DragFloat3("EndPos", (_float*)&iter.fEndPos); ImGui::SameLine();
	//						ImGui::Checkbox("AllEndPos", &iter.bAllEndPos);
	//						if (iter.bAllEndPos) {
	//							_float3 endpos = iter.fEndPos;
	//							for (auto& node : m_vecEffect)
	//								node.fEndPos = endpos;
	//						}
	//					}
	//					ImGui::Checkbox("DirType", &iter.bDirType);
	//
	//					ImGui::DragFloat3("Dir", (_float*)&iter.fDir, 0.001f); ImGui::SameLine();
	//					ImGui::Checkbox("AllDir", &iter.bAllDir);
	//					if (iter.bAllDir) {
	//						_float3 dir = iter.fDir;
	//						for (auto& node : m_vecEffect)
	//							node.fDir = dir;
	//					}
	//
	//					int d = 0;
	//					if (ImGui::TreeNode("DirKey")) {
	//						for (auto frame = iter.vecFrameDir.begin(); frame != iter.vecFrameDir.end(); ++frame) {
	//							char szFrame[MAX_PATH];
	//							sprintf_s(szFrame, "Frame(%d)", d++);
	//							if (ImGui::TreeNode(szFrame)) {
	//								if (ImGui::InputFloat("Time", &frame->first)) {
	//									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//										_float fir = frame->first;
	//										for (auto& node : m_vecEffect) {
	//											node.vecFrameDir[d - 1].first = fir;
	//										}
	//									}
	//								}
	//								if (ImGui::InputFloat3("Dir", (_float*)&frame->second)) {
	//									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//										_float3 sec = frame->second;
	//										for (auto& node : m_vecEffect) {
	//											node.vecFrameDir[d - 1].second = sec;
	//										}
	//									}
	//								}
	//
	//								if (ImGui::Button("Erase")) {
	//									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//										for (auto& node : m_vecEffect) {
	//											if (&node == &iter)
	//												continue;
	//											node.vecFrameDir.erase(node.vecFrameDir.begin() + d - 1);
	//										}
	//									}
	//									else {
	//										ImGui::TreePop();
	//										iter.iKeyFrame[4] = 0;
	//										iter.vecFrameDir.erase(frame);
	//										break;
	//									}
	//								}
	//								ImGui::TreePop();
	//							}
	//						}
	//						if (ImGui::Button("Create")) {
	//							if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//								for (auto& node : m_vecEffect)
	//									node.vecFrameDir.emplace_back();
	//							}
	//							else
	//								iter.vecFrameDir.emplace_back();
	//						}
	//						ImGui::TreePop();
	//					}
	//
	//					ImGui::Checkbox("RandRot", &iter.bRandomEndRot);
	//					if (iter.bRandomEndRot) {
	//						//_vector vRRotmin, vRRotmax;
	//						//if (ImGui::DragFloat3("RandRotMin", (_float*)&iter.fRandomRotRange[0]))
	//						//	vRRotmin = XMLoadFloat3(&iter.fRandomRotRange[0]);
	//						//if (ImGui::DragFloat3("RandRotMax", (_float*)&iter.fRandomRotRange[1]))
	//						//	vRRotmax = XMLoadFloat3(&iter.fRandomRotRange[1]);
	//						//XMStoreFloat3(&iter.fEndRot, ((vRRotmax - vRRotmin) * (_float)rand()) / RAND_MAX);
	//					}
	//					else
	//						ImGui::DragFloat3("EndRot", (_float*)&iter.fEndRot);
	//
	//					ImGui::Checkbox("RandScale", &iter.bRandomEndScale);
	//					if (iter.bRandomEndScale) {
	//						//_vector vRScalemin, vRScalemax;
	//						//if (ImGui::DragFloat3("RandScaleMin", (_float*)&iter.fRandomScaleRange[0]))
	//						//	vRScalemin = XMLoadFloat3(&iter.fRandomScaleRange[0]);
	//						//if (ImGui::DragFloat3("RandScaleMax", (_float*)&iter.fRandomScaleRange[1]))
	//						//	vRScalemax = XMLoadFloat3(&iter.fRandomScaleRange[1]);
	//						//XMStoreFloat3(&iter.fEndScale, ((vRScalemax - vRScalemin) * (_float)rand()) / RAND_MAX);
	//					}
	//					else {
	//						ImGui::DragFloat3("EndScale", (_float*)&iter.fEndScale);
	//						if (ImGui::Button("AllEndScale")) {
	//							_float3 endscale = iter.fEndScale;
	//							for (auto& iter : m_vecEffect)
	//								iter.fEndScale = endscale;
	//						}
	//					}
	//				}
	//			}
	//			if (ImGui::CollapsingHeader("Physics")) {
	//				ImGui::Checkbox("PhysicsMove", &iter.bPhysics);
	//				if (iter.bPhysics) {
	//					if (ImGui::DragFloat3("Accel", (_float*)&iter.vSetAccel)) {
	//						iter.vAccel = iter.vSetAccel;
	//					}
	//					ImGui::DragFloat2("Resist", (_float*)&iter.fResist);
	//					ImGui::DragFloat("ResistPower", (_float*)&iter.fResistP, 0.001f, 0.f, 1.f);
	//
	//					ImGui::Checkbox("RandRot", &iter.bRandomEndRot);
	//					if (iter.bRandomEndRot) {
	//						//_vector vRRotmin, vRRotmax;
	//						//ImGui::DragFloat3("RandRotMin", (_float*)&iter.fRandomRotRange[0]);
	//						//ImGui::DragFloat3("RandRotMax", (_float*)&iter.fRandomRotRange[1]);
	//						//vRRotmin = XMLoadFloat3(&iter.fRandomRotRange[0]);
	//						//vRRotmax = XMLoadFloat3(&iter.fRandomRotRange[1]);
	//						//XMStoreFloat3(&iter.fEndRot, ((vRRotmax - vRRotmin) * (_float)rand()) / RAND_MAX);
	//					}
	//					else
	//						ImGui::DragFloat3("EndRot", (_float*)&iter.fEndRot);
	//
	//					ImGui::Checkbox("RandScale", &iter.bRandomEndScale);
	//					if (iter.bRandomEndScale) {
	//						//_vector vRScalemin, vRScalemax;
	//						//ImGui::DragFloat3("RandScaleMin", (_float*)&iter.fRandomScaleRange[0]);
	//						//ImGui::DragFloat3("RandScaleMax", (_float*)&iter.fRandomScaleRange[1]);
	//						//vRScalemin = XMLoadFloat3(&iter.fRandomScaleRange[0]);
	//						//vRScalemax = XMLoadFloat3(&iter.fRandomScaleRange[1]);
	//						//XMStoreFloat3(&iter.fEndScale, ((vRScalemax - vRScalemin) * (_float)rand()) / RAND_MAX);
	//					}
	//					else
	//						ImGui::DragFloat3("EndScale", (_float*)&iter.fEndScale);
	//				}
	//				ImGui::Checkbox("isGravity", &iter.bGravity);
	//				if (iter.bGravity)
	//					ImGui::DragFloat("Gravity", &iter.fGravity);
	//			}
	//			if (ImGui::CollapsingHeader("Shader")) {
	//				ImGui::DragFloat("Speed", &iter.fFrameSpeed);
	//				ImGui::DragFloat2("UV", (_float*)&iter.fDivide, 1.f, 0.f, 10.f);
	//				ImGui::DragInt("SelectFrame", &iter.iOtherFrame);
	//				ImGui::DragFloat("SelectSpeed", &iter.fOtherSpeed);
	//				ImGui::DragInt("SelectAlphaFrame", &iter.iAlphaFrame);
	//				ImGui::DragFloat("SelectAlphaSet", &iter.fAlphaSet); ImGui::SameLine();
	//				ImGui::Checkbox("AlphaReverse", &iter.bAlphaReverse);
	//				ImGui::DragFloat("DissolveTime", (_float*)&iter.fDissolveTime);
	//
	//				int j = 0;
	//				if (ImGui::TreeNode("alphaKey")) {
	//					for (auto frame = iter.vecFrameAlpha.begin(); frame != iter.vecFrameAlpha.end(); ++frame) {
	//						char szFrame[MAX_PATH];
	//						sprintf_s(szFrame, "Frame(%d)", j++);
	//						if (ImGui::TreeNode(szFrame)) {
	//							if (ImGui::InputFloat("Time", &frame->first)) {
	//								if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//									_float fir = frame->first;
	//									for (auto& node : m_vecEffect) {
	//										node.vecFrameAlpha[j - 1].first = fir;
	//									}
	//								}
	//							}
	//							if (ImGui::InputFloat("alpha", (_float*)&frame->second)) {
	//								if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//									_float sec = frame->second;
	//									for (auto& node : m_vecEffect) {
	//										node.vecFrameAlpha[j - 1].second = sec;
	//									}
	//								}
	//							}
	//
	//							if (ImGui::Button("Erase")) {
	//								if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//									_float fir = iter.vecFrameAlpha.begin()->first;
	//									_float sec = iter.vecFrameAlpha.begin()->second;
	//									for (auto& node : m_vecEffect) {
	//										for (auto frame = node.vecFrameAlpha.begin(); frame != node.vecFrameAlpha.end(); ++frame) {
	//											frame->first = fir;
	//											frame->second = sec;
	//										}
	//									}
	//								}
	//								else {
	//									ImGui::TreePop();
	//									iter.iKeyFrame[0] = 0;
	//									iter.vecFrameAlpha.erase(frame);
	//									break;
	//								}
	//							}
	//							ImGui::TreePop();
	//						}
	//					}
	//					if (ImGui::Button("Create")) {
	//						if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//							for (auto& node : m_vecEffect)
	//								node.vecFrameAlpha.emplace_back();
	//						}
	//						else
	//							iter.vecFrameAlpha.emplace_back();
	//					}
	//					ImGui::TreePop();
	//				}
	//
	//				if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
	//					_float a = iter.fAlphaSet;
	//					_float d = iter.fDissolveTime;
	//					for (auto& node : m_vecEffect) {
	//						node.fAlphaSet = a;
	//						node.fDissolveTime = d;
	//					}
	//				}
	//
	//				ImGui::DragFloat4("Color", (_float*)&iter.fColor, 0.01f); ImGui::SameLine();
	//				ImGui::Checkbox("AllColor", &iter.bAllColor);
	//				if (iter.bAllColor) {
	//					_float4 color = iter.fColor;
	//					for (auto& node : m_vecEffect)
	//						node.fColor = color;
	//				}
	//
	//				if (ImGui::Button("UV.y_UP")) {
	//					++iter.fsetUVY;
	//					iter.fUV.y = iter.fsetUVY;
	//				}ImGui::SameLine();
	//				if (ImGui::Button("UV.y_DOWN")) {
	//					--iter.fsetUVY;
	//					iter.fUV.y = iter.fsetUVY;
	//				}
	//			}
	//			if (ImGui::CollapsingHeader("Smoke")) {
	//				ImGui::DragFloat3("ShaderScale", (_float*)&iter.fShadeScale, 0.1f);
	//				ImGui::DragFloat3("ScrollSpeed", (_float*)&iter.fScrollSpeeds, 0.1f);
	//				ImGui::DragFloat("SmokeAlpha", (_float*)&iter.fSmokeAlpha, 0.05f);
	//				ImGui::DragFloat2("Distortion1", (_float*)&iter.fDistortion1, 0.1f);
	//				ImGui::DragFloat2("Distortion2", (_float*)&iter.fDistortion2, 0.1f);
	//				ImGui::DragFloat2("Distortion3", (_float*)&iter.fDistortion3, 0.1f);
	//				ImGui::DragFloat2("Distortion4", (_float*)&iter.fDistortion4, 0.1f);
	//				ImGui::DragFloat("DiscardAlpha", (_float*)&iter.fDiscardAlpha, 0.05f, 0.f, 1.1f);
	//			}
	//			const char* szshader[] = { "VTXShader", "VTXNORShader" };
	//			_int shader = static_cast<_int>(iter.iShader);
	//			ImGui::Combo("ShaderIdx", &shader, szshader, IM_ARRAYSIZE(szshader));
	//			iter.iShader = static_cast<_uint>(shader);
	//
	//			// 셰이더 선택 - 텍스처
	//			if (iter.iShader == 0) {
	//				const char* iTexture[] = { "Technique_Default", "Pass_AlpahBlend","Pass_Smoke" ,"Pass_Frame","Pass_ColorFrame", "Pass_ColorFrame_A", "Pass_ProjSmoke",
	//											"Pass_SpiderCrack","Pass_BloodStain","Pass_Frame_A","Pass_OtherAlphaSmoke","Pass_HuntressSmoke","Pass_HuntressParticle","Pass_FireParticle","Pass_SetColorTex",
	//											"Pass_SmokeColorFrame","Pass_OtherAlphaColorFrame","Pass_OtherAlphaColorFrame_a" };
	//				_int pass = static_cast<_int>(iter.iShaderPass);
	//				ImGui::Combo("TexturePass", &pass, iTexture, IM_ARRAYSIZE(iTexture));
	//				iter.iShaderPass = static_cast<_uint>(pass);
	//				if (ImGui::Button("AllPass")) {
	//					for (auto& node : m_vecEffect)
	//						node.iShaderPass = iter.iShaderPass;
	//				}
	//			}
	//			else if (iter.iShader == 1) {
	//				const char* iTexture[] = { "Technique_Default", "Pass_NorFrame" };
	//				_int pass = static_cast<_int>(iter.iShaderPass);
	//				ImGui::Combo("TexturePass", &pass, iTexture, IM_ARRAYSIZE(iTexture));
	//				iter.iShaderPass = static_cast<_uint>(pass);
	//			}
	//
	//			// 렌더 선택
	//			const char* items[] = { "PRIORITY", "NONALPHABLEND", "DECAL", "DISTORTION", "SHADOW", "SHADOW_BAKE", "NONLIGHT", "GLOW", "ALPHABLEND", "PENETRATE", "UI" };
	//			ImGui::Combo("RENDERER", &m_iRenderIdx, items, IM_ARRAYSIZE(items));
	//
	//			if (ImGui::Button("Delete")) {
	//				Safe_Release(iter.pTransform);
	//				Safe_Release(iter.pDiffuseTex);
	//				Safe_Release(iter.pNormalTex);
	//				Safe_Release(iter.pNoiseTex);
	//				Safe_Release(iter.pTempTex);
	//				Safe_Release(iter.pDestTex);
	//				m_vecEffect.erase(m_vecEffect.begin() + i - 1);
	//			}
	//
	//			ImGui::TreePop();
	//		}
	//	}
	//}
}

void CEffect_CamBlood_Free::Late_Update(const _double& dTimeDelta) {
	if (nullptr == m_pRenderer)
		return;

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	_vector vCameraPos = pCamera->Get_Pos();
	for (auto& iter : m_vecEffect) {
		if (iter.bLookCam) {
			_float3 scale = m_pTransform->Get_Scale();
			_vector vLook = XMVector3Normalize((m_pTransform->Get_Matrix()).r[3] - vCameraPos);
			XMVectorSetY(vLook, 0.f);
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, vRight);
			m_pTransform->Set_Row(CTransform::ERow::UP, vUp);
			m_pTransform->Set_Row(CTransform::ERow::LOOK, vLook);
			m_pTransform->Set_Scale(XMLoadFloat3(&scale));
		}
	}

	m_pRenderer->Add_Object(static_cast<CRenderer::EOrder>(m_iRenderIdx), this);
}

const HRESULT CEffect_CamBlood_Free::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	pCamera->Set_RawValue(m_pShader);

	if (m_pShader == nullptr)
		return hr;

	ID3D11ShaderResourceView* pDepthSRV = m_pTarget_Depth->Get_ShaderResourceView();
	_float2 fWinSize = { g_iWinSizeX, g_iWinSizeY };

	for (auto& iter : m_vecEffect) {
		if (iter.bRender) {
			switch (eOrder) {
			case CRenderer::EOrder::NONLIGHT: {
				if (iter.bProj) {
					hr |= m_pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= m_pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}

				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
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
				hr |= m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= m_pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex)
						hr |= iter.pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(m_pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(m_pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
					hr |= m_pRect->Render(m_pShader, iter.iShaderPass);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
											break;
			case CRenderer::EOrder::ALPHABLEND: {
				if (iter.bProj) {
					hr |= m_pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= m_pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_Resource("g_DepthTexture", pDepthSRV);
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
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
				hr |= m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				_int r = 1;
				hr |= m_pShader->Set_RawValue("g_iAlphaColor", &r, sizeof(_int));

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex)
						hr |= iter.pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(m_pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(m_pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
					hr |= m_pRect->Render(m_pShader, iter.iShaderPass);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
											  break;
			case CRenderer::EOrder::GLOW: {
				if (iter.bProj) {
					hr |= m_pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= m_pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
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
				hr |= m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= m_pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex)
						hr |= iter.pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(m_pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(m_pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
					hr |= m_pRect->Render(m_pShader, iter.iShaderPass);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
										break;
			case CRenderer::EOrder::UI: {
				if (iter.bProj) {
					hr |= m_pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= m_pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= m_pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
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
				hr |= m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= m_pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex)
						hr |= iter.pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(m_pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(m_pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(m_pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(m_pShader, "g_DestTexture", 0);
					hr |= m_pRect->Render(m_pShader, iter.iShaderPass);
				}
				hr |= m_pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
			}
									  break;
			}
		}
	}
	return hr;
}

void CEffect_CamBlood_Free::LoadEffect() {
	
	_uint iRandBlood = static_cast<_uint>(rand() % 7);
	_tchar* filepath = TEXT("");
	switch (iRandBlood)
	{
	case 0:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood0.txt");
		break;
	case 1:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood1.txt");
		break;
	case 2:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood2.txt");
		break;
	case 3:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood3.txt");
		break;
	case 4:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood4.txt");
		break;
	case 5:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood5.txt");
		break;
	case 6:
		filepath = TEXT("../Bin/Resource/03.GamePlay/Effect/CamBlood6.txt");
		break;
	default:
		break;
	}
	wstring wname(&filepath[0]);
	
	ifstream ifs(wname.c_str());
	Json::Value root;
	ifs >> root;
	ifs.close();

	m_fTime = root["EndTime"].asFloat();
	m_bRepeat = root["Loop"].asBool();
	m_bProgressLoop = root["ProgressLoop"].asBool();
	m_fSizeX = root["sizeX"].asFloat();
	m_fSizeY = root["sizeY"].asFloat();
	m_fX = root["PosX"].asFloat();
	m_fY = root["PosY"].asFloat();
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	EffectTex iter;
	for (auto& node : root["Node"]) {
		iter = EffectTex{};
		strcpy_s(iter.szDif, node["Texture"][0].asString().c_str());
		strcpy_s(iter.szNor, node["Texture"][1].asString().c_str());
		strcpy_s(iter.szNoise, node["Texture"][2].asString().c_str());
		strcpy_s(iter.szTemp, node["Texture"][3].asString().c_str());
		strcpy_s(iter.szDest, node["Texture"][4].asString().c_str());

		string str;
		wstring wstr;
		str = iter.szDif;
		iter.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Texture_") + wstr;
		iter.pDiffuseTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
		str = iter.szNor;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Texture_") + wstr;
		iter.pNormalTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
		str = iter.szNoise;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Texture_") + wstr;
		iter.pNoiseTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
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
		iter.bProj = node["Proj"].asBool();

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
		iter.bRot[0] = node["isRotX"][0].asBool();
		iter.bRot[1] = node["isRotX"][1].asBool();
		iter.bRot[2] = node["isRotX"][2].asBool();

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
		//Pos
		for (auto& frame : node["vecPos"]) {
			pair<_float, _float3> iter2;
			iter2.first = frame["FrameTime"].asFloat();
			iter2.second.x = frame["Pos"]["x"].asFloat();
			iter2.second.y = frame["Pos"]["y"].asFloat();
			iter2.second.z = frame["Pos"]["z"].asFloat();
			iter.vecFramePos.push_back(iter2);
		}
		for (auto& frame : node["vecDir"]) {
			pair<_float, _float3> iter2;
			iter2.first = frame["FrameTime"].asFloat();
			iter2.second.x = frame["Dir"]["x"].asFloat();
			iter2.second.y = frame["Dir"]["y"].asFloat();
			iter2.second.z = frame["Dir"]["z"].asFloat();
			iter.vecFrameDir.push_back(iter2);
		}
		//Scale
		for (auto& frame : node["vecScale"]) {
			pair<_float, _float3> iter2;
			iter2.first = frame["FrameTime"].asFloat();
			iter2.second.x = frame["Scale"]["x"].asFloat();
			iter2.second.y = frame["Scale"]["y"].asFloat();
			iter2.second.z = frame["Scale"]["z"].asFloat();
			iter.vecFrameScale.push_back(iter2);
		}

		// shader
		iter.fDivide.x = node["Divide"]["x"].asFloat();
		iter.fDivide.y = node["Divide"]["y"].asFloat();
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
		iter.bLookCam = node["LookCam"].asBool();
		m_bCreate = true;
		m_bStart = true;

		if (iter.bProj)
			iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
		else
			iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

		iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
		iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
		iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));

		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
		iter.fAlphaTime = 0.f;
		iter.fUV.x = 0;
		iter.fUV.y = iter.fsetUVY;
		iter.iSetFrame = 0;
		iter.vAccel = iter.vSetAccel;
		iter.fResist.x = iter.fResist.y;


		m_vecEffect.emplace_back(iter);
	}

	sort(m_vecEffect.begin(), m_vecEffect.end(), [](EffectTex pSour, EffectTex pDest) {
		return pSour.fEndPos.z > pDest.fEndPos.z;
		});
}

const HRESULT CEffect_CamBlood_Free::Save_Data(Json::Value& root) {



	return HRESULT();
}

void CEffect_CamBlood_Free::Load_Data(Json::Value& root) {

}

void CEffect_CamBlood_Free::Update_Network(const string& pkey) {
}

void CEffect_CamBlood_Free::Update_Network(void) {
}
