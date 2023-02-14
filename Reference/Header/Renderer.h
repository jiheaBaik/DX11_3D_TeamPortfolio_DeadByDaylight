#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CRenderer final : public CComponent {
public:
	enum class EOrder : _ubyte { PRIORITY, NONALPHABLEND, DECAL, DISTORTION, SHADOW, SHADOW_BAKE, NONLIGHT, GLOW, ALPHABLEND, PENETRATE, UI, GODRAY, ENVIRO, EFFECT_UI, END };
public:
	static CRenderer* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CRenderer(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Render(void);
	const HRESULT Render_Enviro(void);
public:
	const HRESULT Add_Object(const EOrder& eOrder, class CObject* const& pObject);
	const HRESULT Render_Object(const EOrder& eOrder);
	const HRESULT Render_Enviro(const EOrder& eOrder);

	void Set_Bake(const _bool& bBake);
private:
	list<class CObject*> m_listObject[static_cast<_ubyte>(EOrder::END)];

	class CTarget_Mgr* m_pTarget_Mgr = nullptr;
	class CCompute* m_pCompute = nullptr;
	class CFragment* m_pFragment = nullptr;
private:
	_bool m_bBake = false;
};
END_NAMESPACE