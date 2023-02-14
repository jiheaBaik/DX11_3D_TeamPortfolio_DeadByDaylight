#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Icon_Passive final : public CUI_Image
{
public:
	enum class EFXType { BACK, FX, END };
	struct SPos {
		_float3 vPos;
		_float2 vScale;
	};
	struct SHud {
		SImageDesc* pImage = nullptr;
		CTexture* pTexture = nullptr; 
	};
	struct SIcon {
		CTexture* pTexture = nullptr;
		_vector vDist = XMVectorZero();
		_float fScale = 1.f;
		_uint iIdx = 0;
		_uint iPush = 0; // 0: none/1: right/2: left
		_float fCoolTime = 0.f;
		SIcon() {};
		void Init(CTexture* const& pTexture, const _int& iIdx = -1) {
			this->pTexture = pTexture;	
			this->iIdx = iIdx;
		}
		void Reset(const _float& fScale) {
			this->fScale = fScale;
			this->vDist = XMVectorZero();
			this->iPush = 0;
		}
	};
public:
	static CUI_Play_Icon_Passive* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Icon_Passive(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Icon_Passive(const CUI_Play_Icon_Passive& rhs);
	virtual ~CUI_Play_Icon_Passive(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Push_Slot(const _int& iSlot);
	void Pop_Slot();
	void Set_FontCnt(const _int& iCnt, const _bool& bAcc);
	void Set_FontStr(const _tchar* pText, const _bool& bAcc);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	_uint m_iSlotNum = 2;
	SHud m_sSlots[2];	// slot image
	vector<CTexture*> m_vecFXTexs;

	list<SIcon> m_listIcons;  // icon texture
	SFontDesc m_sFont;
};
END_NAMESPACE

