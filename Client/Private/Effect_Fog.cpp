#include "pch.h"
#include "Effect_Fog.h"

CEffect_Fog* const CEffect_Fog::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, const _tchar* const& pFilePath) {
	CEffect_Fog* pInstnace = new CEffect_Fog(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create(pFilePath)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect* const CEffect_Fog::Clone(void* const& pArg) {
	CEffect_Fog* pInstnace = new CEffect_Fog(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect_Fog::CEffect_Fog(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CEffect(pDevice, pContext, pNetwork) {
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}

CEffect_Fog::CEffect_Fog(const CEffect_Fog& rhs)
	: CEffect(rhs),
	m_vecEffect(rhs.m_vecEffect),
	m_ViewMatrix(rhs.m_ViewMatrix),
	m_ProjMatrix(rhs.m_ProjMatrix),
	m_fMainEndTime(rhs.m_fMainEndTime),
	m_iRenderIdx(rhs.m_iRenderIdx),
	m_bRepeat(rhs.m_bRepeat),
	m_bProgressLoop(rhs.m_bProgressLoop),
	m_bLookCam(rhs.m_bLookCam) {
	for (auto& iter : m_vecEffect) {
		Safe_AddRef(iter.pDiffuseTex);
		Safe_AddRef(iter.pNormalTex);
		Safe_AddRef(iter.pNoiseTex);
		Safe_AddRef(iter.pTempTex);
		Safe_AddRef(iter.pDestTex);
	}
}

void CEffect_Fog::Delete(void) {
	__super::Delete();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pAnimShader);
	Safe_Release(m_pRect);
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

const HRESULT CEffect_Fog::Init_Create(const _tchar* const& pFilePath) {
	LoadEffect(pFilePath);
	return S_OK;
}

const HRESULT CEffect_Fog::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Effect"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelEffect"), TEXT("ModelShader"), reinterpret_cast<CComponent*&>(m_pAnimShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Rect"), reinterpret_cast<CComponent*&>(m_pRect));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	for (auto& iter : m_vecEffect) {
		iter.pTransform = dynamic_cast<CTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), nullptr));
		iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));

		if (iter.bRot[0])
			iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.x));
		if (iter.bRot[1])
			iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(iter.fRot.y));
		if (iter.bRot[2])
			iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(iter.fRot.z));

		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
	}
	InIt();
	m_bCreate = true;
	m_bStart = true;
	m_pTarget_Depth = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"));
	return hr;
}

const _ubyte CEffect_Fog::Update(const _double& dTimeDelta) {
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
						/*iter.fAlpha = 0.1f;*/
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
						//iter.bRender = false;
						for (auto& arr : iter.iKeyFrame)
							arr = 0;
					}
					if (&iter == &m_vecEffect.back())
						m_dMainTime = 0.;
					continue;
				}
				else if (!m_bRepeat && (m_dMainTime > m_fMainEndTime)) {
					m_dMainTime = static_cast<_double>(m_fMainEndTime);
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
					//iter.bRender = false;
					continue;
				}

				if (m_dMainTime > iter.fCreateTime) {
					_float ratioTime;
					//iter.bRender = true;
					iter.fTime += static_cast<_float>(dTimeDelta);
					iter.fFrameTime += static_cast<_float>(dTimeDelta);
					ratioTime = iter.fTime;
					if (iter.fTime > iter.fProgressTime) {
						//iter.fTime = iter.fDeleteTime;
						//iter.bRender = false;
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
								//iter.fAlpha = 0.1f;
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
								//iter.bRender = false;
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
										iter.fDir.x = 0;
									vPos += XMLoadFloat3(&iter.fDir) * static_cast<_float>(dTimeDelta);
									iter.pTransform->Set_Row(CTransform::ERow::POS, vPos);
								}
							}
						}

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

						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(iter.fEndRot.x), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::UP), XMConvertToRadians(iter.fEndRot.y), dTimeDelta);
						iter.pTransform->Rotate(iter.pTransform->Get_Row(CTransform::ERow::LOOK), XMConvertToRadians(iter.fEndRot.z), dTimeDelta);

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
	if (m_bEffectEnd) {
		m_SetEndTime += static_cast<_float>(dTimeDelta);
		for (auto& iter : m_vecEffect) {
			iter.fAlpha = m_SetEndTime;
			if (&iter == &m_vecEffect.back() && iter.fAlpha > 1.f)
				return OBJ_ERASE;
		}
	}
	return OBJ_NOEVENT;
}

