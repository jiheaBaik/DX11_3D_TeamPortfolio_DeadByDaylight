#pragma once
#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CNavMesh : public CEdit {
public:
	static CNavMesh* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CNavMesh(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CNavMesh(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
	virtual const HRESULT Render(void) override;
public:
	void Set_Navi(class CNavi* const& pNavi);
	void Save_Data(const _tchar* const& pFilePath);
public:
	void Get_Ray(class CCamera* const& pCamera, _float3* const& vOutDir, _float3* const& vOutPos);
private:
	_bool m_bNaviRender = false;

	class CNavi* m_pNavi = nullptr;
	class CCell* m_pCell = nullptr;

	_uint m_iClickIndex = 0;
	_float3 m_vClickPos[3];
	int m_iIndex = 0;
};
END_NAMESPACE