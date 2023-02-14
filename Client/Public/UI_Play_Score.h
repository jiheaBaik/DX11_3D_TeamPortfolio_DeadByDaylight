#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)

class CUI_Play_Score final : public CUI_Image
{
public:
	enum class ECamperScore  { ALTRUISM_GOODSKILL, ALTRUISM_GREATSKILL, ALTRUISM_HEAL,
		ALTRUISM_HOOK,BOLDNESS_GOODSKILL, BOLDNESS_GREATSKILL, BOLDNESS_KILLER_BLIND,
		BOLDNESS_PALLET_DROP, BOLDNESS_TOTEM, BOLDNESS_VAULTING, BOLDNESS_WIGGLE,
		OBJECTIVE_CHEST_SEARCH, OBJECTIVE_GOODSKILL, OBJECTIVE_GREATSKILL,
		OBJECTIVE_HATCH_ESCAPE, OBJECTIVE_REPAIRS, OBJECTIVE_UNLOCKING,
		SURVIVAL_HEAL, SURVIVAL_HOOK_ESCAPE, SURVIVAL_KILLER_ESCAPE, SURVIVAL_SURVIVED, END };

	enum class ESlasherScore { BRUTALLITY_CATCH, BRUTALLITY_DESTORY, BRUTALLITY_GENE,
		BRUTALLITY_HIT, DEVIOUSNESS_HATCH_CLOSED, DEVIOUSNESS_HAUNTING,
		DEVIOUSNESS_PRECISE_SHOT, HUNTER_CATCH, SACRIFICE_ENTITY_SUMMONED, 
		SACRIFICE_HOOKED, SACRIFICE_SACRIFICE, SACRIFICE_TIMESUP,END };
	enum class ESlasherNum{BRUTALITY,DEVIOUSNESS,HUNTER,SACRIFICE,END};
	enum class ECamperNum { ALTRUISM,BOLDNESS, OBJECTIVE, SURVIVAL, END };
public:
	static CUI_Play_Score* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Score(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Score(const CUI_Play_Score& rhs);
	virtual ~CUI_Play_Score(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Add_Score(_uint iScoreType);
	void Plus_SlasherScore(_uint iScoreNum, _uint iScore);
	void Plus_CamperScore(_uint iScoreNum, _uint iScore);
	_uint Get_SlasherScore(_uint iScoreNum);
	_uint Get_CamperScore(_uint iScoreNum);
	_uint* const Get_SlasherScorePtr(_uint iScoreNum);
	_uint* const Get_CamperScorePtr(_uint iScoreNum);
	_uint* const Get_SumScorePtr() { return &m_iSumScore; }
public:
	/*virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;*/
private:
	_uint m_iScorePos[6]{ 9999,9999,9999,9999,9999,9999 };
	_bool m_bStartErase = false;
	_double m_dEraseTime = 0.0;
	//slasher
	_uint m_iBrutality = 0;
	_uint m_iDeviousness = 0;
	_uint m_iHunter = 0;
	_uint m_iSacrifice = 0;
	//camper
	_uint m_iAltruismScore = 0;
	_uint m_iBlodnessScore = 0;
	_uint m_iObjectiveScore = 0;
	_uint m_iSurvivalScore = 0;
	// sum
	_uint m_iSumScore = 0;

#ifdef _DEBUG
	_tchar m_szOneScore[_MAX_PATH] = L"";
	_tchar m_szTwoScore[_MAX_PATH] = L"";
	_tchar m_szThreeScore[_MAX_PATH] = L"";
	_tchar m_szFourScore[_MAX_PATH] = L"";
	_double m_dAccTime = 0.0;
	
#endif // _DEBUG
};
END_NAMESPACE