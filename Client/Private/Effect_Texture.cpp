#include "pch.h"
#include "Effect_Texture.h"

CEffect_Texture* const CEffect_Texture::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, const _tchar* const& pFilePath) {
	CEffect_Texture* pInstnace = new CEffect_Texture(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create(pFilePath)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEffect_Texture::Clone(void* const& pArg) {
	CEffect_Texture* pInstnace = new CEffect_Texture(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect_Texture::CEffect_Texture(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {

}

CEffect_Texture::CEffect_Texture(const CEffect_Texture& rhs)
	: CObject(rhs) {
}

void CEffect_Texture::Delete(void) {
	__super::Delete();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pAnimShader);
	Safe_Release(m_pRect);
	Safe_Release(m_pCollider);
	for (auto& iter : m_vecEffect) {
		Safe_Release(iter.pDiffuseTex);
		Safe_Release(iter.pNormalTex);
		Safe_Release(iter.pNoiseTex);
		Safe_Release(iter.pTempTex);
		Safe_Release(iter.pDestTex);
		Safe_Release(iter.pModel1);
		Safe_Release(iter.pModel2);
		Safe_Release(iter.pTransform);
	}
	m_vecEffect.clear();
}

const HRESULT CEffect_Texture::Init_Create(const _tchar* const& pFilePath) {

	return S_OK;
}

const HRESULT CEffect_Texture::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Effect"), TEXT("TestureShader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelEffect"), TEXT("ModelShader"), reinterpret_cast<CComponent*&>(m_pAnimShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Rect"), reinterpret_cast<CComponent*&>(m_pRect));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	m_pTarget_Depth = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"));
	return hr;
}

const _ubyte CEffect_Texture::Update(const _double& dTimeDelta) {
	if (m_bCreate) {
		if (m_bStart) {
			m_dMainTime += dTimeDelta;
			for (auto& iter : m_vecEffect) {
				if (m_bRepeat && (m_dMainTime > m_fMainEndTime)) {
					if (!m_bProgressLoop) {
						iter.fTime = 0.f;
						if (iter.bRandCreateTime)
							iter.fCreateTime = (iter.fRandCreateTime.y - iter.fRandCreateTime.x) * rand() / RAND_MAX + iter.fRandCreateTime.x;
						if (iter.bRandProgressTime)
							iter.fProgressTime = (iter.fRandProgressTime.y - iter.fRandProgressTime.x) * rand() / RAND_MAX + iter.fRandProgressTime.x;

						if (iter.bRandPos) {
							iter.fPos.x = (iter.fRandPos[1].x - iter.fRandPos[0].x) * rand() / RAND_MAX + iter.fRandPos[0].x;
							iter.fPos.y = (iter.fRandPos[1].y - iter.fRandPos[0].y) * rand() / RAND_MAX + iter.fRandPos[0].y;
							iter.fPos.z = (iter.fRandPos[1].z - iter.fRandPos[0].z) * rand() / RAND_MAX + iter.fRandPos[0].z;

							if (iter.fPos.x < 0)
								iter.fPos.x += iter.fLimitPos[0].x;
							else
								iter.fPos.x += iter.fLimitPos[1].x;

							if (iter.fPos.y < 0)
								iter.fPos.y += iter.fLimitPos[0].y;
							else
								iter.fPos.y += iter.fLimitPos[1].y;

							if (iter.fPos.z < 0)
								iter.fPos.z += iter.fLimitPos[0].z;
							else
								iter.fPos.z += iter.fLimitPos[1].z;
						}
						//if (iter.bProj)
						//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
						//else
						iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

						if (iter.bRandRot) {
							iter.fRot.x = (iter.fRandRot[1].x - iter.fRandRot[0].x) * rand() / RAND_MAX + iter.fRandRot[0].x;
							iter.fRot.y = (iter.fRandRot[1].y - iter.fRandRot[0].y) * rand() / RAND_MAX + iter.fRandRot[0].y;
							iter.fRot.z = (iter.fRandRot[1].z - iter.fRandRot[0].z) * rand() / RAND_MAX + iter.fRandRot[0].z;
						}
						if (iter.bRot[0])
							iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
						if (iter.bRot[1])
							iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
						if (iter.bRot[2])
							iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));

						if (iter.bRandScale) {
							iter.fScale.x = (iter.fRandScale[1].x - iter.fRandScale[0].x) * rand() / RAND_MAX + iter.fRandScale[0].x;
							iter.fScale.y = (iter.fRandScale[1].y - iter.fRandScale[0].y) * rand() / RAND_MAX + iter.fRandScale[0].y;
							iter.fScale.z = (iter.fRandScale[1].z - iter.fRandScale[0].z) * rand() / RAND_MAX + iter.fRandScale[0].z;
						}
						iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));

						if (iter.bRandomEndPos) {
							iter.fEndPos.x = (iter.fRandomPosRange[1].x - iter.fRandomPosRange[0].x) * rand() / RAND_MAX + iter.fRandomPosRange[0].x;
							iter.fEndPos.y = (iter.fRandomPosRange[1].y - iter.fRandomPosRange[0].y) * rand() / RAND_MAX + iter.fRandomPosRange[0].y;
							iter.fEndPos.z = (iter.fRandomPosRange[1].z - iter.fRandomPosRange[0].z) * rand() / RAND_MAX + iter.fRandomPosRange[0].z;
						}
						if (iter.bRandomDir) {
							iter.fDir.x = (iter.fRandomDirRange[1].x - iter.fRandomDirRange[0].x) * rand() / RAND_MAX + iter.fRandomDirRange[0].x;
							iter.fDir.y = (iter.fRandomDirRange[1].y - iter.fRandomDirRange[0].y) * rand() / RAND_MAX + iter.fRandomDirRange[0].y;
							iter.fDir.z = (iter.fRandomDirRange[1].z - iter.fRandomDirRange[0].z) * rand() / RAND_MAX + iter.fRandomDirRange[0].z;
						}
						for (auto& frame : iter.vecFrameDir) {
							if (frame.bRandTime)
								frame.fDirTime = (frame.fRandDirTimeRange.y - frame.fRandDirTimeRange.x) * rand() / RAND_MAX + frame.fRandDirTimeRange.x;
							if (frame.bRandDir)
								frame.fDir = {
								(frame.fRandDirRange[1].x - frame.fRandDirRange[0].x) * rand() / RAND_MAX + frame.fRandDirRange[0].x,
								(frame.fRandDirRange[1].y - frame.fRandDirRange[0].y) * rand() / RAND_MAX + frame.fRandDirRange[0].y,
								(frame.fRandDirRange[1].z - frame.fRandDirRange[0].z) * rand() / RAND_MAX + frame.fRandDirRange[0].z
							};
						}
						if (iter.bRandomEndRot) {
							iter.fEndRot.x = (iter.fRandomRotRange[1].x - iter.fRandomRotRange[0].x) * rand() / RAND_MAX + iter.fRandomRotRange[0].x;
							iter.fEndRot.y = (iter.fRandomRotRange[1].y - iter.fRandomRotRange[0].y) * rand() / RAND_MAX + iter.fRandomRotRange[0].y;
							iter.fEndRot.z = (iter.fRandomRotRange[1].z - iter.fRandomRotRange[0].z) * rand() / RAND_MAX + iter.fRandomRotRange[0].z;
						}
						if (iter.bRandomEndScale) {
							iter.fEndScale.x = (iter.fRandomScaleRange[1].x - iter.fRandomScaleRange[0].x) * rand() / RAND_MAX + iter.fRandomScaleRange[0].x;
							iter.fEndScale.y = (iter.fRandomScaleRange[1].y - iter.fRandomScaleRange[0].y) * rand() / RAND_MAX + iter.fRandomScaleRange[0].y;
							iter.fEndScale.z = (iter.fRandomScaleRange[1].z - iter.fRandomScaleRange[0].z) * rand() / RAND_MAX + iter.fRandomScaleRange[0].z;
						}
						for (auto& frame : iter.vecFrameScale) {
							if (frame.bRandTime)
								frame.fScaleTime = (frame.fRandScaleTimeRange.y - frame.fRandScaleTimeRange.x) * rand() / RAND_MAX + frame.fRandScaleTimeRange.x;
							if (frame.bRandScale)
								frame.fScale = {
								(frame.fRandScaleRange[1].x - frame.fRandScaleRange[0].x) * rand() / RAND_MAX + frame.fRandScaleRange[0].x,
								(frame.fRandScaleRange[1].y - frame.fRandScaleRange[0].y) * rand() / RAND_MAX + frame.fRandScaleRange[0].y,
								(frame.fRandScaleRange[1].z - frame.fRandScaleRange[0].z) * rand() / RAND_MAX + frame.fRandScaleRange[0].z
							};
						}
						iter.fAlphaTime = 0.f;
						iter.fAlpha = 0.1f;
						iter.fUV.x = 0;
						iter.fUV.y = iter.fsetUVY;
						iter.iSetFrame = 0;

						if (iter.bRandAccel) {
							iter.vSetAccel.x = (iter.vRandAccel[1].x - iter.vRandAccel[0].x) * rand() / RAND_MAX + iter.vRandAccel[0].x;
							iter.vSetAccel.y = (iter.vRandAccel[1].y - iter.vRandAccel[0].y) * rand() / RAND_MAX + iter.vRandAccel[0].y;
							iter.vSetAccel.z = (iter.vRandAccel[1].z - iter.vRandAccel[0].z) * rand() / RAND_MAX + iter.vRandAccel[0].z;
						}
						iter.vAccel = iter.vSetAccel;

						if (iter.bRandResist) {
							iter.fResist.y = (iter.fRandResist[1].y - iter.fRandResist[0].y) * rand() / RAND_MAX + iter.fRandResist[0].y;
						}
						iter.fResist.x = iter.fResist.y;

						if (iter.bRandResistP)
							iter.fResistP = (iter.fRandResistP[1] - iter.fRandResistP[0]) * rand() / RAND_MAX + iter.fRandResistP[0];

						iter.fGa = 0.f;
						iter.bRender = false;
						for (auto& arr : iter.iKeyFrame)
							arr = 0;
					}
					if (&iter == &m_vecEffect.back())
						m_dMainTime = 0.;
					continue;
				}
				else if (!m_bRepeat && (m_dMainTime > m_fMainEndTime))
					m_dMainTime = static_cast<_double>(m_fMainEndTime);

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
					if (iter.fTime > iter.fProgressTime) {
						//iter.fTime = iter.fDeleteTime;
						iter.bRender = false;
					}
					if (m_bProgressLoop) {
						if (iter.fTime >= iter.fProgressTime) {
							if (m_bRepeat) {
								iter.fTime = 0.f;
								if (iter.bRandCreateTime)
									iter.fCreateTime = (iter.fRandCreateTime.y - iter.fRandCreateTime.x) * rand() / RAND_MAX + iter.fRandCreateTime.x;
								if (iter.bRandProgressTime)
									iter.fProgressTime = (iter.fRandProgressTime.y - iter.fRandProgressTime.x) * rand() / RAND_MAX + iter.fRandProgressTime.x;

								if (iter.bRandPos) {
									iter.fPos.x = (iter.fRandPos[1].x - iter.fRandPos[0].x) * rand() / RAND_MAX + iter.fRandPos[0].x;
									iter.fPos.y = (iter.fRandPos[1].y - iter.fRandPos[0].y) * rand() / RAND_MAX + iter.fRandPos[0].y;
									iter.fPos.z = (iter.fRandPos[1].z - iter.fRandPos[0].z) * rand() / RAND_MAX + iter.fRandPos[0].z;

									if (iter.fPos.x < 0)
										iter.fPos.x += iter.fLimitPos[0].x;
									else
										iter.fPos.x += iter.fLimitPos[1].x;

									if (iter.fPos.y < 0)
										iter.fPos.y += iter.fLimitPos[0].y;
									else
										iter.fPos.y += iter.fLimitPos[1].y;

									if (iter.fPos.z < 0)
										iter.fPos.z += iter.fLimitPos[0].z;
									else
										iter.fPos.z += iter.fLimitPos[1].z;
								}
								//if (iter.bProj)
								//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
								//else
								iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

								if (iter.bRandRot) {
									iter.fRot.x = (iter.fRandRot[1].x - iter.fRandRot[0].x) * rand() / RAND_MAX + iter.fRandRot[0].x;
									iter.fRot.y = (iter.fRandRot[1].y - iter.fRandRot[0].y) * rand() / RAND_MAX + iter.fRandRot[0].y;
									iter.fRot.z = (iter.fRandRot[1].z - iter.fRandRot[0].z) * rand() / RAND_MAX + iter.fRandRot[0].z;
								}
								if (iter.bRot[0])
									iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
								if (iter.bRot[1])
									iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
								if (iter.bRot[2])
									iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));

								if (iter.bRandScale) {
									iter.fScale.x = (iter.fRandScale[1].x - iter.fRandScale[0].x) * rand() / RAND_MAX + iter.fRandScale[0].x;
									iter.fScale.y = (iter.fRandScale[1].y - iter.fRandScale[0].y) * rand() / RAND_MAX + iter.fRandScale[0].y;
									iter.fScale.z = (iter.fRandScale[1].z - iter.fRandScale[0].z) * rand() / RAND_MAX + iter.fRandScale[0].z;
								}
								iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));

								if (iter.bRandomEndPos) {
									iter.fEndPos.x = (iter.fRandomPosRange[1].x - iter.fRandomPosRange[0].x) * rand() / RAND_MAX + iter.fRandomPosRange[0].x;
									iter.fEndPos.y = (iter.fRandomPosRange[1].y - iter.fRandomPosRange[0].y) * rand() / RAND_MAX + iter.fRandomPosRange[0].y;
									iter.fEndPos.z = (iter.fRandomPosRange[1].z - iter.fRandomPosRange[0].z) * rand() / RAND_MAX + iter.fRandomPosRange[0].z;
								}
								if (iter.bRandomDir) {
									iter.fDir.x = (iter.fRandomDirRange[1].x - iter.fRandomDirRange[0].x) * rand() / RAND_MAX + iter.fRandomDirRange[0].x;
									iter.fDir.y = (iter.fRandomDirRange[1].y - iter.fRandomDirRange[0].y) * rand() / RAND_MAX + iter.fRandomDirRange[0].y;
									iter.fDir.z = (iter.fRandomDirRange[1].z - iter.fRandomDirRange[0].z) * rand() / RAND_MAX + iter.fRandomDirRange[0].z;
								}
								for (auto& frame : iter.vecFrameDir) {
									if (frame.bRandTime)
										frame.fDirTime = (frame.fRandDirTimeRange.y - frame.fRandDirTimeRange.x) * rand() / RAND_MAX + frame.fRandDirTimeRange.x;
									if (frame.bRandDir)
										frame.fDir = {
										(frame.fRandDirRange[1].x - frame.fRandDirRange[0].x) * rand() / RAND_MAX + frame.fRandDirRange[0].x,
										(frame.fRandDirRange[1].y - frame.fRandDirRange[0].y) * rand() / RAND_MAX + frame.fRandDirRange[0].y,
										(frame.fRandDirRange[1].z - frame.fRandDirRange[0].z) * rand() / RAND_MAX + frame.fRandDirRange[0].z
									};
								}
								if (iter.bRandomEndRot) {
									iter.fEndRot.x = (iter.fRandomRotRange[1].x - iter.fRandomRotRange[0].x) * rand() / RAND_MAX + iter.fRandomRotRange[0].x;
									iter.fEndRot.y = (iter.fRandomRotRange[1].y - iter.fRandomRotRange[0].y) * rand() / RAND_MAX + iter.fRandomRotRange[0].y;
									iter.fEndRot.z = (iter.fRandomRotRange[1].z - iter.fRandomRotRange[0].z) * rand() / RAND_MAX + iter.fRandomRotRange[0].z;
								}
								if (iter.bRandomEndScale) {
									iter.fEndScale.x = (iter.fRandomScaleRange[1].x - iter.fRandomScaleRange[0].x) * rand() / RAND_MAX + iter.fRandomScaleRange[0].x;
									iter.fEndScale.y = (iter.fRandomScaleRange[1].y - iter.fRandomScaleRange[0].y) * rand() / RAND_MAX + iter.fRandomScaleRange[0].y;
									iter.fEndScale.z = (iter.fRandomScaleRange[1].z - iter.fRandomScaleRange[0].z) * rand() / RAND_MAX + iter.fRandomScaleRange[0].z;
								}
								for (auto& frame : iter.vecFrameScale) {
									if (frame.bRandTime)
										frame.fScaleTime = (frame.fRandScaleTimeRange.y - frame.fRandScaleTimeRange.x) * rand() / RAND_MAX + frame.fRandScaleTimeRange.x;
									if (frame.bRandScale)
										frame.fScale = {
										(frame.fRandScaleRange[1].x - frame.fRandScaleRange[0].x) * rand() / RAND_MAX + frame.fRandScaleRange[0].x,
										(frame.fRandScaleRange[1].y - frame.fRandScaleRange[0].y) * rand() / RAND_MAX + frame.fRandScaleRange[0].y,
										(frame.fRandScaleRange[1].z - frame.fRandScaleRange[0].z) * rand() / RAND_MAX + frame.fRandScaleRange[0].z
									};
								}
								iter.fAlphaTime = 0.f;
								iter.fAlpha = 0.1f;
								iter.fUV.x = 0;
								iter.fUV.y = iter.fsetUVY;
								iter.iSetFrame = 0;

								if (iter.bRandAccel) {
									iter.vSetAccel.x = (iter.vRandAccel[1].x - iter.vRandAccel[0].x) * rand() / RAND_MAX + iter.vRandAccel[0].x;
									iter.vSetAccel.y = (iter.vRandAccel[1].y - iter.vRandAccel[0].y) * rand() / RAND_MAX + iter.vRandAccel[0].y;
									iter.vSetAccel.z = (iter.vRandAccel[1].z - iter.vRandAccel[0].z) * rand() / RAND_MAX + iter.vRandAccel[0].z;
								}
								iter.vAccel = iter.vSetAccel;

								if (iter.bRandResist) {
									iter.fResist.y = (iter.fRandResist[1].y - iter.fRandResist[0].y) * rand() / RAND_MAX + iter.fRandResist[0].y;
								}
								iter.fResist.x = iter.fResist.y;

								if (iter.bRandResistP)
									iter.fResistP = (iter.fRandResistP[1] - iter.fRandResistP[0]) * rand() / RAND_MAX + iter.fRandResistP[0];

								iter.fGa = 0.f;
								iter.bRender = false;
								for (auto& arr : iter.iKeyFrame)
									arr = 0;
							}
							else {
								ratioTime = m_fMainEndTime;
								iter.fTime = m_fMainEndTime;
							}
						}
					}

					if (iter.bMove) {
						_float fRatio = ratioTime / (iter.fProgressTime);
						if (isnan(fRatio))
							fRatio = 0.f;
						if (isinf(fRatio))
							fRatio = 0.f;
						_vector vPos = iter.pTransform->Get_Row(CTransform::ERow::POS);
						if (!iter.bDirType) {
							vPos = XMVectorLerp(XMLoadFloat3(&iter.fPos), XMLoadFloat3(&iter.fEndPos), fRatio);
							//if (iter.bProj)
							//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(XMVectorGetX(vPos) - g_iWinSizeX * 0.5f, XMVectorGetY(vPos) + g_iWinSizeY * 0.5f, 0.1f, 1.f));
							//else
							iter.pTransform->Set_Row(CTransform::ERow::POS, vPos);
						}
						else {
							if (iter.vecFrameDir.empty())
								vPos += XMLoadFloat3(&iter.fDir) * static_cast<_float>(dTimeDelta);
							else {
								if ((iter.fTime >= iter.vecFrameDir.back().fDirTime) || iter.vecFrameDir.size() == 1) {
									iter.fDir = iter.vecFrameDir.back().fDir;
								}
								else {
									while (iter.fTime > iter.vecFrameDir[iter.iKeyFrame[4] + 1].fDirTime)
										++iter.iKeyFrame[4];
									_vector vSour = XMLoadFloat3(&iter.vecFrameDir[iter.iKeyFrame[4]].fDir);
									_vector vDest = XMLoadFloat3(&iter.vecFrameDir[iter.iKeyFrame[4] + 1].fDir);
									_float fdirRatio = static_cast<_float>((iter.fTime - iter.vecFrameDir[iter.iKeyFrame[4]].fDirTime) / (iter.vecFrameDir[iter.iKeyFrame[4] + 1].fDirTime - iter.vecFrameDir[iter.iKeyFrame[4]].fDirTime));
									_vector vDir = XMVectorLerp(vSour, vDest, fdirRatio);
									XMStoreFloat3(&iter.fDir, vDir);
									if (isnan(iter.fDir.x))
										int a = 0;
									vPos += XMLoadFloat3(&iter.fDir) * static_cast<_float>(dTimeDelta);
									iter.pTransform->Set_Row(CTransform::ERow::POS, vPos);
								}
							}
						}

						//_float3 vRot;
						//vRot.x = fRatio * iter.fEndRot.x + (1 - fRatio) * iter.fRot.x;
						//vRot.y = fRatio * iter.fEndRot.y + (1 - fRatio) * iter.fRot.y;
						//vRot.z = fRatio * iter.fEndRot.z + (1 - fRatio) * iter.fRot.z;

						//_float3 scale = iter.pTransform->Get_Scale();
						//_vector right = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
						//_vector up = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::UP));
						//_vector look = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::LOOK));
						//_matrix matrixRotX = XMMatrixRotationAxis(right, XMConvertToRadians(vRot.x));
						//_matrix matrixRotY = XMMatrixRotationAxis(up, XMConvertToRadians(vRot.y));
						//_matrix matrixRotZ = XMMatrixRotationAxis(look, XMConvertToRadians(vRot.z));
						//_matrix matrixRot = matrixRotX * matrixRotY * matrixRotZ;

						//iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRot* dTimeDelta));
						//iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRot* dTimeDelta));
						//iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRot* dTimeDelta));

						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(iter.fEndRot.x), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::UP), XMConvertToRadians(iter.fEndRot.y), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(iter.fEndRot.z), dTimeDelta);

						if (iter.vecFrameScale.empty()) {
							_vector vScale = XMVectorLerp(XMLoadFloat3(&iter.fScale), XMLoadFloat3(&iter.fEndScale), fRatio);
							iter.pTransform->Set_Scale(vScale);
						}
						else {
							if ((iter.fTime >= iter.vecFrameScale.back().fScaleTime) || iter.vecFrameScale.size() == 1) {
								iter.fScale = iter.vecFrameScale.back().fScale;
							}
							else {
								while (iter.fTime > iter.vecFrameScale[iter.iKeyFrame[3] + 1].fScaleTime)
									++iter.iKeyFrame[3];
								_vector vSour = XMLoadFloat3(&iter.vecFrameScale[iter.iKeyFrame[3]].fScale);
								_vector vDest = XMLoadFloat3(&iter.vecFrameScale[iter.iKeyFrame[3] + 1].fScale);
								_float fScaleRatio = static_cast<_float>((iter.fTime - iter.vecFrameScale[iter.iKeyFrame[3]].fScaleTime) / (iter.vecFrameScale[iter.iKeyFrame[3] + 1].fScaleTime - iter.vecFrameScale[iter.iKeyFrame[3]].fScaleTime));
								_vector vScale = XMVectorLerp(vSour, vDest, fScaleRatio);
								iter.pTransform->Set_Scale(vScale);
							}
						}
					}
					// 가속도
					if (iter.bPhysics) {
						_float fRatio = ratioTime / (iter.fProgressTime);
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
						pos += vAccel * static_cast<_float>(dTimeDelta); // 엑셀값은 계속 줄어드는데 한프레임에서 멈추면 저항으로 감소하지않은 값이 계속 델타타임과 곱해지면서 생기는 문제
						iter.pTransform->Set_Row(CTransform::ERow::POS, pos);
						if (iter.bGravity) {
							iter.fGa += iter.fGravity * static_cast<_float>(dTimeDelta);
							_float3 vPos;
							XMStoreFloat3(&vPos, iter.pTransform->Get_Row(CTransform::ERow::POS));
							vPos.y -= iter.fGa;
							iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos));
						}
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(iter.fEndRot.x), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::UP), XMConvertToRadians(iter.fEndRot.y), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(iter.fEndRot.z), dTimeDelta);

						_vector vScale = XMVectorLerp(XMLoadFloat3(&iter.fScale), XMLoadFloat3(&iter.fEndScale), resistratio);
						iter.pTransform->Set_Scale(vScale);
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

					if (!iter.vecFrameColor.empty()) {
						if (iter.iSetFrame >= (_uint)iter.iOtherFrame)
							fTime = iter.fOtherSpeed;

						if (iter.iSetFrame >= (_uint)iter.iAlphaFrame)
							iter.fAlphaTime += static_cast<_float>(dTimeDelta);

						if ((iter.fTime >= iter.vecFrameColor.back().first) || iter.vecFrameColor.size() == 1) {
							iter.fColor = iter.vecFrameColor.back().second;
						}
						else {
							while (iter.fTime > iter.vecFrameColor[iter.iKeyFrame[5] + 1].first)
								++iter.iKeyFrame[5];
							_vector vSour = XMLoadFloat4(&iter.vecFrameColor[iter.iKeyFrame[5]].second);
							_vector vDest = XMLoadFloat4(&iter.vecFrameColor[iter.iKeyFrame[5] + 1].second);
							_float fColorRatio = static_cast<_float>((iter.fTime - iter.vecFrameColor[iter.iKeyFrame[5]].first) / (iter.vecFrameColor[iter.iKeyFrame[5] + 1].first - iter.vecFrameColor[iter.iKeyFrame[5]].first));
							_vector vColor = XMVectorLerp(vSour, vDest, fColorRatio);
							XMStoreFloat4(&iter.fColor, vColor);
						}
					}

					if (iter.bFrameLoop) {
						if (iter.fFrameTime > fTime) {
							iter.fFrameTime = 0.f;
							++iter.iSetFrame;
							++iter.fUV.x;
							if (iter.fUV.x >= iter.fDivide.x) {
								++iter.fUV.y;
								if (iter.fUV.y >= iter.fDivide.y) {
									iter.fUV.y = 0;
									iter.fUV.x = 0;
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

void CEffect_Texture::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	if (ImGui::CollapsingHeader("Create")) {
		ImGui::Text("Texture");
		ImGui::InputText("Diffuse", m_sEffectTex.szDif, IM_ARRAYSIZE(m_sEffectTex.szDif)); ImGui::SameLine();
		if (ImGui::Button("Change_dif")) {
			if (m_vecEffect.back().pDiffuseTex != nullptr) {
				Safe_Release(m_vecEffect.back().pDiffuseTex);
				string str;
				wstring wstr;
				str = m_sEffectTex.szDif;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szDif, m_sEffectTex.szDif);
				m_vecEffect.back().pDiffuseTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Normal", m_sEffectTex.szNor, IM_ARRAYSIZE(m_sEffectTex.szNor)); ImGui::SameLine();
		if (ImGui::Button("Change_nor")) {
			if (m_vecEffect.back().pNormalTex != nullptr) {
				Safe_Release(m_vecEffect.back().pNormalTex);
				string str;
				wstring wstr;
				str = m_sEffectTex.szNor;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szNor, m_sEffectTex.szNor);
				m_vecEffect.back().pNormalTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Noise", m_sEffectTex.szNoise, IM_ARRAYSIZE(m_sEffectTex.szNoise)); ImGui::SameLine();
		if (ImGui::Button("Change_noise")) {
			if (m_vecEffect.back().pNoiseTex != nullptr) {
				Safe_Release(m_vecEffect.back().pNoiseTex);
				string str;
				wstring wstr;
				str = m_sEffectTex.szNoise;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szNoise, m_sEffectTex.szNoise);
				m_vecEffect.back().pNoiseTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Temp", m_sEffectTex.szTemp, IM_ARRAYSIZE(m_sEffectTex.szTemp)); ImGui::SameLine();
		if (ImGui::Button("Change_tem")) {
			if (m_vecEffect.back().pTempTex != nullptr) {
				Safe_Release(m_vecEffect.back().pTempTex);
				string str;
				wstring wstr;
				str = m_sEffectTex.szTemp;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szTemp, m_sEffectTex.szTemp);
				m_vecEffect.back().pTempTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Dest", m_sEffectTex.szDest, IM_ARRAYSIZE(m_sEffectTex.szDest)); ImGui::SameLine();
		if (ImGui::Button("Change_des")) {
			if (m_vecEffect.back().pDestTex != nullptr) {
				Safe_Release(m_vecEffect.back().pDestTex);
				string str;
				wstring wstr;
				str = m_sEffectTex.szDest;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Texture_") + wstr;
				strcpy_s(m_vecEffect.back().szDest, m_sEffectTex.szDest);
				m_vecEffect.back().pDestTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::Text("Model");
		ImGui::InputText("Model1", m_sEffectTex.szModel1, IM_ARRAYSIZE(m_sEffectTex.szModel1)); ImGui::SameLine();
		if (ImGui::Button("Change_Model1")) {
			if (m_vecEffect.back().pModel1 != nullptr) {
				Safe_Release(m_vecEffect.back().pModel1);
				string str;
				wstring wstr;
				str = m_sEffectTex.szModel1;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Model_") + wstr;
				strcpy_s(m_vecEffect.back().szModel1, m_sEffectTex.szModel1);
				m_vecEffect.back().pModel1 = dynamic_cast<CModel*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		ImGui::InputText("Model2", m_sEffectTex.szModel2, IM_ARRAYSIZE(m_sEffectTex.szModel2)); ImGui::SameLine();
		if (ImGui::Button("Change_Model2")) {
			if (m_vecEffect.back().pModel2 != nullptr) {
				Safe_Release(m_vecEffect.back().pModel2);
				string str;
				wstring wstr;
				str = m_sEffectTex.szModel2;
				wstr.assign(str.begin(), str.end());
				wstr = TEXT("Model_") + wstr;
				strcpy_s(m_vecEffect.back().szModel2, m_sEffectTex.szModel2);
				m_vecEffect.back().pModel2 = dynamic_cast<CModel*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
			}
		}
		if (ImGui::Button("Create_Effect"))
			Create_Effect(m_sEffectTex);
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Rect")) {
		ImGui::DragFloat("SizeX", &m_fSizeX);
		ImGui::DragFloat("SizeY", &m_fSizeY);

		ImGui::DragFloat("PosX", &m_fX, 1.f, -1280.f, 1280.f);
		ImGui::DragFloat("PosY", &m_fY, 1.f, -720.f, 720.f);
	}
	if (ImGui::CollapsingHeader("Effecttime")) {
		_float time = static_cast<_float>(m_dMainTime);
		ImGui::DragFloat("MainTime", &time);
		ImGui::DragFloat("EndTime", &m_fMainEndTime);
	}
	if (ImGui::Button("Play"))
		m_bStart = !m_bStart;
	ImGui::Separator();
	if (ImGui::Button("AllDelete")) {
		if (!m_vecEffect.empty()) {
			for (auto& iter : m_vecEffect) {
				Safe_Release(iter.pDiffuseTex);
				Safe_Release(iter.pNormalTex);
				Safe_Release(iter.pNoiseTex);
				Safe_Release(iter.pTempTex);
				Safe_Release(iter.pDestTex);
				Safe_Release(iter.pModel1);
				Safe_Release(iter.pModel2);
				Safe_Release(iter.pTransform);
			}
			m_vecEffect.clear();
		}
	}

	ImGui::InputText("Save&Load_Path", m_szSave, IM_ARRAYSIZE(m_szSave));
	string str{ m_szSave };
	wstring swtr{ str.begin(), str.end() };
	if (ImGui::Button("Save")) {
		SaveEffect(swtr.c_str());
	}ImGui::SameLine();
	if (ImGui::Button("Load"))
		LoadEffect(swtr.c_str());

	ImGui::Checkbox("Repeat", &m_bRepeat); ImGui::SameLine();
	ImGui::Checkbox("LoopType", &m_bProgressLoop); ImGui::SameLine();
	ImGui::Checkbox("MainLookCam", &m_bLookCam); ImGui::SameLine();
	ImGui::Checkbox("Sorting", &m_bSort); ImGui::SameLine();
	ImGui::Checkbox("Z_Sorting", &m_bZSort);
	ImGui::Separator();
	if (ImGui::Button("Z_Depth")) {
		sort(m_vecEffect.begin(), m_vecEffect.end(), [](EffectTex pSour, EffectTex pDest) {
			return XMVectorGetZ(pSour.pTransform->Get_Row(CTransform::ERow::POS)) > XMVectorGetZ(pDest.pTransform->Get_Row(CTransform::ERow::POS));
			});
	}
	_uint i = 0;
	for (auto& iter : m_vecEffect) {
		char szNode[MAX_PATH];
		sprintf_s(szNode, "Node(%d)", i++);
		if (m_bCreate) {
			if (ImGui::TreeNode(szNode)) {
				ImGui::Text(iter.szDif);
				ImGui::Checkbox("Proj", &iter.bProj); ImGui::SameLine();
				ImGui::Checkbox("LookCam", &iter.bLookCam); ImGui::SameLine();
				if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
					for (auto& node : m_vecEffect)
						node.bLookCam = iter.bLookCam;
				}
				ImGui::Checkbox("isRenderNow", &iter.bRender);
				ImGui::Separator();
				ImGui::Checkbox("RandCreateTime", &iter.bRandCreateTime); ImGui::SameLine();
				ImGui::Checkbox("RandProgressTime", &iter.bRandProgressTime);

				ImGui::DragFloat("CreateTime", &iter.fCreateTime, 0.001f, 0.0f);
				if (iter.bRandCreateTime) {
					ImGui::DragFloat2("RandCreateTime", (_float*)&iter.fRandCreateTime, 0.001f, 0.0f);
					if (iter.fRandCreateTime.x > iter.fRandCreateTime.y)
						iter.fRandCreateTime.y = iter.fRandCreateTime.x;
				}

				ImGui::DragFloat("ProgressTime", &iter.fProgressTime, 0.01f, 0.f, m_fMainEndTime);
				if (iter.bRandProgressTime) {
					ImGui::DragFloat2("RandProgressTime", (_float*)&iter.fRandProgressTime, 0.001f, 0.0f);
					if (iter.fRandProgressTime.x > iter.fRandProgressTime.y)
						iter.fRandProgressTime.y = iter.fRandProgressTime.x;
				}
				if (ImGui::Button("AllProgressTime")) {
					for (auto& node : m_vecEffect)
						node.fProgressTime = iter.fProgressTime;
				}

				ImGui::DragFloat("time", &iter.fTime);
				ImGui::DragFloat("FrameTime", &iter.fFrameTime);
				ImGui::DragFloat("AlphaTime", &iter.fAlphaTime);
				ImGui::DragFloat2("UV", (_float*)&iter.fUV); ImGui::SameLine();
				ImGui::Checkbox("FixUV", &iter.bFix);
				if (iter.bFix)
					ImGui::DragFloat2("FixUV", (_float*)&iter.fFixUV);
				ImGui::DragFloat3("CurPos", (_float*)&iter.pTransform->Get_Row(CTransform::ERow::POS));
				ImGui::DragFloat3("CurRot", (_float*)&iter.fRot);
				ImGui::DragFloat3("CurScale", (_float*)&iter.pTransform->Get_Scale());
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::Checkbox("RandPos", &iter.bRandPos);
					if (iter.bRandPos) {
						ImGui::DragFloat3("RandPos_s", (_float*)&iter.fRandPos[0], 0.001f);
						ImGui::DragFloat3("RandPos_e", (_float*)&iter.fRandPos[1], 0.001f);
						ImGui::Checkbox("RandLimitPos", &iter.bLimitPos);
						if (iter.bLimitPos) {
							ImGui::DragFloat3("RandLimitPos_s", (_float*)&iter.fLimitPos[0], 0.001f);
							ImGui::DragFloat3("RandLimitPos_e", (_float*)&iter.fLimitPos[1], 0.001f);
						}
					}
					if (ImGui::DragFloat3("Pos", (_float*)&iter.fPos, 0.001f)) {
						//if (iter.bProj)
						//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
						//else
						iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
					}

					ImGui::Checkbox("RandRot", &iter.bRandRot);
					if (iter.bRandRot) {
						ImGui::DragFloat3("RandRot_s", (_float*)&iter.fRandRot[0], 0.001f);
						ImGui::DragFloat3("RandRot_e", (_float*)&iter.fRandRot[1], 0.001f);
						ImGui::Checkbox("rotx", &iter.bRot[0]); ImGui::SameLine();
						ImGui::Checkbox("roty", &iter.bRot[1]); ImGui::SameLine();
						ImGui::Checkbox("rotz", &iter.bRot[2]);

					}
					else {
						ImGui::Checkbox("rotx", &iter.bRot[0]); ImGui::SameLine();
						ImGui::Checkbox("roty", &iter.bRot[1]); ImGui::SameLine();
						ImGui::Checkbox("rotz", &iter.bRot[2]);
					}
					if (ImGui::DragFloat3("Rot", (_float*)&iter.fRot)) {
						if (iter.bRot[0])
							iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
						if (iter.bRot[1])
							iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
						if (iter.bRot[2])
							iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));
					}

					ImGui::Checkbox("RandScale", &iter.bRandScale);
					if (iter.bRandScale) {
						ImGui::DragFloat3("RandScale_s", (_float*)&iter.fRandScale[0], 0.001f);
						ImGui::DragFloat3("RandScale_e", (_float*)&iter.fRandScale[1], 0.001f);
					}
					if (ImGui::DragFloat3("Scale", (_float*)&iter.fScale, 0.001f, 0.1f)) 
						iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
					if (ImGui::Button("AllScale")) {
						for (auto& node : m_vecEffect) {
							node.fScale = iter.fScale;
							node.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
						}
					}
				}
				if (ImGui::CollapsingHeader("EndTransform")) {
					ImGui::Checkbox("Move", &iter.bMove);
					if (iter.bMove) {
						ImGui::Checkbox("RandEndPos", &iter.bRandomEndPos);
						if (iter.bRandomEndPos) {
							ImGui::DragFloat3("RandomPos_start", (_float*)&iter.fRandomPosRange[0], 0.001f);
							ImGui::DragFloat3("RandomPos_end", (_float*)&iter.fRandomPosRange[1], 0.001f);
						}
						ImGui::DragFloat3("EndPos", (_float*)&iter.fEndPos, 0.001f); ImGui::SameLine();
						if (ImGui::Button("AllEndPos")) {
							_float3 endpos = iter.fEndPos;
							for (auto& node : m_vecEffect)
								node.fEndPos = endpos;
						}
						ImGui::Checkbox("DirType", &iter.bDirType);
						if (iter.bDirType) {
							ImGui::SameLine();
							ImGui::Checkbox("RandDir", &iter.bRandomDir);
						}
						if (iter.bRandomDir) {
							ImGui::DragFloat3("RandomDir_start", (_float*)&iter.fRandomDirRange[0], 0.001f);
							ImGui::DragFloat3("RandomDir_end", (_float*)&iter.fRandomDirRange[1], 0.001f);
						}

						ImGui::DragFloat3("Dir", (_float*)&iter.fDir, 0.001f); ImGui::SameLine();
						if (ImGui::Button("AllDir")) {
							_float3 dir = iter.fDir;
							for (auto& node : m_vecEffect)
								node.fDir = dir;
						}

						int d = 0;
						if (ImGui::TreeNode("DirKey")) {
							for (auto frame = iter.vecFrameDir.begin(); frame != iter.vecFrameDir.end(); ++frame) {
								char szFrame[MAX_PATH];
								sprintf_s(szFrame, "Frame(%d)", d++);
								if (ImGui::TreeNode(szFrame)) {
									if (ImGui::InputFloat("Time", &frame->fDirTime)) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											_float fir = frame->fDirTime;
											for (auto& node : m_vecEffect) {
												node.vecFrameDir[d - 1].fDirTime = fir;
											}
										}
									}ImGui::SameLine();
									ImGui::Checkbox("KeyDirRandTime", &frame->bRandTime);
									if (frame->bRandTime)
										ImGui::InputFloat2("KeyDirRandTimeRange", (_float*)&frame->fRandDirTimeRange);

									if (ImGui::DragFloat3("Dir", (_float*)&frame->fDir, 0.001f)) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											_float3 sec = frame->fDir;
											for (auto& node : m_vecEffect) {
												node.vecFrameDir[d - 1].fDir = sec;
											}
										}
									}ImGui::SameLine();
									ImGui::Checkbox("KeyRandDir", &frame->bRandDir);
									if (frame->bRandDir) {
										ImGui::InputFloat3("KeyRandDirRange_s", (_float*)&frame->fRandDirRange[0]);
										ImGui::InputFloat3("KeyRandDirRange_e", (_float*)&frame->fRandDirRange[1]);
									}

									if (ImGui::Button("Erase")) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											for (auto& node : m_vecEffect) {
												if (&node == &iter)
													continue;
												node.vecFrameDir.erase(node.vecFrameDir.begin() + d - 1);
											}
										}
										else {
											ImGui::TreePop();
											iter.iKeyFrame[4] = 0;
											iter.vecFrameDir.erase(frame);
											break;
										}
									}
									ImGui::TreePop();
								}
							}
							if (ImGui::Button("Create")) {
								if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
									for (auto& node : m_vecEffect)
										node.vecFrameDir.emplace_back();
								}
								else
									iter.vecFrameDir.emplace_back();
							}
							ImGui::TreePop();
						}

						ImGui::Checkbox("RandEndRot", &iter.bRandomEndRot);
						if (iter.bRandomEndRot) {
							ImGui::DragFloat3("RandomRot_start", (_float*)&iter.fRandomRotRange[0], 0.01f);
							ImGui::DragFloat3("RandomRot_end", (_float*)&iter.fRandomRotRange[1], 0.01f);
						}
						ImGui::DragFloat3("EndRot", (_float*)&iter.fEndRot, 0.001f);

						ImGui::Checkbox("RandEndScale", &iter.bRandomEndScale);
						if (iter.bRandomEndScale) {
							ImGui::DragFloat3("RandomScale_start", (_float*)&iter.fRandomScaleRange[0], 0.001f);
							ImGui::DragFloat3("RandomScale_end", (_float*)&iter.fRandomScaleRange[1], 0.001f);
						}
						ImGui::DragFloat3("EndScale", (_float*)&iter.fEndScale, 0.001f);
						if (ImGui::Button("AllEndScale")) {
							_float3 endscale = iter.fEndScale;
							for (auto& iter : m_vecEffect)
								iter.fEndScale = endscale;
						}
						int s = 0;
						if (ImGui::TreeNode("ScaleKey")) {
							for (auto frame = iter.vecFrameScale.begin(); frame != iter.vecFrameScale.end(); ++frame) {
								char szFrame[MAX_PATH];
								sprintf_s(szFrame, "Frame(%d)", s++);
								if (ImGui::TreeNode(szFrame)) {
									if (ImGui::InputFloat("Time", &frame->fScaleTime)) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											_float fir = frame->fScaleTime;
											for (auto& node : m_vecEffect) {
												node.vecFrameScale[s - 1].fScaleTime = fir;
											}
										}
									}ImGui::SameLine();
									ImGui::Checkbox("KeyScaleRandTime", &frame->bRandTime);
									if (frame->bRandTime)
										ImGui::InputFloat2("KeyScaleRandTimeRange", (_float*)&frame->fRandScaleTimeRange);

									if (ImGui::InputFloat3("Scale", (_float*)&frame->fScale)) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											_float3 sec = frame->fScale;
											for (auto& node : m_vecEffect) {
												node.vecFrameScale[s - 1].fScale = sec;
											}
										}
									}ImGui::SameLine();
									ImGui::Checkbox("KeyRandScale", &frame->bRandScale);
									if (frame->bRandScale) {
										ImGui::InputFloat3("KeyRandScaleRange_s", (_float*)&frame->fRandScaleRange[0]);
										ImGui::InputFloat3("KeyRandScaleRange_e", (_float*)&frame->fRandScaleRange[1]);
									}

									if (ImGui::Button("Erase")) {
										if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
											for (auto& node : m_vecEffect) {
												if (&node == &iter)
													continue;
												node.vecFrameScale.erase(node.vecFrameScale.begin() + s - 1);
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

					}
				}
				if (ImGui::CollapsingHeader("Physics")) {
					ImGui::Checkbox("PhysicsMove", &iter.bPhysics);
					if (iter.bPhysics) {
						ImGui::Checkbox("RandAccel", &iter.bRandAccel);
						if (iter.bRandAccel) {
							ImGui::DragFloat3("RandAccel_s", (_float*)&iter.vRandAccel[0], 0.001f);
							ImGui::DragFloat3("RandAccel_e", (_float*)&iter.vRandAccel[1], 0.001f);

							if (iter.vRandAccel[0].x > iter.vRandAccel[1].x)
								iter.vRandAccel[1].x = iter.vRandAccel[0].x;
							if (iter.vRandAccel[0].y > iter.vRandAccel[1].y)
								iter.vRandAccel[1].y = iter.vRandAccel[0].y;
							if (iter.vRandAccel[0].z > iter.vRandAccel[1].z)
								iter.vRandAccel[1].z = iter.vRandAccel[0].z;

						}
						if (ImGui::DragFloat3("Accel", (_float*)&iter.vSetAccel, 0.001f))
							iter.vAccel = iter.vSetAccel;

						ImGui::Checkbox("RandResist", &iter.bRandResist);
						if (iter.bRandResist) {
							ImGui::DragFloat2("RandResist_s", (_float*)&iter.fRandResist[0], 0.01f);
							ImGui::DragFloat2("RandResist_e", (_float*)&iter.fRandResist[1], 0.01f);

							if (iter.fRandResist[0].x > iter.fRandResist[1].x)
								iter.fRandResist[1].x = iter.fRandResist[0].x;
							if (iter.fRandResist[0].y > iter.fRandResist[1].y)
								iter.fRandResist[1].y = iter.fRandResist[0].y;

						}
						ImGui::DragFloat2("Resist", (_float*)&iter.fResist);

						ImGui::Checkbox("RandResistP", &iter.bRandResistP);
						if (iter.bRandResistP) {
							ImGui::DragFloat("RandResistPower_s", (_float*)&iter.fRandResistP[0], 0.001f, 0.f, 1.f);
							ImGui::DragFloat("RandResistPower_e", (_float*)&iter.fRandResistP[1], 0.001f, 0.f, 1.f);

							if (iter.fRandResistP[0] > iter.fRandResistP[1])
								iter.fRandResistP[1] = iter.fRandResistP[0];
						}
						ImGui::DragFloat("ResistPower", (_float*)&iter.fResistP, 0.001f, 0.f, 1.f);

						ImGui::Checkbox("RandEndRot", &iter.bRandomEndRot);
						if (iter.bRandomEndRot) {
							ImGui::DragFloat3("RandomRot_start", (_float*)&iter.fRandomRotRange[0], 0.01f);
							ImGui::DragFloat3("RandomRot_end", (_float*)&iter.fRandomRotRange[1], 0.01f);
						}
						ImGui::DragFloat3("EndRot", (_float*)&iter.fEndRot, 0.001f);

						ImGui::Checkbox("RandEndScale", &iter.bRandomEndScale);
						if (iter.bRandomEndScale) {
							ImGui::DragFloat3("RandomScale_start", (_float*)&iter.fRandomScaleRange[0], 0.001f);
							ImGui::DragFloat3("RandomScale_end", (_float*)&iter.fRandomScaleRange[1], 0.001f);
						}
						ImGui::DragFloat3("EndScale", (_float*)&iter.fEndScale);
					}
					ImGui::Checkbox("isGravity", &iter.bGravity);
					if (iter.bGravity)
						ImGui::DragFloat("Gravity", &iter.fGravity);
				}
				if (ImGui::CollapsingHeader("Shader")) {
					ImGui::DragFloat("Speed", &iter.fFrameSpeed);
					ImGui::DragFloat2("UV", (_float*)&iter.fDivide, 1.f, 0.f, 10.f);
					ImGui::Checkbox("FrmaeLoop", &iter.bFrameLoop);
					ImGui::DragInt("SelectFrame", &iter.iOtherFrame);
					ImGui::DragFloat("SelectSpeed", &iter.fOtherSpeed);
					ImGui::DragInt("SelectAlphaFrame", &iter.iAlphaFrame);
					ImGui::DragFloat("SelectAlphaSet", &iter.fAlphaSet); ImGui::SameLine();
					ImGui::Checkbox("AlphaReverse", &iter.bAlphaReverse);
					ImGui::DragFloat("DissolveTime", (_float*)&iter.fDissolveTime);
					ImGui::DragFloat2("FlowAlpha", (_float*)&iter.fFlowUV, 0.01f);
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

					if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
						_float a = iter.fAlphaSet;
						_float d = iter.fDissolveTime;
						for (auto& node : m_vecEffect) {
							node.fAlphaSet = a;
							node.fDissolveTime = d;
						}
					}
					ImGui::DragFloat2("Glow", (_float*)&iter.fGlow_Buffer, 0.01f); ImGui::SameLine();
					ImGui::Checkbox("AllGlow", &iter.bAllGlow);
					if (iter.bAllGlow) {
						_float2 glow = iter.fGlow_Buffer;
						for (auto& node : m_vecEffect)
							node.fGlow_Buffer = glow;
					}
					ImGui::DragFloat4("Color", (_float*)&iter.fColor, 0.01f); ImGui::SameLine();
					ImGui::Checkbox("AllColor", &iter.bAllColor);
					if (iter.bAllColor) {
						_float4 color = iter.fColor;
						for (auto& node : m_vecEffect)
							node.fColor = color;
					}
					ImGui::DragFloat4("PlusColor", (_float*)&iter.fPlusColor, 0.01f); ImGui::SameLine();
					ImGui::Checkbox("AllPlusColor", &iter.bAllPlusColor);
					if (iter.bAllPlusColor) {
						_float4 color = iter.fPlusColor;
						for (auto& node : m_vecEffect)
							node.fPlusColor = color;
					}
					int c = 0;
					if (ImGui::TreeNode("ColorKey")) {
						for (auto frame = iter.vecFrameColor.begin(); frame != iter.vecFrameColor.end(); ++frame) {
							char szFrame[MAX_PATH];
							sprintf_s(szFrame, "Frame(%d)", c++);
							if (ImGui::TreeNode(szFrame)) {
								if (ImGui::InputFloat("Time", &frame->first)) {
									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
										_float fir = frame->first;
										for (auto& node : m_vecEffect) {
											node.vecFrameColor[c - 1].first = fir;
										}
									}
								}
								if (ImGui::InputFloat4("Color", (_float*)&frame->second)) {
									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
										_float4 sec = frame->second;
										for (auto& node : m_vecEffect) {
											node.vecFrameColor[c - 1].second = sec;
										}
									}
								}

								if (ImGui::Button("Erase")) {
									if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
										_float fir = iter.vecFrameColor.begin()->first;
										_float4 sec = iter.vecFrameColor.begin()->second;
										for (auto& node : m_vecEffect) {
											for (auto frame = node.vecFrameColor.begin(); frame != node.vecFrameColor.end(); ++frame) {
												frame->first = fir;
												frame->second = sec;
											}
										}
									}
									else {
										ImGui::TreePop();
										iter.iKeyFrame[5] = 0;
										iter.vecFrameColor.erase(frame);
										break;
									}
								}
								ImGui::TreePop();
							}
						}
						if (ImGui::Button("Create")) {
							if (CGame_Instance::Get_Instance()->Get_Input_KeyboardKey(DIK_A)) {
								for (auto& node : m_vecEffect)
									node.vecFrameColor.emplace_back();
							}
							else
								iter.vecFrameColor.emplace_back();
						}
						ImGui::TreePop();
					}


					if (ImGui::Button("UV.y_UP")) {
						++iter.fsetUVY;
						iter.fUV.y = iter.fsetUVY;
					}ImGui::SameLine();
					if (ImGui::Button("UV.y_DOWN")) {
						--iter.fsetUVY;
						iter.fUV.y = iter.fsetUVY;
					}
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
				const char* szshader[] = { "TextureShader", "ModelShader" };
				_int shader = static_cast<_int>(iter.iShader);
				ImGui::Combo("ShaderIdx", &shader, szshader, IM_ARRAYSIZE(szshader));
				iter.iShader = static_cast<_uint>(shader);

				// 셰이더 선택 - 텍스처
				if (iter.iShader == 0) {
					const char* iTexture[] = { "Technique_Default", "Pass_BloodMain","Pass_Smoke" ,"Pass_Frame","Pass_ColorFrame", "Pass_ColorFrame_A", "Pass_ProjSmoke",
												"Pass_SpiderCrack","Pass_BloodStain","Pass_Frame_A","Pass_OtherAlphaSmoke","Pass_HuntressSmoke","Pass_HuntressParticle","Pass_FireParticle","Pass_SetColorTex",
												"Pass_SmokeColorFrame","Pass_OtherAlphaColorFrame","Pass_OtherAlphaColorFrame_a","Pass_ColorFrame_AB_NoDepth","Pass_ParticleColorFrame_AB","Pass_ParticleColorwFrame_AB",
												"Pass_SmokeColorAlphaFrame", "Pass_SoftTest","Pass_SoftFrame_NA","Pass_SoftFrame_A","Pass_Soft_OtherFrameAlpha_NA","Pass_Soft_OtherFrameAlpha_A","Pass_PlusColorwFrame_AB",
												"Pass_SoftFlow_NA","Pass_SoftFlow_A"};
					_int pass = static_cast<_int>(iter.iShaderPass);
					ImGui::Combo("TexturePass", &pass, iTexture, IM_ARRAYSIZE(iTexture));
					iter.iShaderPass = static_cast<_uint>(pass);
					if (ImGui::Button("AllPass")) {
						for (auto& node : m_vecEffect)
							node.iShaderPass = iter.iShaderPass;
					}
				}
				else if (iter.iShader == 1) {
					const char* iTexture[] = { "Pass_Default", "Pass_Frame","Pass_UnNormalFrame" ,"Pass_OtherTexFrame","Pass_OtherTexSetColorFrame", "Pass_BloodSplainRG", "Pass_BloodSplainR",
										"Pass_AlphatestColorBlood","Pass_BloodParticle","Pass_FootBlood", "Pass_FootBlood_N","Pass_ColorAMesh","Pass_RainDrop" };
					_int pass = static_cast<_int>(iter.iShaderPass);
					ImGui::Combo("ModelPass", &pass, iTexture, IM_ARRAYSIZE(iTexture));
					iter.iShaderPass = static_cast<_uint>(pass);
					if (ImGui::Button("AllPass")) {
						for (auto& node : m_vecEffect)
							node.iShaderPass = iter.iShaderPass;
					}
				}

				// 렌더 선택
				const char* items[] = { "PRIORITY", "NONALPHABLEND", "DECAL", "DISTORTION", "SHADOW", "SHADOW_BAKE", "NONLIGHT", "GLOW", "ALPHABLEND", "PENETRATE", "UI" };
				ImGui::Combo("RENDERER", &m_iRenderIdx, items, IM_ARRAYSIZE(items));

				if (ImGui::Button("Delete")) {
					Safe_Release(iter.pTransform);
					Safe_Release(iter.pDiffuseTex);
					Safe_Release(iter.pNormalTex);
					Safe_Release(iter.pNoiseTex);
					Safe_Release(iter.pTempTex);
					Safe_Release(iter.pDestTex);
					m_vecEffect.erase(m_vecEffect.begin() + i - 1);
				}

				ImGui::TreePop();
			}
		}
	}
}