void CEffect_Fog::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEffect_Fog::Late_Update(const _double& dTimeDelta) {
	if (nullptr == m_pRenderer)
		return;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	_vector vCameraPos = pCamera->Get_Pos();

	_float3 scale = m_pTransform->Get_Scale();
	_vector vLook = XMVector3Normalize(m_pTransform->Get_Row(CTransform::ERow::POS) - vCameraPos);
	XMVectorSetY(vLook, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	m_pTransform->Set_Row(CTransform::ERow::RIGHT, vRight);
	m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransform->Set_Row(CTransform::ERow::LOOK, vLook);
	m_pTransform->Set_Scale(XMLoadFloat3(&scale));

	_float fLength = m_pTransform->Get_Dist(vCameraPos);
	
	_float fMin = 2.f, fMax = 5.f;
	_float fRatio = (fLength - fMin) / (fMax - fMin);
	//fCameraDist = (fCameraDist - g_fCameraDist.x) / (g_fCameraDist.y - g_fCameraDist.x);
	//_float fRatio = fLength / 10.f;
	if (fRatio < 0.f)
		fRatio = 0.f;
	if(fRatio > 1.f)
		fRatio = 1.f;


	for (auto& iter : m_vecEffect) {
		m_fColorAlpha = { iter.fColor.x,iter.fColor.y,iter.fColor.z,iter.fColor.w * fRatio };
		if (fRatio == 0.f)
			iter.bRender = false;
		else
			iter.bRender = true;
	}

	m_pRenderer->Add_Object(static_cast<CRenderer::EOrder>(m_iRenderIdx), this);
}

const HRESULT CEffect_Fog::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	CShader* pShader = nullptr;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));

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
				hr |= pShader->Set_RawValue("g_Color", &m_fColorAlpha, sizeof(_float4));
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
				hr |= pShader->Set_RawValue("g_Color", &m_fColorAlpha, sizeof(_float4));
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
				hr |= pShader->Set_RawValue("g_Color", &m_fColorAlpha, sizeof(_float4));
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
				hr |= pShader->Set_RawValue("g_Color", &m_fColorAlpha, sizeof(_float4));
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
				hr |= pShader->Set_RawValue("g_Color", &m_fColorAlpha, sizeof(_float4));
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

void CEffect_Fog::LoadEffect(const _tchar* const& pFilePath) {
	wstring wname(&pFilePath[0]);
	ifstream ifs(wname.c_str());
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
		iter.fFrameSpeed = node["FrameSpeed"].asFloat();
		iter.iOtherFrame = node["OtherSpeedFrame"].asInt();
		iter.fOtherSpeed = node["OtherSpeed"].asFloat();
		iter.bMove = node["Move"].asBool();
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
		//iter.fAlpha = 0.1f;
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

	sort(m_vecEffect.begin(), m_vecEffect.end(), [](EffectTex pSour, EffectTex pDest) {
		return pSour.fPos.z > pDest.fPos.z;
		});
}

void CEffect_Fog::InIt() {
	for (auto& iter : m_vecEffect) {
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
		//iter.fAlpha = 0.1f;
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
		//iter.bRender = false;
		for (auto& arr : iter.iKeyFrame)
			arr = 0;
	}
}

void CEffect_Fog::Set_RepeatEnd() {
	m_bEffectEnd = true;
}


void CEffect_Fog::Set_Pos(_fvector pos) {
	m_pTransform->Set_Row(CTransform::ERow::POS, pos);
}

const _double& CEffect_Fog::Get_Time(){
	return m_dMainTime;
}

