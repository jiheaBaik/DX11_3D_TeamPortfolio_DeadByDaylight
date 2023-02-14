#pragma once
#include "Transform.h"

// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=pxkey&logNo=221558646854

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CRectTransform final : public CTransform
{
public:
	enum class EState { POS, SCL, ROT, ORB, END };
	enum class EEdge { NONE = -1, LTOP, CTOP, RTOP, LCOR, CEN, RCOR, LBOT, CBOT, RBOT, END  };
	struct SDesc {
		_float3		vPosition;
		_float		fWidth, fHeight;
		_float2		vAnchor, vPivot;
		_float3		vScale, vRotation, vOrbit;
		SDesc() {
			Reset();
		}
		void Reset() {
			vPosition = _float3(0.f, 0.f, 0.f);
			fWidth = 100.f, fHeight = 100.f;
			vAnchor = _float2(0.5f, 0.5f);
			vPivot = _float2(0.5f, 0.5f);
			vScale = _float3(1.f, 1.f, 1.f);
			vRotation = _float3(0.f, 0.f, 0.f);
			vOrbit = _float3(0.f, 0.f, 0.f);
		}
	};
public:
	static void Set_CanvasScl(const _float& fScale);
public:
	static CRectTransform* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CRectTransform(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CRectTransform(const CRectTransform& rhs);
	virtual ~CRectTransform(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg);
	virtual void Update_Edit(const _int& iPushID) override;
	void Update_Desc_Old();
	void Update_Desc();
public:
	const CRectTransform* Get_Parent() { return m_pParent; }
	void Set_Parent(CRectTransform* const& pParent, const _bool& bScaleBy = true);
public:
	static _float& Get_CanvasScl() { return s_fCanvasScl; }
	const SDesc& Get_Desc() { return m_sDesc; }
	const SDesc* Get_DescPtr() { return &m_sDesc; }
	_vector Get_DestSize() { return XMLoadFloat3(&m_vDestSize); }
	_float3 Get_DestSizeF3() { return m_vDestSize; }
	const _float2 Get_WorldToScreen(const _bool& bLeftTop = false);
	const _int IsPtInRect(POINT* pMouse, const _bool& bEdit = true);
	_vector Get_PreMouse() { return m_vPreMouse; }
	class CObject* Get_AttachObj() { return m_pAttachObj; }
	const _bool& Get_SclByParent() { return m_bSclByParent; }

	void Set_Desc(const SDesc& sDesc) { memcpy(&m_sDesc, &sDesc, sizeof(CRectTransform::SDesc)); }
	void Copy_Desc(SDesc* const& pDesc) { memcpy(pDesc, &m_sDesc, sizeof(SDesc)); }
	void Set_Size(const _float& fWidth, const _float& fHeight);
	void Set_Size();
	void Set_Anchor(const _float2& vAnchor/*, _float3* const& vTrans = nullptr*/);
	void Set_Pivot(const _float2& vPivot/*, _float3* const& vTrans = nullptr*/);
	void Set_Edge(const EEdge& eEdge);
	void Set_State(const EState& eState, const _float3& vState, const _bool& bAcc = false);
	void Set_State(const EState& eState, _fvector vState, const _bool& bAcc = false);
	void Set_Scale(const _float& fScale, const _bool& bAcc = false);
	void Set_ScreenToWorld(POINT* pMouse, const _bool& bEdit = true); // call at pre frame
	void Set_MouseMove(POINT* pMouse);
public:
	virtual void Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	static _float	s_fCanvasScl;
	// Inheritance
	class CObject*	m_pAttachObj = nullptr;
	CRectTransform* m_pParent = nullptr;
	_float2			m_vWinSize = { 0.f, 0.f };
	// out info
	SDesc			m_sDesc;
	_bool			m_bSclByParent = true;
	// dest info
	_float3*		m_pBaseSize = nullptr;
	_float3			m_vDestSize = _float3(0.f, 0.f, 0.f);
	// picking
	_bool			m_bSetPre = false;
	_vector			m_vPreMouse = XMVectorZero();
};
END_NAMESPACE