void CEffect_Texture::Late_Update(const _double& dTimeDelta) {
	if (nullptr == m_pRenderer)
		return;

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	_vector vCameraPos = pCamera->Get_Pos();
	_bool bProj = false;
	if (m_bLookCam) {
		_float3 scale = m_pTransform->Get_Scale();
		_vector vLook = XMVector3Normalize(m_pTransform->Get_Row(CTransform::ERow::POS) - vCameraPos);
		XMVectorSetY(vLook, 0.f);
		_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
		m_pTransform->Set_Row(CTransform::ERow::RIGHT, vRight);
		m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransform->Set_Row(CTransform::ERow::LOOK, vLook);
		m_pTransform->Set_Scale(XMLoadFloat3(&scale));
	}
	for (auto& iter : m_vecEffect) {
		if (iter.bLookCam) {
			_float3 scale = iter.pTransform->Get_Scale();
			_vector vLook = XMVector3Normalize(m_pTransform->Get_Row(CTransform::ERow::POS) + iter.pTransform->Get_Row(CTransform::ERow::POS) - vCameraPos);
			_vector vUp = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::UP));
			_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
			vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
			iter.pTransform->Set_Row(CTransform::ERow::RIGHT, vRight);
			iter.pTransform->Set_Row(CTransform::ERow::UP, vUp);
			iter.pTransform->Set_Row(CTransform::ERow::LOOK, vLook);
			iter.pTransform->Set_Scale(XMLoadFloat3(&scale));
		}
		if (iter.bProj)
			bProj = true;
	}
	if (!bProj && m_bStart && m_bSort) {
		sort(m_vecEffect.begin(), m_vecEffect.end(), [&](EffectTex pSour, EffectTex pDest) {
		_float sourLength = fabsf(XMVectorGetX(XMVector3Length(vCameraPos - m_pTransform->Get_Row(CTransform::ERow::POS) + pSour.pTransform->Get_Row(CTransform::ERow::POS))));
		_float destLength = fabsf(XMVectorGetX(XMVector3Length(vCameraPos - m_pTransform->Get_Row(CTransform::ERow::POS) + pDest.pTransform->Get_Row(CTransform::ERow::POS))));
		return sourLength < destLength;
			});
	}
	if (m_bStart && m_bZSort) {
		sort(m_vecEffect.begin(), m_vecEffect.end(), [](EffectTex pSour, EffectTex pDest) {
			return XMVectorGetZ(pSour.pTransform->Get_Row(CTransform::ERow::POS)) > XMVectorGetZ(pDest.pTransform->Get_Row(CTransform::ERow::POS));
			});
	}
	m_pRenderer->Add_Object(static_cast<CRenderer::EOrder>(m_iRenderIdx), this);
}

