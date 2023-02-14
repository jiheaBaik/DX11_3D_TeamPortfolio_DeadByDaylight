#include "pch.h"
#include "Effect_Blood_Free.h"

CEffect_Blood_Free* const CEffect_Blood_Free::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEffect_Blood_Free* pInstnace = new CEffect_Blood_Free(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEffect_Blood_Free::Clone(void* const& pArg) {
	CEffect_Blood_Free* pInstnace = new CEffect_Blood_Free(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEffect_Blood_Free::CEffect_Blood_Free(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEffect_Blood_Free::CEffect_Blood_Free(const CEffect_Blood_Free& rhs)
	: CObject(rhs),
	m_vecEffect(rhs.m_vecEffect),
	m_ViewMatrix(rhs.m_ViewMatrix),
	m_ProjMatrix(rhs.m_ProjMatrix),
	m_fTime(rhs.m_fTime),
	m_fMainEndTime(rhs.m_fMainEndTime),
	m_iRenderIdx(rhs.m_iRenderIdx),
	m_bRepeat(rhs.m_bRepeat),
	m_bProgressLoop(rhs.m_bProgressLoop),
	m_bLookCam(rhs.m_bLookCam){	
}
void CEffect_Blood_Free::Delete(void) {
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

const HRESULT CEffect_Blood_Free::Init_Create(void) {
	return S_OK;
}

const HRESULT CEffect_Blood_Free::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelEffect"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));

	LoadEffect();

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	for (auto& iter : m_vecEffect) {
		iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMLoadFloat3(&iter.fRight));
		iter.pTransform->Set_Row(CTransform::ERow::UP, XMLoadFloat3(&iter.fUp));
		iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMLoadFloat3(&iter.fLook));
		iter.pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&iter.fPos));
		iter.pTransform->Set_Scale(XMLoadFloat3(&iter.fScale));
	}
	m_bCreate = true;
	m_bStart = true;
	return hr;
}

const _ubyte CEffect_Blood_Free::Update(const _double& dTimeDelta) {
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
						}
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
					iter.bRender = false;
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
						if (iter.bRot[0])
							iter.pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vRot.x));
						if (iter.bRot[1])
							iter.pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vRot.y));
						if (iter.bRot[2])
							iter.pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(vRot.z));

						_vector vScale = XMVectorLerp(XMLoadFloat3(&iter.fScale), XMLoadFloat3(&iter.fEndScale), fRatio);
						iter.pTransform->Set_Scale(vScale);
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

void CEffect_Blood_Free::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEffect_Blood_Free::Late_Update(const _double& dTimeDelta) {
	if (nullptr == m_pRenderer)
		return;
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	_vector vCameraPos = pCamera->Get_Pos();
	for (auto& iter : m_vecEffect) {
		//if (m_bLookCam) {
			_float3 scale = m_pTransform->Get_Scale();
			_vector vLook = XMVector3Normalize((m_pTransform->Get_Matrix()).r[3] - vCameraPos);
			XMVectorSetY(vLook, 0.f);
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, vRight);
			m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
			m_pTransform->Set_Row(CTransform::ERow::LOOK, vLook);
			m_pTransform->Set_Scale(XMLoadFloat3(&scale));
		//}
	}
	m_pRenderer->Add_Object(static_cast<CRenderer::EOrder>(m_iRenderIdx), this);
}

const HRESULT CEffect_Blood_Free::Render(const CRenderer::EOrder& eOrder) {
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

void CEffect_Blood_Free::LoadEffect() {
	_tchar* filepath = TEXT("");
	_uint iRandBlood = rand() % 5;

	switch (iRandBlood)	{
	case 0:
		filepath = TEXT("../Bin/Resource/03.Gameplay/Effect/3DFrameBlood0.txt");
		break;
	case 1:
		filepath = TEXT("../Bin/Resource/03.Gameplay/Effect/3DFrameBlood1.txt");
		break;
	case 2:
		filepath = TEXT("../Bin/Resource/03.Gameplay/Effect/3DFrameBlood2.txt");
		break;
	case 3:
		filepath = TEXT("../Bin/Resource/03.Gameplay/Effect/3DFrameBlood3.txt");
		break;
	case 4:
		filepath = TEXT("../Bin/Resource/03.Gameplay/Effect/3DFrameBlood4.txt");
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
	m_fSizeX = root["sizeX"].asFloat();
	m_fSizeY = root["sizeY"].asFloat();
	m_fX = root["PosX"].asFloat();
	m_fY = root["PosY"].asFloat();
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
		iter.bSetColor = node["SetColor"].asBool();
		
		iter.fRight = _float3(node["Right"]["x"].asFloat(), node["Right"]["y"].asFloat(), node["Right"]["z"].asFloat());
		iter.fUp = _float3(node["Up"]["x"].asFloat(), node["Up"]["y"].asFloat(), node["Up"]["z"].asFloat());
		iter.fLook = _float3(node["Look"]["x"].asFloat(), node["Look"]["y"].asFloat(), node["Look"]["z"].asFloat());

		_vector right = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::RIGHT));
		_vector up = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::UP));
		_vector look = XMVector3Normalize(iter.pTransform->Get_Row(CTransform::ERow::LOOK));
		_matrix matrixRotX = XMMatrixRotationAxis(right, XMConvertToRadians(iter.fRot.x));
		iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRotX));
		iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRotX));
		iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRotX));
		_matrix matrixRotY = XMMatrixRotationAxis(up, XMConvertToRadians(iter.fRot.y));
		iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRotY));
		iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRotY));
		iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRotY));
		_matrix matrixRotZ = XMMatrixRotationAxis(look, XMConvertToRadians(iter.fRot.z));
		iter.pTransform->Set_Row(CTransform::ERow::RIGHT, XMVector3TransformNormal(right, matrixRotZ));
		iter.pTransform->Set_Row(CTransform::ERow::UP, XMVector3TransformNormal(up, matrixRotZ));
		iter.pTransform->Set_Row(CTransform::ERow::LOOK, XMVector3TransformNormal(look, matrixRotZ));

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

void CEffect_Blood_Free::Update_Network(const string& pkey) {
}

void CEffect_Blood_Free::Update_Network(void) {
}
