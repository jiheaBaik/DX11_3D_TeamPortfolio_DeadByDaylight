#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CPost final : public CEdit {
public:
	static CPost* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CPost(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CPost(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
private:
	class CCompute* m_pCompute = nullptr;
	class CFragment* m_pFragment = nullptr;
};
END_NAMESPACE