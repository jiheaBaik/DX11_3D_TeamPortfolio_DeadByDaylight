#pragma once
#include "UI.h"

BEGIN_NAMESPACE(Client)
#define CONV_UTF_8TO16		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>	// convert.from_bytes(const char*)
#define CONV_UTF_WSTRTO8	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))
#define IM_LABELING(szLabel, szMessage1, szMessage2)			\
strcpy_s(szLabel, szMessage1);									\
strcat_s(szLabel, szMessage2);	

//static void HelpMarker(const char* desc) {
//	ImGui::TextDisabled("(?)");
//	if (ImGui::IsItemHovered()) {
//		ImGui::BeginTooltip();
//		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
//		ImGui::TextUnformatted(desc);
//		ImGui::PopTextWrapPos();
//		ImGui::EndTooltip();
//	}
//}

class CUI_Image abstract : public CUI
{
public:
	enum class EUIPass { PASS_DEFAULT, PASS_ALPHABLEND, PASS_SPREAD, PASS_FLOW, PASS_DECREASE, PASS_LOADING, PASS_OVERLAP, PASS_DISSOLVE, PASS_COOLDOWN, END };
	static const _char* EUIPass_ToString(const EUIPass& eType) {
		switch (eType)
		{
		case Client::CUI_Image::EUIPass::PASS_DEFAULT: return "PASS_DEFAULT";
		case Client::CUI_Image::EUIPass::PASS_ALPHABLEND: return "PASS_ALPHABLEND";
		case Client::CUI_Image::EUIPass::PASS_SPREAD: return "PASS_SPREAD";
		case Client::CUI_Image::EUIPass::PASS_FLOW: return "PASS_FLOW";
		case Client::CUI_Image::EUIPass::PASS_DECREASE: return "PASS_DECREASE";
		case Client::CUI_Image::EUIPass::PASS_OVERLAP: return "PASS_OVERLAP";
		case Client::CUI_Image::EUIPass::PASS_LOADING: return "PASS_LOADING";
		case Client::CUI_Image::EUIPass::PASS_DISSOLVE: return "PASS_DISSOLVE";
		case Client::CUI_Image::EUIPass::PASS_COOLDOWN: return "PASS_COOLDOWN";
		}
		return nullptr;
	}
	struct SImageDesc {
		CTexture* pTexture = nullptr;
		CRectTransform* pRectTransform = nullptr;
		CCollider* pCollider = nullptr;
		_tchar szTexture[_MAX_PATH] = L"";
		_tchar szParent[_MAX_PATH] = L"";
		_bool bRender = true;
		_bool bSizeOfTex = true;
		_bool bCollider = false;
		_bool bActByParent = true;
		_int iDepth = -1;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		_uint iPass = 1;
		_bool bSave = true;
		_double dAccTime = 0.;
		SImageDesc() {}
		void Reset() {
			pRectTransform = nullptr;
			pTexture = nullptr;
			pCollider = nullptr;
			bRender = true;
			bCollider = false;
			bSizeOfTex = true;
			bActByParent = true;
			iDepth = -1;
			vColor = { 1.f, 1.f, 1.f, 1.f };
			iPass = 1;
			dAccTime = 0.;
			bSave = true;
		}
		void Release() {
			Safe_Release(pTexture);
			Safe_Release(pRectTransform);
			Safe_Release(pCollider);
		}
		const _bool IsContinue() {
			return (!this->bRender || (nullptr == this->pRectTransform) || (nullptr == this->pTexture));
		}
		const _bool Invoke(const _double& dDeltaTime, const _double& dLimTime) {
			if (dAccTime >= dLimTime) {
				dAccTime = 0.0;
				return true;
			}
			dAccTime += dDeltaTime;
			return false;
		}
	};
	struct SFontDesc {
		CRectTransform* pRectTransform = nullptr;
		_tchar szFontKey[_MAX_PATH] = L"";
		_tchar szText[_MAX_PATH] = L"";
		_float fScale = 1.f;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		_uint iDepth = 0;
		_bool bRender = true;
		_bool bLeftTop = true;
		_double dAccTime = 0.;
		SFontDesc() {}
		void Reset() {
			pRectTransform = nullptr;
			lstrcpy(szFontKey, L"");
			lstrcpy(szText, L"");
			bRender = true;
			bLeftTop = true;
			iDepth = 0;
			fScale = 1.f;
			vColor = { 1.f, 1.f, 1.f, 1.f };
			_double dAccTime = 0.;
		}
		void Release() {
			Safe_Release(pRectTransform);
		}
		const _bool Invoke(const _double& dDeltaTime, const _double& dLimTime) {
			if (dAccTime >= dLimTime) {
				dAccTime = 0.0;
				return true;
			}
			dAccTime += dDeltaTime;
			return false;
		}
	};
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CUI_Image(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Image(const CUI& rhs);
	virtual ~CUI_Image(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	const HRESULT Init_Font();
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Update_Edit_Images();
	virtual void Update_Edit_Font(SFontDesc* const& pText);
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
	const HRESULT Render_Images(const CRenderer::EOrder& eOrder);
	const HRESULT Render_Font(SFontDesc* const& pFont, CRectTransform* const& pRectTransform = nullptr);
public:
	const _bool& Get_Active() { return m_bActiveSelf; }
	CRectTransform* const& Get_RectTransform() { return m_pRectTransform; }
	const _bool& Get_ActiveByParent() { return m_bActiveByParent; }
	const CUI_Image* Get_Parent() { return m_pParent; }
	const _uint Get_ChildNums() { return static_cast<_uint>(m_vecChilds.size()); }
	CUI_Image* const Get_Child(const _uint& iCount) {
		if (0 >= m_vecChilds.size())
			return nullptr;
		return m_vecChilds[iCount];
	}
	vector<CUI_Image*>* const Get_AllChilds() { return &m_vecChilds; }
	wstring Get_TextureKey(wstring wstrKey);
	const _float4& Get_Color() { return m_vColor; }
	const _int& Get_GroupIdx() { return m_iGroup; }

	void Set_Active(const _bool& bActive);
	void Set_Dead() { m_bDead = true; }
	void Set_Parent(CUI_Image* const& pParent, const _bool& bScaleBy = true);
	void Push_Child(CUI_Image* const& pChild);
	void Pop_Child(CUI_Image* const& pChild);
	// temp
	const _bool& Get_IsRender() { return m_bRender; }
	const _bool& Get_IsFade() { return m_bFade; }
	void Set_Render(const _bool& bRender) { m_bRender = bRender; }
	void Set_Alpha(const _float& fAlpha) { m_vColor.w = fAlpha; }
	void Set_Fade(const _bool& bIsIn, const _float& fAlphaMin, const _float& fAlphaMax, const _float& fPerSec, const _bool& bReset = false);
protected:
	void Set_SizeOfTexture(CTexture* const& pTexture, CRectTransform* const& pRectTransform);
	const _bool IsCollisionEnter(CCollider* const& pCollider, CRectTransform* const& pRectTransform);
	const _bool IsCollisionStay(CCollider* const& pCollider, CRectTransform* const& pRectTransform);
	void Sort_Images() { 
		if (m_vecImages.empty())
			return;
		sort(m_vecImages.begin(), m_vecImages.end(), [&](SImageDesc sSour, SImageDesc sDesc)->_bool { return sSour.iDepth < sDesc.iDepth; });
	}
	SImageDesc* const Add_Image(const _tchar* pTexKey, const CRectTransform::EEdge& eEdge = CRectTransform::EEdge::CEN, const _bool& bSizeOfTex = true);
	SImageDesc* const Add_Image(const _tchar* pTexKey, const _int& iDepth, const _bool& bRender = true, const CRectTransform::EEdge& eEdge = CRectTransform::EEdge::CEN, const _bool& bSizeOfTex = true);
	SImageDesc* const Add_Image(const _tchar* pTexKey, const _tchar* pParnetKey, CRectTransform* const& pParnetTf, const _bool& bSclByParent = true, const CRectTransform::EEdge& eEdge = CRectTransform::EEdge::CEN, const _bool& bSizeOfTex = true);
	const SFontDesc Add_Font(const _tchar* pFontKey, const _bool& bLeftTop, const _float2& fSize, const CRectTransform::EEdge& eEdge = CRectTransform::EEdge::CEN, CRectTransform* const& pRectTransform = nullptr);
	void Delete_List(const _uint& iIndex);
	void Set_FontCnt(SFontDesc* const& pFont, const _int& iCnt, const _bool& bAcc);
	void Set_FontStr(SFontDesc* const& pFont, const _tchar* pText, const _bool& bAcc);
	const _bool Invoke(const _double& dDeltaTime, const _double& dLimTime);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
	virtual const HRESULT Save_AddData(const _tchar* pLayer);
	virtual void Load_AddData(const _tchar* pLayer, const _uint& iAccSize = 0);
#ifdef _DEBUG
	virtual void Delete_AddData(const _tchar* pLayer);
#endif // _DEBUG
protected:
	class CBuffer_Rect* m_pVBRect = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CRectTransform* m_pRectTransform = nullptr;
	class CTexture* m_pTexture = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CAudio* m_pAudio = nullptr;

	CUI_Image* m_pParent = nullptr;
	vector<CUI_Image*> m_vecChilds;
	SImageDesc m_sImage;
	vector<SImageDesc> m_vecImages;
protected:
	_bool	m_bActiveSelf = true;
	_bool	m_bActiveByParent = true;
	_bool	m_bPreActive = true;
	_bool	m_bDead = false;
	_bool	m_bStart = false;
	_int	m_iGroup = -1;
	_int	m_iType = -1;
	_int	m_iSort = -1;
	// image info
	_bool	m_bRender = true;
	_uint	m_iPass = 0;
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_bool	m_bFade = false;
	_double	m_dAccTime = 0.0;
	_bool	m_bOneOff = false;
	// gui
	_bool	m_bCollider = false;
	_tchar	m_szTexKey[_MAX_PATH] = L"";
	_char	m_szParentKey[_MAX_PATH] = "";
	//vector<const _tchar*> m_vecFontKeys;
	_char m_szBuffer[_MAX_PATH] = "";
	_tchar m_szFilePath[_MAX_PATH] = L"";
	const _float m_fAtt = (1.f / 1600.f) * (_float)g_iWinSizeX;
};
END_NAMESPACE

