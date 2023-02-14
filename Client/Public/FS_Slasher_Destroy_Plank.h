#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Destroy_Plank final : public CFS {
public:
	explicit CFS_Slasher_Destroy_Plank(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Destroy_Plank(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CInter_PlankAnim* m_pPlank = nullptr;
	class CTransform* m_pPlankTransform = nullptr;
	class CTransform* m_pPlankTransform2 = nullptr;
	class CTransform* m_pPlankTransform_Slasher = nullptr;
public:
	void Set_Broken(const _bool& bBroken);
private:
	_uint m_iPlankDir = 0;

	_bool m_bBroken = false;
};
END_NAMESPACE