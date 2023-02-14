#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CView final : public CEdit {
public:
	static CView* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CView(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CView(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
private:
	class CTarget* m_pTarget = nullptr;
};
END_NAMESPACE