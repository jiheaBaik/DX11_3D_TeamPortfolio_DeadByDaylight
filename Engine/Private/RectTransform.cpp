#include "RectTransform.h"
#include "Game_Instance.h"
#include "Object.h"

_float CRectTransform::s_fCanvasScl = 1.f;

void CRectTransform::Set_CanvasScl(const _float& fScale) {
	s_fCanvasScl = fScale;
}

CRectTransform* const CRectTransform::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CRectTransform* pInstance = new CRectTransform(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CRectTransform::Clone(void* const& pArg) {
	CRectTransform* pInstance = new CRectTransform(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CRectTransform::CRectTransform(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CTransform(pDevice, pContext),
	m_pParent(nullptr)
{
	ZeroMemory(&m_sDesc, sizeof(SDesc));
}

CRectTransform::CRectTransform(const CRectTransform& rhs)
	: CTransform(rhs),
	m_sDesc(rhs.m_sDesc),
	m_vWinSize(rhs.m_vWinSize),
	m_pParent(rhs.m_pParent), 
	m_bSetPre(rhs.m_bSetPre)
{
}

void CRectTransform::Delete(void) {
	__super::Delete();
	//Safe_Release(m_pParent);
}

const HRESULT CRectTransform::Init_Create(void) {
	if (nullptr == m_pContext)
		return E_FAIL;
	_uint iNumViewport = 1;
	D3D11_VIEWPORT	tViewPort;
	m_pContext->RSGetViewports(&iNumViewport, &tViewPort);
	m_vWinSize.x = tViewPort.Width;
	m_vWinSize.y = tViewPort.Height;

	_float fScale = (1.f / 1920.f) * m_vWinSize.x;
	this->Set_CanvasScl(fScale);
	//this->Set_CanvasScl(1.f);

	m_sDesc.Reset();
	return S_OK;
}

const HRESULT CRectTransform::Init_Clone(void* const& pArg) {
	if (FAILED(__super::Init_Clone(pArg)))
		return E_FAIL;
	if (nullptr != pArg) 
		memcpy(&m_sDesc, pArg, sizeof(SDesc));
	// init
	this->Set_Parent(nullptr);
	//this->Update_Desc();	
	return S_OK;
}

void CRectTransform::Update_Edit(const _int& iPushId) {
	ImGui::Separator();
	if (nullptr == m_pParent)
		ImGui::InputFloat("Canvas Scale", &s_fCanvasScl, 0.1f);
	if (ImGui::TreeNodeEx("RectTransform", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginPopupContextItem("Reset", ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::MenuItem("Reset")) {
				m_sDesc.Reset();
				this->Set_Desc(m_sDesc);
			}
			ImGui::EndPopup();
		}

		_float vPos[3];
		memcpy(vPos, &m_sDesc.vPosition, sizeof(_float3));
		if (ImGui::DragFloat3("Pos", vPos, 0.01f))
			this->Set_State(EState::POS, _float3(vPos[0], vPos[1], vPos[2]));

		_float vSize[2] = { m_sDesc.fWidth, m_sDesc.fHeight };
		if (ImGui::DragFloat2("Size", vSize, 0.01f, 0.0f))
			this->Set_Size(vSize[0], vSize[1]);

		_float vAnchor[2];
		memcpy(vAnchor, &m_sDesc.vAnchor, sizeof(_float2));
		if (ImGui::DragFloat2("Anchor", vAnchor, 0.01f, 0.0f, 1.0f))
			this->Set_Anchor(_float2(vAnchor[0], vAnchor[1]));

		_float vPivot[2];
		memcpy(vPivot, &m_sDesc.vPivot, sizeof(_float2));
		if (ImGui::DragFloat2("Pivot", vPivot, 0.01f, 0.0f, 1.0f))
			this->Set_Pivot(_float2(vPivot[0], vPivot[1]));

		if (ImGui::SmallButton("LT"))
			this->Set_Edge(EEdge::LTOP);
		ImGui::SameLine();
		if (ImGui::SmallButton("CT")) 
			this->Set_Edge(EEdge::CTOP);
		ImGui::SameLine();
		if (ImGui::SmallButton("RT")) 
			this->Set_Edge(EEdge::RTOP);

		if (ImGui::SmallButton("LC"))
			this->Set_Edge(EEdge::LCOR);
		ImGui::SameLine();
		if (ImGui::SmallButton("CE")) 
			this->Set_Edge(EEdge::CEN);
		ImGui::SameLine();
		if (ImGui::SmallButton("RC")) 
			this->Set_Edge(EEdge::RCOR);

		if (ImGui::SmallButton("LB"))
			this->Set_Edge(EEdge::LBOT);
		ImGui::SameLine();
		if (ImGui::SmallButton("CB")) 
			this->Set_Edge(EEdge::CBOT);
		ImGui::SameLine();
		if (ImGui::SmallButton("RB")) 
			this->Set_Edge(EEdge::RBOT);

		ImGui::Text("=============== Inheritance ===============");
		_float vScl[3];
		memcpy(vScl, &m_sDesc.vScale, sizeof(_float3));
		if (ImGui::DragFloat3("Scale", vScl, 0.01f))
			if (vScl[0] > 0.f && vScl[1] > 0.f && vScl[2] > 0.f)
				this->Set_State(EState::SCL, _float3(vScl[0], vScl[1], vScl[2]));

		_float vRot[3];
		memcpy(vRot, &m_sDesc.vRotation, sizeof(_float3));
		if (ImGui::DragFloat3("Rotation", vRot, 0.01f))
			this->Set_State(EState::ROT, _float3(vRot[0], vRot[1], vRot[2]));

		_float vOrb[3];
		memcpy(vOrb, &m_sDesc.vOrbit, sizeof(_float3));
		if (ImGui::DragFloat3("Orbit", vOrb, 0.01f))
			this->Set_State(EState::ORB, _float3(vOrb[0], vOrb[1], vOrb[2]));

		ImGui::TreePop();
	}

}

void CRectTransform::Set_Parent(CRectTransform* const& pParent, const _bool& bScaleBy) {
	if (nullptr != pParent) {
		if (nullptr == m_pParent) {
			_vector vScale = XMLoadFloat3(&m_sDesc.vScale);
			if (bScaleBy) vScale *= 0.01f;
			this->Set_State(EState::SCL, vScale);
			m_pParent = pParent;
		}
		else {
			_vector vScale = XMLoadFloat3(&m_sDesc.vScale);
			if (bScaleBy) vScale *= 0.01f;
			this->Set_State(EState::SCL, vScale);
			m_pParent = pParent;
		}
	}
	else {
		if (nullptr != m_pParent) {
			m_pParent = nullptr;
			_vector vScale = XMLoadFloat3(&m_sDesc.vScale);
			if (bScaleBy) vScale *= 100.f;
			this->Set_State(EState::SCL, vScale);
		}
	}
	
	m_bSclByParent = bScaleBy;
	this->Update_Desc();
}

void CRectTransform::Update_Desc_Old() {
	m_vDestSize = { m_sDesc.fWidth * m_sDesc.vScale.x, m_sDesc.fHeight * m_sDesc.vScale.y, 1.f };
	//m_fX = g_iWinSizeX >> 1;
	//m_fY = g_iWinSizeY >> 1;
	//m_fSizeX = g_iWinSizeX;
	//m_fSizeY = g_iWinSizeY;
	//XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
	_float3		vTemp;
	if (nullptr == m_pParent) {
		vTemp = {
			(m_vDestSize.x * (.5f - m_sDesc.vPivot.x) + m_sDesc.vPosition.x) - m_vWinSize.x * 0.5f,
			(m_vDestSize.y * (.5f - m_sDesc.vPivot.y) + m_sDesc.vPosition.y) + m_vWinSize.y * 0.5f,
			1.f
		};
		//_float3 vTemp = {
		//	m_sDesc.vPosition.x - m_vWinSize.x * (.5f - m_sDesc.vAnchor.x) + m_vDestSize.x * (.5f - m_sDesc.vPivot.x),
		//	-m_sDesc.vPosition.y + m_vWinSize.y * (.5f - m_sDesc.vAnchor.y) + m_vDestSize.y * (-.5f + m_sDesc.vPivot.y),
		//	0.f
		//};
	}
	else {
		XMStoreFloat3(&vTemp, m_pParent->Get_Row(CTransform::ERow::POS));
		vTemp.x += m_pParent->m_vDestSize.x * (-.5f + m_sDesc.vAnchor.x) + m_vDestSize.x * (.5f - m_sDesc.vPivot.x);
		vTemp.y += m_pParent->m_vDestSize.y * (-.5f + m_sDesc.vAnchor.y) + m_vDestSize.y * (.5f - m_sDesc.vPivot.y);
	}

	__super::Set_Scale(_float3(m_vWinSize.x, m_vWinSize.y, 1.f));
	__super::Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vTemp));
}

void CRectTransform::Update_Desc() {
	_matrix	matScale, matRot, matTrans, matOrbit;
	m_vDestSize = { m_sDesc.fWidth * m_sDesc.vScale.x, m_sDesc.fHeight * m_sDesc.vScale.y, 1.f };

	if (nullptr == m_pParent) {
		XMStoreFloat3(&m_vDestSize, XMLoadFloat3(&m_vDestSize) * s_fCanvasScl);
		matScale = XMMatrixScaling(m_vDestSize.x, m_vDestSize.y, m_vDestSize.z);
		_float3 vTemp = {
			(m_sDesc.vPosition.x * s_fCanvasScl) - m_vWinSize.x * (.5f - m_sDesc.vAnchor.x) + m_vDestSize.x * (.5f - m_sDesc.vPivot.x),
			-(m_sDesc.vPosition.y * s_fCanvasScl) + m_vWinSize.y * (.5f - m_sDesc.vAnchor.y) + m_vDestSize.y * (-.5f + m_sDesc.vPivot.y),
			0.f
		};
		matTrans = XMMatrixTranslation(vTemp.x, vTemp.y, vTemp.z);
		//matRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_sDesc.vRotation.x), XMConvertToRadians(m_sDesc.vRotation.y), XMConvertToRadians(m_sDesc.vRotation.z));
		matRot = XMMatrixRotationZ(XMConvertToRadians(m_sDesc.vRotation.z));
		matOrbit = XMMatrixRotationZ(XMConvertToRadians(m_sDesc.vOrbit.z));
		__super::Set_Matrix(matScale * matRot * matTrans * matOrbit);
	}
	else {
		// percentage
		matRot = XMMatrixRotationZ(XMConvertToRadians(m_sDesc.vRotation.z));
		matOrbit = XMMatrixRotationZ(XMConvertToRadians(m_sDesc.vOrbit.z));

		if (!m_bSclByParent) {
			XMStoreFloat3(&m_vDestSize, XMLoadFloat3(&m_vDestSize) * s_fCanvasScl);
			matScale = XMMatrixScaling(m_vDestSize.x, m_vDestSize.y, m_vDestSize.z) /** 0.01f*/;
			_float3 vTemp = {
				(m_sDesc.vPosition.x * s_fCanvasScl) - m_pParent->m_vDestSize.x * (.5f - m_sDesc.vAnchor.x) + m_vDestSize.x * (.5f - m_sDesc.vPivot.x),
				-(m_sDesc.vPosition.y * s_fCanvasScl) + m_pParent->m_vDestSize.y * (.5f - m_sDesc.vAnchor.y) + m_vDestSize.y * (-.5f + m_sDesc.vPivot.y),
				0.f
			};
			matTrans = XMMatrixTranslation(vTemp.x, vTemp.y, vTemp.z) /** 0.01f*/;
			_float4x4 matParent = m_pParent->Get_Float4x4();
			_vector vRight = XMVector3Normalize(XMLoadFloat4((_float4*)&matParent.m[0][0]));
			_vector vUp = XMVector3Normalize(XMLoadFloat4((_float4*)&matParent.m[1][0]));
			_vector vLook = XMVector3Normalize(XMLoadFloat4((_float4*)&matParent.m[2][0]));
			//_vector vPos = XMVector3Normalize(XMLoadFloat4((_float4*)&matParent.m[3][0]));
			XMStoreFloat3((_float3*)&matParent.m[0][0], vRight);
			XMStoreFloat3((_float3*)&matParent.m[1][0], vUp);
			XMStoreFloat3((_float3*)&matParent.m[2][0], vLook);
			//XMStoreFloat3((_float3*)&matParent.m[3][0], vPos);
			__super::Set_Matrix(matScale * matRot * matTrans * matOrbit * XMLoadFloat4x4(&matParent));
		}
		else {
			//XMStoreFloat3(&m_vDestSize, XMLoadFloat3(&m_vDestSize) * s_fCanvasScl);
			matScale = XMMatrixScaling(m_vDestSize.x, m_vDestSize.y, m_vDestSize.z);
			_float3 vTemp = {
				(m_sDesc.vPosition.x * 0.01f) - m_pParent->m_vDestSize.x * ((.5f - m_sDesc.vAnchor.x) * 0.01f) + m_vDestSize.x * (.5f - m_sDesc.vPivot.x),
				-(m_sDesc.vPosition.y * 0.01f) + m_pParent->m_vDestSize.y * ((.5f - m_sDesc.vAnchor.y) * 0.01f) + m_vDestSize.y * (-.5f + m_sDesc.vPivot.y),
				0.f
			};
			matTrans = XMMatrixTranslation(vTemp.x, vTemp.y, vTemp.z);
			__super::Set_Matrix(matScale * matRot * matTrans * matOrbit * m_pParent->Get_Matrix());
		}			
	}		
}

const _float2 CRectTransform::Get_WorldToScreen(const _bool& bLeftTop) {
	_float3 vPos;
	XMStoreFloat3(&vPos, __super::Get_Row(CTransform::ERow::POS));
	vPos.x += m_vWinSize.x * .5f;
	vPos.y = -vPos.y + (m_vWinSize.y * .5f);
	if (bLeftTop) {
		//_float2 vSize = { m_vDestSize.x, m_vDestSize.y};
		//if (m_pParent && m_bSclByParent)
		//	vSize = { m_pParent->m_vDestSize.x, m_pParent->m_vDestSize.y };
		_float2 vScale = { m_sDesc.vScale.x,  m_sDesc.vScale.y };
		if (m_pParent && m_bSclByParent) {
			vScale.x *= 100.f;
			vScale.y *= 100.f;
		}
		vPos.x -= m_sDesc.fWidth * vScale.x * .5f;
		vPos.y -= m_sDesc.fHeight * vScale.y * .5f;
	}
	return _float2(vPos.x, vPos.y);
}

const _int CRectTransform::IsPtInRect(POINT* pMouse, const _bool& bEdit) {
	// to screen
	// x : -400 0 400 -> 0 400 800 
	// y : -300 0 300 -> 600 300 0
	_float3 vPos /*= m_sDesc.vPosition*/;
	XMStoreFloat3(&vPos, __super::Get_Row(CTransform::ERow::POS));
	RECT rc;
	_float3 vScale = __super::Get_Scale();
	SetRect(&rc,
		(int)(vPos.x - vScale.x * .5f),
		(int)(vPos.y - vScale.y * .5f),
		(int)(vPos.x + vScale.x * .5f),
		(int)(vPos.y + vScale.y * .5f));

	POINT ptTemp = *pMouse;
	ptTemp.x -= (_long)(m_vWinSize.x * .5f);
	ptTemp.y = -ptTemp.y + (_long)(m_vWinSize.y * .5f);
	if (bEdit)
		*pMouse = ptTemp;
	return PtInRect(&rc, ptTemp);
}

void CRectTransform::Set_Size(const _float& fWidth, const _float& fHeight) {
	m_sDesc.fWidth = fWidth;
	m_sDesc.fHeight = fHeight;
}

void CRectTransform::Set_Size() {
	m_sDesc.fWidth = m_sDesc.fWidth * s_fCanvasScl;
	m_sDesc.fHeight = m_sDesc.fHeight * s_fCanvasScl;
}

void CRectTransform::Set_Anchor(const _float2& vAnchor) {
	//if (nullptr != vTrans) {	
	//	_float2	vBaseSize = m_vWinSize;
	//	if (nullptr != m_pParent) {
	//		vBaseSize.x = m_pParent->m_vDestSize.x;
	//		vBaseSize.y = m_pParent->m_vDestSize.y;
	//	}
	//	_float	fX = m_sDesc.vPosition.x + (vBaseSize.x * (.5f - vAnchor.x));
	//	_float	fY = m_sDesc.vPosition.y + (vBaseSize.y * (.5f - vAnchor.y));
	//	*vTrans = _float3(fX, fY, 0.f);
	//}
	m_sDesc.vAnchor = vAnchor;
}

void CRectTransform::Set_Pivot(const _float2& vPivot) {
	m_sDesc.vPivot = vPivot;
}

void CRectTransform::Set_Edge(const EEdge& eEdge) {
	switch (eEdge)
	{
	case Engine::CRectTransform::EEdge::LTOP:
		Set_Anchor(_float2(0.f, 0.f));
		Set_Pivot(_float2(0.f, 0.f));
		break;
	case Engine::CRectTransform::EEdge::CTOP:
		Set_Anchor(_float2(0.5f, 0.f));
		Set_Pivot(_float2(0.5f, 0.f));
		break;
	case Engine::CRectTransform::EEdge::RTOP: 
		Set_Anchor(_float2(1.f, 0.f));
		Set_Pivot(_float2(1.f, 0.f));
		break;
	case Engine::CRectTransform::EEdge::LCOR:
		Set_Anchor(_float2(0.f, 0.5));
		Set_Pivot(_float2(0.f, 0.5));
		break;
	case Engine::CRectTransform::EEdge::CEN:
		Set_Anchor(_float2(0.5f, 0.5f));
		Set_Pivot(_float2(0.5f, 0.5f));
		break;
	case Engine::CRectTransform::EEdge::RCOR:
		Set_Anchor(_float2(1.f, 0.5f));
		Set_Pivot(_float2(1.f, 0.5f));
		break;
	case Engine::CRectTransform::EEdge::LBOT: 
		Set_Anchor(_float2(0.f, 1.f));
		Set_Pivot(_float2(0.f, 1.f));
		break;
	case Engine::CRectTransform::EEdge::CBOT:
		Set_Anchor(_float2(0.5f, 1.f));
		Set_Pivot(_float2(0.5f, 1.f));
		break;
	case Engine::CRectTransform::EEdge::RBOT:
		Set_Anchor(_float2(1.f, 1.f));
		Set_Pivot(_float2(1.f, 1.f));
		break;
	default:return;
	}
}

void CRectTransform::Set_State(const EState& eState, const _float3& vState, const _bool& bAcc) {
	//_float fScale = ((1.f / 1920.f) * m_vWinSize.x);
	_float fScale = 1.f;

	switch (eState)
	{
	case EState::POS: {
		//XMStoreFloat3(&m_sDesc.vPosition, XMLoadFloat3(&m_sDesc.vPosition) + XMLoadFloat3(&vState));
		if (bAcc) {
			//_vector vPos = XMLoadFloat3(&m_sDesc.vPosition) + XMLoadFloat3(&vState);
			_vector vPos = XMLoadFloat3(&m_sDesc.vPosition) + XMLoadFloat3(&vState) * fScale;
			XMStoreFloat3(&m_sDesc.vPosition, vPos);
		}
		else {
			//m_sDesc.vPosition = vState;
			m_sDesc.vPosition = _float3(vState.x * fScale, vState.y * fScale, vState.z * fScale);
		}
			
	}
					break;
	case EState::SCL: {
		_float3 vTemp = vState;
		if (bAcc) {
			//_vector vScl = XMLoadFloat3(&m_sDesc.vScale) + XMLoadFloat3(&vState);
			_vector vScl = XMLoadFloat3(&m_sDesc.vScale) + XMLoadFloat3(&vState) * fScale;
			XMStoreFloat3(&m_sDesc.vScale, vScl);
		}
		else {
			//m_sDesc.vScale = vState;
			m_sDesc.vScale = _float3(vState.x * fScale, vState.y * fScale, vState.z * fScale);
		}
			
	}
					break;
	case EState::ROT: {
		if (bAcc) {
			_vector vRot = XMLoadFloat3(&m_sDesc.vRotation) + XMLoadFloat3(&vState);
			XMStoreFloat3(&m_sDesc.vRotation, vRot);
		}
		else {
			m_sDesc.vRotation = vState;
		}
			
	}
					break;
	case EState::ORB: {
		if (bAcc) {
			_vector vOrb = XMLoadFloat3(&m_sDesc.vOrbit) + XMLoadFloat3(&vState);
			XMStoreFloat3(&m_sDesc.vOrbit, vOrb);
		}
		else {
			m_sDesc.vOrbit = vState;
		}
			
	}
					break;
	}
}

void CRectTransform::Set_State(const EState& eState, _fvector vState, const _bool& bAcc) {
	//_float fScale = ((1.f / 1920.f) * m_vWinSize.x);
	_float fScale = 1.f;

	switch (eState)
	{
	case EState::POS: {
		if (bAcc) {
			//_vector vPos = XMLoadFloat3(&m_sDesc.vPosition) + vState;
			_vector vPos = XMLoadFloat3(&m_sDesc.vPosition) + vState * fScale;
			XMStoreFloat3(&m_sDesc.vPosition, vPos);
		}
		else {
			//XMStoreFloat3(&m_sDesc.vPosition, vState);
			XMStoreFloat3(&m_sDesc.vPosition, vState * fScale);
		}
			
	}
					break;
	case EState::SCL: {
		if (bAcc) {
			//_vector vScl = XMLoadFloat3(&m_sDesc.vScale) + vState;
			_vector vScl = XMLoadFloat3(&m_sDesc.vScale) + vState * fScale;
			XMStoreFloat3(&m_sDesc.vScale, vScl);
		}
		else {
			//XMStoreFloat3(&m_sDesc.vScale, vState);
			XMStoreFloat3(&m_sDesc.vScale, vState * fScale);
		}
			
	}
					break;
	case EState::ROT: {
		if (bAcc) {
			_vector vRot = XMLoadFloat3(&m_sDesc.vRotation) + vState;
			XMStoreFloat3(&m_sDesc.vRotation, vRot);
		}
		else {
			XMStoreFloat3(&m_sDesc.vRotation, vState);
		}
			
	}
					break;
	case EState::ORB: {
		if (bAcc) {
			_vector vOrb = XMLoadFloat3(&m_sDesc.vOrbit) + vState;
			XMStoreFloat3(&m_sDesc.vOrbit, vOrb);
		}
		else
			XMStoreFloat3(&m_sDesc.vOrbit, vState);
	}
					break;
	}
}

void CRectTransform::Set_Scale(const _float& fScale, const _bool& bAcc) {
	if (bAcc) {
		_float3 vScl = _float3(m_sDesc.vScale.x + fScale, m_sDesc.vScale.y + fScale, m_sDesc.vScale.z + fScale);
		m_sDesc.vScale = vScl;
	}
	else {
		m_sDesc.vScale = _float3(fScale, fScale, fScale);
	}
}

void CRectTransform::Set_ScreenToWorld(POINT* pMouse, const _bool& bEdit) {
	POINT ptTemp = *pMouse;
	ptTemp.x -= (_long)(m_vWinSize.x * .5f);
	ptTemp.y = -ptTemp.y + (_long)(m_vWinSize.y * .5f);

	m_vPreMouse = XMLoadFloat3(&_float3((_float)ptTemp.x, (_float)ptTemp.y, 0.f));
	if (bEdit)
		*pMouse = ptTemp;
	m_bSetPre = true;
}

void CRectTransform::Set_MouseMove(POINT* pMouse) {
	if (0 == this->IsPtInRect(pMouse, true))
		return;
	_vector vCurMouse = XMLoadFloat3(&_float3((_float)pMouse->x, (_float)pMouse->y, 0.f));
	if (!m_bSetPre)
		m_vPreMouse = vCurMouse;
	this->Set_State(EState::POS, vCurMouse - m_vPreMouse, true);
}

void CRectTransform::Save_Data(Json::Value& root) {
	root["Position"]["x"] = m_sDesc.vPosition.x;
	root["Position"]["y"] = m_sDesc.vPosition.y;
	root["Position"]["z"] = m_sDesc.vPosition.z;
	root["Size"]["x"] = m_sDesc.fWidth;
	root["Size"]["y"] = m_sDesc.fHeight;
	root["Anchor"]["x"] = m_sDesc.vAnchor.x;
	root["Anchor"]["y"] = m_sDesc.vAnchor.y;
	root["Pivot"]["x"] = m_sDesc.vPivot.x;
	root["Pivot"]["y"] = m_sDesc.vPivot.y;
	root["Rotation"]["x"] = m_sDesc.vRotation.x;
	root["Rotation"]["y"] = m_sDesc.vRotation.y;
	root["Rotation"]["z"] = m_sDesc.vRotation.z;
	if (nullptr != m_pParent && m_bSclByParent) {
		_float3 vScale;
		XMStoreFloat3(&vScale, XMLoadFloat3(&m_sDesc.vScale) * 100.f);
		root["Scale"]["x"] = vScale.x;
		root["Scale"]["y"] = vScale.y;
		root["Scale"]["z"] = vScale.z;
	}
	else {
		root["Scale"]["x"] = m_sDesc.vScale.x;
		root["Scale"]["y"] = m_sDesc.vScale.y;
		root["Scale"]["z"] = m_sDesc.vScale.z;
	}
	// abstract
	root["SclByParent"] = m_bSclByParent;
}

void CRectTransform::Load_Data(Json::Value& root) {
	SDesc sDesc;
	sDesc.Reset();
	sDesc.vPosition.x = root["Position"]["x"].asFloat();
	sDesc.vPosition.y = root["Position"]["y"].asFloat();
	sDesc.vPosition.z = root["Position"]["z"].asFloat();
	sDesc.fWidth = root["Size"]["x"].asFloat();
	sDesc.fHeight = root["Size"]["y"].asFloat();
	sDesc.vAnchor.x = root["Anchor"]["x"].asFloat();
	sDesc.vAnchor.y = root["Anchor"]["y"].asFloat();
	sDesc.vPivot.x = root["Pivot"]["x"].asFloat();
	sDesc.vPivot.y = root["Pivot"]["y"].asFloat();
	sDesc.vScale.x = root["Scale"]["x"].asFloat();
	sDesc.vScale.y = root["Scale"]["y"].asFloat();
	sDesc.vScale.z = root["Scale"]["z"].asFloat();
	sDesc.vRotation.x = root["Rotation"]["x"].asFloat();
	sDesc.vRotation.y = root["Rotation"]["y"].asFloat();
	sDesc.vRotation.z = root["Rotation"]["z"].asFloat();
	this->Set_Desc(sDesc);
	// abstract
	m_bSclByParent = root["SclByParent"].asBool();
}