const HRESULT CEffect_Texture::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	CShader* pShader = nullptr;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//m_pCollider->Render(pCamera);

	if (m_pShader == nullptr)
		return hr;
	if (m_pAnimShader == nullptr)
		return hr;

	ID3D11ShaderResourceView* pDepthSRV = m_pTarget_Depth->Get_ShaderResourceView();
	_float2 fWinSize = { g_iWinSizeX, g_iWinSizeY };

	for (auto& iter : m_vecEffect) {
		if (iter.bRender) {
			if (iter.iShader == 0)
				pShader = m_pShader;
			else if (iter.iShader == 1)
				pShader = m_pAnimShader;

			pCamera->Set_RawValue(pShader);

			switch (eOrder) {
			case CRenderer::EOrder::NONALPHABLEND: {
				if (iter.bProj) {
					hr |= pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}

				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_flowUV", &iter.fFlowUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_PlusColor", &iter.fPlusColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_fGlow_Buffer", &iter.fGlow_Buffer, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex) {
						hr |= iter.pDiffuseTex->Set_Resource(pShader, "g_DiffuseTexture", 0);
						hr |= m_pRect->Render(pShader, iter.iShaderPass);
					}
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(pShader, "g_DestTexture", 0);

					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel1->Render(j, pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2 != nullptr) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel2->Render(j, pShader, iter.iShaderPass);
						}
					}
				}
			}
												 break;
			case CRenderer::EOrder::NONLIGHT: {
				if (iter.bProj) {
					hr |= pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}

				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_flowUV", &iter.fFlowUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_PlusColor", &iter.fPlusColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_fGlow_Buffer", &iter.fGlow_Buffer, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex) {
						hr |= iter.pDiffuseTex->Set_Resource(pShader, "g_DiffuseTexture", 0);
						hr |= m_pRect->Render(pShader, iter.iShaderPass);
					}
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(pShader, "g_DestTexture", 0);

					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel1->Render(j, pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2 != nullptr) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel2->Render(j, pShader, iter.iShaderPass);
						}
					}
				}
			}
											break;
			case CRenderer::EOrder::ALPHABLEND: {
				if (iter.bProj) {
					hr |= pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= pShader->Set_Resource("g_DepthTexture", pDepthSRV);
				hr |= pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_flowUV", &iter.fFlowUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_PlusColor", &iter.fPlusColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_fGlow_Buffer", &iter.fGlow_Buffer, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex) {
						hr |= iter.pDiffuseTex->Set_Resource(pShader, "g_DiffuseTexture", 0);
						hr |= m_pRect->Render(pShader, iter.iShaderPass);
					}
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(pShader, "g_DestTexture", 0);

					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel1->Render(j, pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2 != nullptr) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel2->Render(j, pShader, iter.iShaderPass);
						}
					}
				}
			}
											  break;
			case CRenderer::EOrder::GLOW: {
				if (iter.bProj) {
					hr |= pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_flowUV", &iter.fFlowUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_PlusColor", &iter.fPlusColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_fGlow_Buffer", &iter.fGlow_Buffer, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex) {
						hr |= iter.pDiffuseTex->Set_Resource(pShader, "g_DiffuseTexture", 0);
						hr |= m_pRect->Render(pShader, iter.iShaderPass);
					}
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(pShader, "g_DestTexture", 0);

					if (iter.pModel1 != nullptr) {
						for (_uint j = 0; j < iter.pModel1->Get_NumMesh(); ++j) {
							iter.pModel1->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel1->Render(j, pShader, iter.iShaderPass);
						}
					}
					if (iter.pModel2 != nullptr) {
						for (_uint j = 0; j < iter.pModel2->Get_NumMesh(); ++j) {
							iter.pModel2->Set_Resource(j, aiTextureType_DIFFUSE, pShader, "g_DiffuseTexture");
							iter.pModel2->Render(j, pShader, iter.iShaderPass);
						}
					}
					
				}
			}
										break;
			case CRenderer::EOrder::UI: {
				if (iter.bProj) {
					hr |= pShader->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&m_ViewMatrix), sizeof(_float4x4));
					hr |= pShader->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&m_ProjMatrix), sizeof(_float4x4));
				}
				_matrix Matrix = iter.pTransform->Get_Matrix() * m_pTransform->Get_Matrix();
				hr |= pShader->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_float4x4));
				hr |= pShader->Set_RawValue("g_TexCoord", &iter.fUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_FixTexCoord", &iter.fFixUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_flowUV", &iter.fFlowUV, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_TexCoordCut", &iter.fDivide, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fAlpha", &iter.fAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_Color", &iter.fColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_PlusColor", &iter.fPlusColor, sizeof(_float4));
				hr |= pShader->Set_RawValue("g_fGlow_Buffer", &iter.fGlow_Buffer, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fTime", &iter.fTime, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fScales", &iter.fShadeScale, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fScrollSpeeds", &iter.fScrollSpeeds, sizeof(_float3));
				hr |= pShader->Set_RawValue("g_fSmokeAlpha", &iter.fSmokeAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fDistortion1", &iter.fDistortion1, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion2", &iter.fDistortion2, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDistortion3", &iter.fDistortion3, sizeof(_float2));
				hr |= pShader->Set_RawValue("g_fDiscardAlpha", &iter.fDiscardAlpha, sizeof(_float));
				hr |= pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
				hr |= pShader->Set_Resource("g_DepthTexture", pDepthSRV);

				if (m_bCreate) {
					if (nullptr != iter.pDiffuseTex) {
						hr |= iter.pDiffuseTex->Set_Resource(pShader, "g_DiffuseTexture", 0);
						hr |= m_pRect->Render(pShader, iter.iShaderPass);
					}
					if (nullptr != iter.pNormalTex)
						hr |= iter.pNormalTex->Set_Resource(pShader, "g_NormalTexture", 0);
					if (nullptr != iter.pNoiseTex)
						hr |= iter.pNoiseTex->Set_Resource(pShader, "g_NoiseTexture", 0);
					if (nullptr != iter.pTempTex)
						hr |= iter.pTempTex->Set_Resource(pShader, "g_TempTexture", 0);
					if (nullptr != iter.pDestTex)
						hr |= iter.pDestTex->Set_Resource(pShader, "g_DestTexture", 0);

				}
			}
									  break;
			}
		}
	}
	return hr;
}

