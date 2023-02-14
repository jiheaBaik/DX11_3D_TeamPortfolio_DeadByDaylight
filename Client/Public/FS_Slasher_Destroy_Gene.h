#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Destroy_Gene final : public CFS {
public:
	explicit CFS_Slasher_Destroy_Gene(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Destroy_Gene(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CInter_Generator* m_pGenerator = nullptr;
	class CTransform* m_pGeneTransform = nullptr;
	class CTransform* m_pGeneTransform2 = nullptr;
	class CTransform* m_pGeneTransform_C = nullptr;
private:
	_uint m_iGeneDir = 0;
};
END_NAMESPACE