const HRESULT CEffect_Texture::Create_Effect(EffectTex sEffect) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	EffectTex seffecttex;
	HRESULT hr = S_OK;

	string dif{ sEffect.szDif };
	string nor{ sEffect.szNor };
	string noi{ sEffect.szNoise };
	string tmp{ sEffect.szTemp };
	string dst{ sEffect.szDest };
	string m1{ sEffect.szModel1 };
	string m2{ sEffect.szModel2 };
	wstring wdif{ dif.begin(), dif.end() };
	wstring wnor{ nor.begin(), nor.end() };
	wstring wnoi{ noi.begin(), noi.end() };
	wstring wtmp{ tmp.begin(), tmp.end() };
	wstring wdst{ dst.begin(), dst.end() };
	wstring wm1{ m1.begin(), m1.end() };
	wstring wm2{ m2.begin(), m2.end() };
	wdif = TEXT("Texture_") + wdif;
	wnor = TEXT("Texture_") + wnor;
	wnoi = TEXT("Texture_") + wnoi;
	wtmp = TEXT("Texture_") + wtmp;
	wdst = TEXT("Texture_") + wdst;
	wm1 = TEXT("Model_") + wm1;
	wm2 = TEXT("Model_") + wm2;

	wsprintf(m_TransformKey[m_iEffectIdx], TEXT("Transform%d"), m_iEffectIdx);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), m_TransformKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pTransform));
	wsprintf(m_DiffuseKey[m_iEffectIdx], TEXT("DiffuseTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wdif.c_str(), m_DiffuseKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pDiffuseTex));
	wsprintf(m_NormalKey[m_iEffectIdx], TEXT("NormalTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wnor.c_str(), m_NormalKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pNormalTex));
	wsprintf(m_NoiseKey[m_iEffectIdx], TEXT("NoiseTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wnoi.c_str(), m_NoiseKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pNoiseTex));
	wsprintf(m_TempKey[m_iEffectIdx], TEXT("TempTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wtmp.c_str(), m_TempKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pTempTex));
	wsprintf(m_DestKey[m_iEffectIdx], TEXT("DestTexture%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), wdst.c_str(), m_DestKey[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pDestTex));
	wsprintf(m_Model1Key[m_iEffectIdx], TEXT("Model1_%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), wm1.c_str(), m_Model1Key[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pModel1));
	wsprintf(m_Model2Key[m_iEffectIdx], TEXT("Model2_%d"), m_iEffectIdx);
	__super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), wm2.c_str(), m_Model2Key[m_iEffectIdx], reinterpret_cast<CComponent*&>(seffecttex.pModel2));
	++m_iEffectIdx;

	if (hr != E_FAIL) {
		m_bCreate = true;
		_char* diff = sEffect.szDif;
		_char* normal = sEffect.szNor;
		_char* noise = sEffect.szNoise;
		_char* temp = sEffect.szTemp;
		_char* dest = sEffect.szDest;
		_char* mo1 = sEffect.szModel1;
		_char* mo2 = sEffect.szModel2;
		m_sEffectTex = seffecttex;
		strcpy_s(m_sEffectTex.szDif, diff);
		strcpy_s(m_sEffectTex.szNor, normal);
		strcpy_s(m_sEffectTex.szNoise, noise);
		strcpy_s(m_sEffectTex.szTemp, temp);
		strcpy_s(m_sEffectTex.szDest, dest);
		strcpy_s(m_sEffectTex.szModel1, mo1);
		strcpy_s(m_sEffectTex.szModel2, mo2);
		m_vecEffect.push_back(m_sEffectTex);
	}
	else {
		Safe_Release(seffecttex.pDiffuseTex);
		Safe_Release(seffecttex.pNormalTex);
		Safe_Release(seffecttex.pNoiseTex);
		Safe_Release(seffecttex.pTempTex);
		Safe_Release(seffecttex.pDestTex);
		Safe_Release(seffecttex.pModel1);
		Safe_Release(seffecttex.pModel2);
		Safe_Release(seffecttex.pTransform);
	}
	return hr;
}

void CEffect_Texture::SaveEffect(const _tchar* const& pFilePath) {
	Json::Value root;
	root["EndTime"] = m_fMainEndTime;
	root["Loop"] = m_bRepeat;
	root["ProgressLoop"] = m_bProgressLoop;
	root["MainLook"] = m_bLookCam;
	root["Sorting"] = m_bSort;
	root["Z_Sorting"] = m_bZSort;
	root["sizeX"] = m_fSizeX;
	root["sizeY"] = m_fSizeY;
	root["PosX"] = m_fX;
	root["PosY"] = m_fY;

	for (auto& iter : m_vecEffect) {
		Json::Value node;

		node["Texture"][0] = string{ iter.szDif };
		node["Texture"][1] = string{ iter.szNor };
		node["Texture"][2] = string{ iter.szNoise };
		node["Texture"][3] = string{ iter.szTemp };
		node["Texture"][4] = string{ iter.szDest };
		node["Model"][0] = string{ iter.szModel1 };
		node["Model"][1] = string{ iter.szModel2 };

		node["CreateTime"] = iter.fCreateTime;
		node["DeleteTime"] = iter.fProgressTime;
		node["IsRandCreateTime"] = iter.bRandCreateTime;
		node["RandCreateTimeRange"]["Min"] = iter.fRandCreateTime.x;
		node["RandCreateTimeRange"]["Max"] = iter.fRandCreateTime.y;
		node["IsRandProgressTime"] = iter.bRandProgressTime;
		node["RandProgressTimeRange"]["Min"] = iter.fRandProgressTime.x;
		node["RandProgressTimeRange"]["Max"] = iter.fRandProgressTime.y;
		node["Proj"] = iter.bProj;
		node["Pos"]["x"] = iter.fPos.x;
		node["Pos"]["y"] = iter.fPos.y;
		node["Pos"]["z"] = iter.fPos.z;
		node["IsRandPos"] = iter.bRandPos;
		node["RandPos"][0]["x"] = iter.fRandPos[0].x;
		node["RandPos"][0]["y"] = iter.fRandPos[0].y;
		node["RandPos"][0]["z"] = iter.fRandPos[0].z;
		node["RandPos"][1]["x"] = iter.fRandPos[1].x;
		node["RandPos"][1]["y"] = iter.fRandPos[1].y;
		node["RandPos"][1]["z"] = iter.fRandPos[1].z;
		node["EndPos"]["x"] = iter.fEndPos.x;
		node["EndPos"]["y"] = iter.fEndPos.y;
		node["EndPos"]["z"] = iter.fEndPos.z;
		node["IsRandEndPos"] = iter.bRandomEndPos;
		node["RandPosMinRange"][0]["x"] = iter.fRandomPosRange[0].x;
		node["RandPosMinRange"][0]["y"] = iter.fRandomPosRange[0].y;
		node["RandPosMinRange"][0]["z"] = iter.fRandomPosRange[0].z;
		node["RandPosMaxRange"][1]["x"] = iter.fRandomPosRange[1].x;
		node["RandPosMaxRange"][1]["y"] = iter.fRandomPosRange[1].y;
		node["RandPosMaxRange"][1]["z"] = iter.fRandomPosRange[1].z;
		node["DirType"] = iter.bDirType;
		node["Dir"]["x"] = iter.fDir.x;
		node["Dir"]["y"] = iter.fDir.y;
		node["Dir"]["z"] = iter.fDir.z;
		node["IsRandDir"] = iter.bRandomDir;
		node["RandDirMinRange"][0]["x"] = iter.fRandomDirRange[0].x;
		node["RandDirMinRange"][0]["y"] = iter.fRandomDirRange[0].y;
		node["RandDirMinRange"][0]["z"] = iter.fRandomDirRange[0].z;
		node["RandDirMaxRange"][1]["x"] = iter.fRandomDirRange[1].x;
		node["RandDirMaxRange"][1]["y"] = iter.fRandomDirRange[1].y;
		node["RandDirMaxRange"][1]["z"] = iter.fRandomDirRange[1].z;
		node["IsLimitPos"] = iter.bLimitPos;
		node["LimitPos"][0]["x"] = iter.fLimitPos[0].x;
		node["LimitPos"][0]["y"] = iter.fLimitPos[0].y;
		node["LimitPos"][0]["z"] = iter.fLimitPos[0].z;
		node["LimitPos"][1]["x"] = iter.fLimitPos[1].x;
		node["LimitPos"][1]["y"] = iter.fLimitPos[1].y;
		node["LimitPos"][1]["z"] = iter.fLimitPos[1].z;

		node["Rot"]["x"] = iter.fRot.x;
		node["Rot"]["y"] = iter.fRot.y;
		node["Rot"]["z"] = iter.fRot.z;
		node["IsRandRot"] = iter.bRandRot;
		node["RandRot"][0]["x"] = iter.fRandRot[0].x;
		node["RandRot"][0]["y"] = iter.fRandRot[0].y;
		node["RandRot"][0]["z"] = iter.fRandRot[0].z;
		node["RandRot"][1]["x"] = iter.fRandRot[1].x;
		node["RandRot"][1]["y"] = iter.fRandRot[1].y;
		node["RandRot"][1]["z"] = iter.fRandRot[1].z;
		node["EndRot"]["x"] = iter.fEndRot.x;
		node["EndRot"]["y"] = iter.fEndRot.y;
		node["EndRot"]["z"] = iter.fEndRot.z;
		node["IsRandEndRot"] = iter.bRandomEndRot;
		node["RandRotMinRange"][0]["x"] = iter.fRandomRotRange[0].x;
		node["RandRotMinRange"][0]["y"] = iter.fRandomRotRange[0].y;
		node["RandRotMinRange"][0]["z"] = iter.fRandomRotRange[0].z;
		node["RandRotMaxRange"][1]["x"] = iter.fRandomRotRange[1].x;
		node["RandRotMaxRange"][1]["y"] = iter.fRandomRotRange[1].y;
		node["RandRotMaxRange"][1]["z"] = iter.fRandomRotRange[1].z;
		node["isRotX"][0] = iter.bRot[0];
		node["isRotX"][1] = iter.bRot[1];
		node["isRotX"][2] = iter.bRot[2];

		node["Scale"]["x"] = iter.fScale.x;
		node["Scale"]["y"] = iter.fScale.y;
		node["Scale"]["z"] = iter.fScale.z;
		node["IsRandScale"] = iter.bRandScale;
		node["RandScale"][0]["x"] = iter.fRandScale[0].x;
		node["RandScale"][0]["y"] = iter.fRandScale[0].y;
		node["RandScale"][0]["z"] = iter.fRandScale[0].z;
		node["RandScale"][1]["x"] = iter.fRandScale[1].x;
		node["RandScale"][1]["y"] = iter.fRandScale[1].y;
		node["RandScale"][1]["z"] = iter.fRandScale[1].z;
		node["EndScale"]["x"] = iter.fEndScale.x;
		node["EndScale"]["y"] = iter.fEndScale.y;
		node["EndScale"]["z"] = iter.fEndScale.z;
		node["IsRandEndScale"] = iter.bRandomEndScale;
		node["RandScaleMinRange"][0]["x"] = iter.fRandomScaleRange[0].x;
		node["RandScaleMinRange"][0]["y"] = iter.fRandomScaleRange[0].y;
		node["RandScaleMinRange"][0]["z"] = iter.fRandomScaleRange[0].z;
		node["RandScaleMaxRange"][1]["x"] = iter.fRandomScaleRange[1].x;
		node["RandScaleMaxRange"][1]["y"] = iter.fRandomScaleRange[1].y;
		node["RandScaleMaxRange"][1]["z"] = iter.fRandomScaleRange[1].z;
		node["Move"] = iter.bMove;

		node["PhysicsMove"] = iter.bPhysics;
		node["Accel"]["x"] = iter.vAccel.x;
		node["Accel"]["y"] = iter.vAccel.y;
		node["Accel"]["z"] = iter.vAccel.z;
		node["IsRandAccel"] = iter.bRandAccel;
		node["RandAccelMinRange"][0]["x"] = iter.vRandAccel[0].x;
		node["RandAccelMinRange"][0]["y"] = iter.vRandAccel[0].y;
		node["RandAccelMinRange"][0]["z"] = iter.vRandAccel[0].z;
		node["RandAccelMaxRange"][1]["x"] = iter.vRandAccel[1].x;
		node["RandAccelMaxRange"][1]["y"] = iter.vRandAccel[1].y;
		node["RandAccelMaxRange"][1]["z"] = iter.vRandAccel[1].z;
		node["SetAccel"]["x"] = iter.vSetAccel.x;
		node["SetAccel"]["y"] = iter.vSetAccel.y;
		node["SetAccel"]["z"] = iter.vSetAccel.z;
		node["Resist"]["x"] = iter.fResist.x;
		node["Resist"]["y"] = iter.fResist.y;
		node["IsRandResist"] = iter.bRandResist;
		node["RandResistMinRange"][0]["x"] = iter.fRandResist[0].x;
		node["RandResistMinRange"][0]["y"] = iter.fRandResist[0].y;
		node["RandResistMaxRange"][1]["x"] = iter.fRandResist[1].x;
		node["RandResistMaxRange"][1]["y"] = iter.fRandResist[1].y;
		node["ResistPower"] = iter.fResistP;
		node["IsRandResistPower"] = iter.bRandResistP;
		node["RandResistPowerMinRange"][0] = iter.fRandResistP[0];
		node["RandResistPowerMaxRange"][1] = iter.fRandResistP[1];
		node["isGravity"] = iter.bGravity;
		node["Gravity"] = iter.fGravity;

		for (auto& iter2 : iter.vecFrameAlpha) {
			Json::Value frame;
			frame["FrameTime"] = iter2.first;
			frame["Alpha"] = iter2.second;
			node["vecAlpha"].append(frame);
		}
		for (auto& iter2 : iter.vecFramePos) {
			Json::Value frame;
			frame["FrameTime"] = iter2.first;
			frame["Pos"]["x"] = iter2.second.x;
			frame["Pos"]["y"] = iter2.second.y;
			frame["Pos"]["z"] = iter2.second.z;
			node["vecPos"].append(frame);
		}
		for (auto& iter2 : iter.vecFrameDir) {
			Json::Value frame;
			frame["FrameTime"] = iter2.fDirTime;
			frame["Dir"]["x"] = iter2.fDir.x;
			frame["Dir"]["y"] = iter2.fDir.y;
			frame["Dir"]["z"] = iter2.fDir.z;
			frame["IsRandDirTime"] = iter2.bRandTime;
			frame["IsRandDir"] = iter2.bRandDir;
			frame["RandTime"]["x"] = iter2.fRandDirTimeRange.x;
			frame["RandTime"]["y"] = iter2.fRandDirTimeRange.y;
			frame["RandDir"][0]["x"] = iter2.fRandDirRange[0].x;
			frame["RandDir"][0]["y"] = iter2.fRandDirRange[0].y;
			frame["RandDir"][0]["z"] = iter2.fRandDirRange[0].z;
			frame["RandDir"][1]["x"] = iter2.fRandDirRange[1].x;
			frame["RandDir"][1]["y"] = iter2.fRandDirRange[1].y;
			frame["RandDir"][1]["z"] = iter2.fRandDirRange[1].z;
			node["vecDir"].append(frame);
		}
		for (auto& iter2 : iter.vecFrameScale) {
			Json::Value frame;
			frame["FrameTime"] = iter2.fScaleTime;
			frame["Scale"]["x"] = iter2.fScale.x;
			frame["Scale"]["y"] = iter2.fScale.y;
			frame["Scale"]["z"] = iter2.fScale.z;
			frame["IsRandTime"] = iter2.bRandTime;
			frame["IsRandScale"] = iter2.bRandScale;
			frame["RandTime"]["x"] = iter2.fRandScaleTimeRange.x;
			frame["RandTime"]["y"] = iter2.fRandScaleTimeRange.y;
			frame["RandScale"][0]["x"] = iter2.fRandScaleRange[0].x;
			frame["RandScale"][0]["y"] = iter2.fRandScaleRange[0].y;
			frame["RandScale"][0]["z"] = iter2.fRandScaleRange[0].z;
			frame["RandScale"][1]["x"] = iter2.fRandScaleRange[1].x;
			frame["RandScale"][1]["y"] = iter2.fRandScaleRange[1].y;
			frame["RandScale"][1]["z"] = iter2.fRandScaleRange[1].z;
			node["vecScale"].append(frame);
		}
		for (auto& iter2 : iter.vecFrameColor) {
			Json::Value frame;
			frame["FrameTime"] = iter2.first;
			frame["Color"]["x"] = iter2.second.x;
			frame["Color"]["y"] = iter2.second.y;
			frame["Color"]["z"] = iter2.second.z;
			frame["Color"]["w"] = iter2.second.w;
			node["vecColor"].append(frame);
		}

		node["ShaderIdx"] = iter.iShader;
		node["ShaderPass"] = iter.iShaderPass;
		node["ShaderColor"]["x"] = iter.fColor.x;
		node["ShaderColor"]["y"] = iter.fColor.y;
		node["ShaderColor"]["z"] = iter.fColor.z;
		node["ShaderColor"]["w"] = iter.fColor.w;
		node["ShaderPlusColor"]["x"] = iter.fPlusColor.x;
		node["ShaderPlusColor"]["y"] = iter.fPlusColor.y;
		node["ShaderPlusColor"]["z"] = iter.fPlusColor.z;
		node["ShaderPlusColor"]["w"] = iter.fPlusColor.w;
		node["Glow"]["x"] = iter.fGlow_Buffer.x;
		node["Glow"]["y"] = iter.fGlow_Buffer.y;
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
		node["Divide"]["x"] = iter.fDivide.x;
		node["Divide"]["y"] = iter.fDivide.y;
		node["FixUV"]["x"] = iter.fFixUV.x;
		node["FixUV"]["y"] = iter.fFixUV.y;
		node["Fix"] = iter.bFix;
		node["FlowUV"]["x"] = iter.fFlowUV.x;
		node["FlowUV"]["y"] = iter.fFlowUV.y;
		node["FrameSpeed"] = iter.fFrameSpeed;
		node["OtherSpeedFrame"] = iter.iOtherFrame;
		node["OtherSpeed"] = iter.fOtherSpeed;
		node["LookCam"] = iter.bLookCam;
		node["FrameLoop"] = iter.bFrameLoop;

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

void CEffect_Texture::LoadEffect(const _tchar* const& pFilePath) {
	wstring wname(&pFilePath[0]);
	string path = "../Bin/Resource/Effect/testeffect/";
	string pathdot = ".txt";
	wstring wpath = { path.begin(),path.end() };
	wstring wpathdot = { pathdot.begin(),pathdot.end() };
	wstring wfullpath = wpath + wname + wpathdot;

	ifstream ifs(wfullpath.c_str());
	if (ifs.fail())
		return;
	Json::Value root;
	ifs >> root;
	ifs.close();

	m_fMainEndTime = root["EndTime"].asFloat();
	m_bRepeat = root["Loop"].asBool();
	m_bProgressLoop = root["ProgressLoop"].asBool();
	m_bLookCam = root["MainLook"].asBool();
	m_bSort = root["Sorting"].asBool();
	m_bZSort = root["Z_Sorting"].asBool();
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
		strcpy_s(iter.szModel1, node["Model"][0].asString().c_str());
		strcpy_s(iter.szModel2, node["Model"][1].asString().c_str());

		iter.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
		string str;
		wstring wstr;
		str = iter.szDif;
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
		str = iter.szModel1;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Model_") + wstr;
		iter.pModel1 = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str(), nullptr));
		str = iter.szModel2;
		wstr.assign(str.begin(), str.end());
		wstr = TEXT("Model_") + wstr;
		iter.pModel2 = dynamic_cast<CModel*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str(), nullptr));


		// time
		iter.fCreateTime = node["CreateTime"].asFloat();
		iter.fProgressTime = node["DeleteTime"].asFloat();
		iter.bRandCreateTime = node["IsRandCreateTime"].asBool();
		iter.fRandCreateTime.x = node["RandCreateTimeRange"]["Min"].asFloat();
		iter.fRandCreateTime.y = node["RandCreateTimeRange"]["Max"].asFloat();
		iter.bRandProgressTime = node["IsRandProgressTime"].asBool();
		iter.fRandProgressTime.x = node["RandProgressTimeRange"]["Min"].asFloat();
		iter.fRandProgressTime.y = node["RandProgressTimeRange"]["Max"].asFloat();
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

		// randpos
		iter.bRandPos = node["IsRandPos"].asBool();
		iter.fRandPos[0].x = node["RandPos"][0]["x"].asFloat();
		iter.fRandPos[0].y = node["RandPos"][0]["y"].asFloat();
		iter.fRandPos[0].z = node["RandPos"][0]["z"].asFloat();
		iter.fRandPos[1].x = node["RandPos"][1]["x"].asFloat();
		iter.fRandPos[1].y = node["RandPos"][1]["y"].asFloat();
		iter.fRandPos[1].z = node["RandPos"][1]["z"].asFloat();

		iter.bRandomEndPos = node["IsRandEndPos"].asBool();
		iter.fRandomPosRange[0].x = node["RandPosMinRange"][0]["x"].asFloat();
		iter.fRandomPosRange[0].y = node["RandPosMinRange"][0]["y"].asFloat();
		iter.fRandomPosRange[0].z = node["RandPosMinRange"][0]["z"].asFloat();
		iter.fRandomPosRange[1].x = node["RandPosMaxRange"][1]["x"].asFloat();
		iter.fRandomPosRange[1].y = node["RandPosMaxRange"][1]["y"].asFloat();
		iter.fRandomPosRange[1].z = node["RandPosMaxRange"][1]["z"].asFloat();

		iter.bRandomDir = node["IsRandDir"].asBool();
		iter.fRandomDirRange[0].x = node["RandDirMinRange"][0]["x"].asFloat();
		iter.fRandomDirRange[0].y = node["RandDirMinRange"][0]["y"].asFloat();
		iter.fRandomDirRange[0].z = node["RandDirMinRange"][0]["z"].asFloat();
		iter.fRandomDirRange[1].x = node["RandDirMaxRange"][1]["x"].asFloat();
		iter.fRandomDirRange[1].y = node["RandDirMaxRange"][1]["y"].asFloat();
		iter.fRandomDirRange[1].z = node["RandDirMaxRange"][1]["z"].asFloat();

		iter.bLimitPos = node["IsLimitPos"].asBool();
		//iter.fLimitPos[0].x = node["LimitPos"]["x"].asFloat();
		//iter.fLimitPos[0].y = node["LimitPos"]["y"].asFloat();
		//iter.fLimitPos[0].z = node["LimitPos"]["z"].asFloat();
		
		iter.fLimitPos[0].x = node["LimitPos"][0]["x"].asFloat();
		iter.fLimitPos[0].y = node["LimitPos"][0]["y"].asFloat();
		iter.fLimitPos[0].z = node["LimitPos"][0]["z"].asFloat();
		iter.fLimitPos[1].x = node["LimitPos"][1]["x"].asFloat();
		iter.fLimitPos[1].y = node["LimitPos"][1]["y"].asFloat();
		iter.fLimitPos[1].z = node["LimitPos"][1]["z"].asFloat();

		// randrot
		iter.bRandRot = node["IsRandRot"].asBool();
		iter.fRandRot[0].x = node["RandRot"][0]["x"].asFloat();
		iter.fRandRot[0].y = node["RandRot"][0]["y"].asFloat();
		iter.fRandRot[0].z = node["RandRot"][0]["z"].asFloat();
		iter.fRandRot[1].x = node["RandRot"][1]["x"].asFloat();
		iter.fRandRot[1].y = node["RandRot"][1]["y"].asFloat();
		iter.fRandRot[1].z = node["RandRot"][1]["z"].asFloat();

		iter.bRandomEndRot = node["IsRandEndRot"].asBool();
		iter.fRandomRotRange[0].x = node["RandRotMinRange"][0]["x"].asFloat();
		iter.fRandomRotRange[0].y = node["RandRotMinRange"][0]["y"].asFloat();
		iter.fRandomRotRange[0].z = node["RandRotMinRange"][0]["z"].asFloat();
		iter.fRandomRotRange[1].x = node["RandRotMaxRange"][1]["x"].asFloat();
		iter.fRandomRotRange[1].y = node["RandRotMaxRange"][1]["y"].asFloat();
		iter.fRandomRotRange[1].z = node["RandRotMaxRange"][1]["z"].asFloat();

		// randscale
		iter.bRandScale = node["IsRandScale"].asBool();
		iter.fRandScale[0].x = node["RandScale"][0]["x"].asFloat();
		iter.fRandScale[0].y = node["RandScale"][0]["y"].asFloat();
		iter.fRandScale[0].z = node["RandScale"][0]["z"].asFloat();
		iter.fRandScale[1].x = node["RandScale"][1]["x"].asFloat();
		iter.fRandScale[1].y = node["RandScale"][1]["y"].asFloat();
		iter.fRandScale[1].z = node["RandScale"][1]["z"].asFloat();

		iter.bRandomEndScale = node["IsRandEndScale"].asBool();
		iter.fRandomScaleRange[0].x = node["RandScaleMinRange"][0]["x"].asFloat();
		iter.fRandomScaleRange[0].y = node["RandScaleMinRange"][0]["y"].asFloat();
		iter.fRandomScaleRange[0].z = node["RandScaleMinRange"][0]["z"].asFloat();
		iter.fRandomScaleRange[1].x = node["RandScaleMaxRange"][1]["x"].asFloat();
		iter.fRandomScaleRange[1].y = node["RandScaleMaxRange"][1]["y"].asFloat();
		iter.fRandomScaleRange[1].z = node["RandScaleMaxRange"][1]["z"].asFloat();

		//physics
		iter.bPhysics = node["PhysicsMove"].asBool();
		iter.vAccel.x = node["Accel"]["x"].asFloat();
		iter.vAccel.y = node["Accel"]["y"].asFloat();
		iter.vAccel.z = node["Accel"]["z"].asFloat();
		iter.vSetAccel.x = node["SetAccel"]["x"].asFloat();
		iter.vSetAccel.y = node["SetAccel"]["y"].asFloat();
		iter.vSetAccel.z = node["SetAccel"]["z"].asFloat();
		iter.bRandAccel = node["IsRandAccel"].asBool();
		iter.vRandAccel[0].x = node["RandAccelMinRange"][0]["x"].asFloat();
		iter.vRandAccel[0].y = node["RandAccelMinRange"][0]["y"].asFloat();
		iter.vRandAccel[0].z = node["RandAccelMinRange"][0]["z"].asFloat();
		iter.vRandAccel[1].x = node["RandAccelMaxRange"][1]["x"].asFloat();
		iter.vRandAccel[1].y = node["RandAccelMaxRange"][1]["y"].asFloat();
		iter.vRandAccel[1].z = node["RandAccelMaxRange"][1]["z"].asFloat();

		if (iter.bRandAccel) {
			iter.vSetAccel.x = (iter.vRandAccel[1].x - iter.vRandAccel[0].x) * rand() / RAND_MAX + iter.vRandAccel[0].x;
			iter.vSetAccel.y = (iter.vRandAccel[1].y - iter.vRandAccel[0].y) * rand() / RAND_MAX + iter.vRandAccel[0].y;
			iter.vSetAccel.z = (iter.vRandAccel[1].z - iter.vRandAccel[0].z) * rand() / RAND_MAX + iter.vRandAccel[0].z;
		}

		iter.fResist.x = node["Resist"]["x"].asFloat();
		iter.fResist.y = node["Resist"]["y"].asFloat();
		iter.bRandResist = node["IsRandResist"].asBool();
		iter.fRandResist[0].x = node["RandResistMinRange"][0]["x"].asFloat();
		iter.fRandResist[0].y = node["RandResistMinRange"][0]["y"].asFloat();
		iter.fRandResist[1].x = node["RandResistMaxRange"][1]["x"].asFloat();
		iter.fRandResist[1].y = node["RandResistMaxRange"][1]["y"].asFloat();

		iter.fResistP = node["ResistPower"].asFloat();
		iter.bRandResistP = node["IsRandResistPower"].asBool();
		iter.fRandResistP[0] = node["RandResistPowerMinRange"][0].asFloat();
		iter.fRandResistP[1] = node["RandResistPowerMaxRange"][1].asFloat();
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
			DirKeyFrame iter2;
			iter2.fDirTime = frame["FrameTime"].asFloat();
			iter2.fDir.x = frame["Dir"]["x"].asFloat();
			iter2.fDir.y = frame["Dir"]["y"].asFloat();
			iter2.fDir.z = frame["Dir"]["z"].asFloat();
			iter2.bRandTime = frame["IsRandDirTime"].asBool();
			iter2.bRandDir = frame["IsRandDir"].asBool();
			iter2.fRandDirTimeRange.x = frame["RandTime"]["x"].asFloat();
			iter2.fRandDirTimeRange.y = frame["RandTime"]["y"].asFloat();
			iter2.fRandDirRange[0].x = frame["RandDir"][0]["x"].asFloat();
			iter2.fRandDirRange[0].y = frame["RandDir"][0]["y"].asFloat();
			iter2.fRandDirRange[0].z = frame["RandDir"][0]["z"].asFloat();
			iter2.fRandDirRange[1].x = frame["RandDir"][1]["x"].asFloat();
			iter2.fRandDirRange[1].y = frame["RandDir"][1]["y"].asFloat();
			iter2.fRandDirRange[1].z = frame["RandDir"][1]["z"].asFloat();
			iter.vecFrameDir.push_back(iter2);
		}
		//Scale
		for (auto& frame : node["vecScale"]) {
			ScaleKeyFrame iter2;
			iter2.fScaleTime = frame["FrameTime"].asFloat();
			iter2.fScale.x = frame["Scale"]["x"].asFloat();
			iter2.fScale.y = frame["Scale"]["y"].asFloat();
			iter2.fScale.z = frame["Scale"]["z"].asFloat();
			iter2.bRandTime = frame["IsRandTime"].asBool();
			iter2.bRandScale = frame["IsRandScale"].asBool();
			iter2.fRandScaleTimeRange.x = frame["RandTime"]["x"].asFloat();
			iter2.fRandScaleTimeRange.y = frame["RandTime"]["y"].asFloat();
			iter2.fRandScaleRange[0].x = frame["RandScale"][0]["x"].asFloat();
			iter2.fRandScaleRange[0].y = frame["RandScale"][0]["y"].asFloat();
			iter2.fRandScaleRange[0].z = frame["RandScale"][0]["z"].asFloat();
			iter2.fRandScaleRange[1].x = frame["RandScale"][1]["x"].asFloat();
			iter2.fRandScaleRange[1].y = frame["RandScale"][1]["y"].asFloat();
			iter2.fRandScaleRange[1].z = frame["RandScale"][1]["z"].asFloat();
			iter.vecFrameScale.push_back(iter2);
		}
		//Color
		for (auto& frame : node["vecColor"]) {
			pair<_float, _float4> iter2;
			iter2.first = frame["FrameTime"].asFloat();
			iter2.second.x = frame["Color"]["x"].asFloat();
			iter2.second.y = frame["Color"]["y"].asFloat();
			iter2.second.z = frame["Color"]["z"].asFloat();
			iter2.second.w = frame["Color"]["w"].asFloat();
			iter.vecFrameColor.push_back(iter2);
		}

		// shader
		iter.fDivide.x = node["Divide"]["x"].asFloat();
		iter.fDivide.y = node["Divide"]["y"].asFloat();
		iter.fFixUV.x = node["FixUV"]["x"].asFloat();
		iter.fFixUV.y = node["FixUV"]["y"].asFloat();
		iter.bFix = node["Fix"].asBool();
		iter.fFlowUV.x = node["FlowUV"]["x"].asFloat();
		iter.fFlowUV.y = node["FlowUV"]["y"].asFloat();
		iter.fFrameSpeed = node["FrameSpeed"].asFloat();
		iter.iOtherFrame = node["OtherSpeedFrame"].asInt();
		iter.fOtherSpeed = node["OtherSpeed"].asFloat();
		iter.bMove = node["Move"].asBool();
		iter.iShader = node["ShaderIdx"].asInt();
		iter.iShaderPass = node["ShaderPass"].asInt();
		iter.fColor.x = node["ShaderColor"]["x"].asFloat();
		iter.fColor.y = node["ShaderColor"]["y"].asFloat();
		iter.fColor.z = node["ShaderColor"]["z"].asFloat();
		iter.fColor.w = node["ShaderColor"]["w"].asFloat();
		iter.fPlusColor.x = node["ShaderPlusColor"]["x"].asFloat();
		iter.fPlusColor.y = node["ShaderPlusColor"]["y"].asFloat();
		iter.fPlusColor.z = node["ShaderPlusColor"]["z"].asFloat();
		iter.fPlusColor.w = node["ShaderPlusColor"]["w"].asFloat();
		iter.fGlow_Buffer.x = node["Glow"]["x"].asFloat();
		iter.fGlow_Buffer.y = node["Glow"]["y"].asFloat();
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
		iter.bFrameLoop = node["FrameLoop"].asBool();
		m_bCreate = true;
		m_bStart = true;

		iter.fTime = 0.f;
		if (iter.bRandCreateTime)
			iter.fCreateTime = (iter.fRandCreateTime.y - iter.fRandCreateTime.x) * rand() / RAND_MAX + iter.fRandCreateTime.x;
		if (iter.bRandProgressTime)
			iter.fProgressTime = (iter.fRandProgressTime.y - iter.fRandProgressTime.x) * rand() / RAND_MAX + iter.fRandProgressTime.x;

		if (iter.bRandPos) {
			iter.fPos.x = (iter.fRandPos[1].x - iter.fRandPos[0].x) * rand() / RAND_MAX + iter.fRandPos[0].x;
			iter.fPos.y = (iter.fRandPos[1].y - iter.fRandPos[0].y) * rand() / RAND_MAX + iter.fRandPos[0].y;
			iter.fPos.z = (iter.fRandPos[1].z - iter.fRandPos[0].z) * rand() / RAND_MAX + iter.fRandPos[0].z;
		}
		//if (iter.bProj)
		//	iter.pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(iter.fPos.x - g_iWinSizeX * 0.5f, iter.fPos.y + g_iWinSizeY * 0.5f, 0.1f, 1.f));
		//else
		iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

		if (iter.bRandRot) {
			iter.fRot.x = (iter.fRandRot[1].x - iter.fRandRot[0].x) * rand() / RAND_MAX + iter.fRandRot[0].x;
			iter.fRot.y = (iter.fRandRot[1].y - iter.fRandRot[0].y) * rand() / RAND_MAX + iter.fRandRot[0].y;
			iter.fRot.z = (iter.fRandRot[1].z - iter.fRandRot[0].z) * rand() / RAND_MAX + iter.fRandRot[0].z;
		}
		if (iter.bRot[0])
			iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
		if (iter.bRot[1])
			iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
		if (iter.bRot[2])
			iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));

		if (iter.bRandScale) {
			iter.fScale.x = (iter.fRandScale[1].x - iter.fRandScale[0].x) * rand() / RAND_MAX + iter.fRandScale[0].x;
			iter.fScale.y = (iter.fRandScale[1].y - iter.fRandScale[0].y) * rand() / RAND_MAX + iter.fRandScale[0].y;
			iter.fScale.z = (iter.fRandScale[1].z - iter.fRandScale[0].z) * rand() / RAND_MAX + iter.fRandScale[0].z;
		}
		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));

		if (iter.bRandomEndPos) {
			iter.fEndPos.x = (iter.fRandomPosRange[1].x - iter.fRandomPosRange[0].x) * rand() / RAND_MAX + iter.fRandomPosRange[0].x;
			iter.fEndPos.y = (iter.fRandomPosRange[1].y - iter.fRandomPosRange[0].y) * rand() / RAND_MAX + iter.fRandomPosRange[0].y;
			iter.fEndPos.z = (iter.fRandomPosRange[1].z - iter.fRandomPosRange[0].z) * rand() / RAND_MAX + iter.fRandomPosRange[0].z;
		}
		if (iter.bRandomDir) {
			iter.fDir.x = (iter.fRandomDirRange[1].x - iter.fRandomDirRange[0].x) * rand() / RAND_MAX + iter.fRandomDirRange[0].x;
			iter.fDir.y = (iter.fRandomDirRange[1].y - iter.fRandomDirRange[0].y) * rand() / RAND_MAX + iter.fRandomDirRange[0].y;
			iter.fDir.z = (iter.fRandomDirRange[1].z - iter.fRandomDirRange[0].z) * rand() / RAND_MAX + iter.fRandomDirRange[0].z;
		}
		for (auto& frame : iter.vecFrameDir) {
			if (frame.bRandTime)
				frame.fDirTime = (frame.fRandDirTimeRange.y - frame.fRandDirTimeRange.x) * rand() / RAND_MAX + frame.fRandDirTimeRange.x;
			if (frame.bRandDir)
				frame.fDir = {
				(frame.fRandDirRange[1].x - frame.fRandDirRange[0].x) * rand() / RAND_MAX + frame.fRandDirRange[0].x,
				(frame.fRandDirRange[1].y - frame.fRandDirRange[0].y) * rand() / RAND_MAX + frame.fRandDirRange[0].y,
				(frame.fRandDirRange[1].z - frame.fRandDirRange[0].z) * rand() / RAND_MAX + frame.fRandDirRange[0].z
			};
		}
		if (iter.bRandomEndRot) {
			iter.fEndRot.x = (iter.fRandomRotRange[1].x - iter.fRandomRotRange[0].x) * rand() / RAND_MAX + iter.fRandomRotRange[0].x;
			iter.fEndRot.y = (iter.fRandomRotRange[1].y - iter.fRandomRotRange[0].y) * rand() / RAND_MAX + iter.fRandomRotRange[0].y;
			iter.fEndRot.z = (iter.fRandomRotRange[1].z - iter.fRandomRotRange[0].z) * rand() / RAND_MAX + iter.fRandomRotRange[0].z;
		}
		if (iter.bRandomEndScale) {
			iter.fEndScale.x = (iter.fRandomScaleRange[1].x - iter.fRandomScaleRange[0].x) * rand() / RAND_MAX + iter.fRandomScaleRange[0].x;
			iter.fEndScale.y = (iter.fRandomScaleRange[1].y - iter.fRandomScaleRange[0].y) * rand() / RAND_MAX + iter.fRandomScaleRange[0].y;
			iter.fEndScale.z = (iter.fRandomScaleRange[1].z - iter.fRandomScaleRange[0].z) * rand() / RAND_MAX + iter.fRandomScaleRange[0].z;
		}
		for (auto& frame : iter.vecFrameScale) {
			if (frame.bRandTime)
				frame.fScaleTime = (frame.fRandScaleTimeRange.y - frame.fRandScaleTimeRange.x) * rand() / RAND_MAX + frame.fRandScaleTimeRange.x;
			if (frame.bRandScale)
				frame.fScale = {
				(frame.fRandScaleRange[1].x - frame.fRandScaleRange[0].x) * rand() / RAND_MAX + frame.fRandScaleRange[0].x,
				(frame.fRandScaleRange[1].y - frame.fRandScaleRange[0].y) * rand() / RAND_MAX + frame.fRandScaleRange[0].y,
				(frame.fRandScaleRange[1].z - frame.fRandScaleRange[0].z) * rand() / RAND_MAX + frame.fRandScaleRange[0].z
			};
		}
		iter.fAlphaTime = 0.f;
		iter.fAlpha = 0.1f;
		iter.fUV.x = 0;
		iter.fUV.y = iter.fsetUVY;
		iter.iSetFrame = 0;

		if (iter.bRandAccel) {
			iter.vSetAccel.x = (iter.vRandAccel[1].x - iter.vRandAccel[0].x) * rand() / RAND_MAX + iter.vRandAccel[0].x;
			iter.vSetAccel.y = (iter.vRandAccel[1].y - iter.vRandAccel[0].y) * rand() / RAND_MAX + iter.vRandAccel[0].y;
			iter.vSetAccel.z = (iter.vRandAccel[1].z - iter.vRandAccel[0].z) * rand() / RAND_MAX + iter.vRandAccel[0].z;
		}
		iter.vAccel = iter.vSetAccel;

		if (iter.bRandResist) {
			iter.fResist.y = (iter.fRandResist[1].y - iter.fRandResist[0].y) * rand() / RAND_MAX + iter.fRandResist[0].y;
		}
		iter.fResist.x = iter.fResist.y;

		if (iter.bRandResistP)
			iter.fResistP = (iter.fRandResistP[1] - iter.fRandResistP[0]) * rand() / RAND_MAX + iter.fRandResistP[0];

		iter.fGa = 0.f;
		iter.bRender = false;
		for (auto& arr : iter.iKeyFrame)
			arr = 0;

		m_dMainTime = 0.;

		m_vecEffect.emplace_back(iter);
	}

	//sort(m_vecEffect.begin(), m_vecEffect.end(), [](EffectTex pSour, EffectTex pDest) {
	//	return pSour.fPos.z > pDest.fPos.z;
	//	});
}

