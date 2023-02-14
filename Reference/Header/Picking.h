#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CPicking : public CBase {
	DECLARE_SINGLETON(CPicking)
private:
	explicit CPicking(void);
	virtual ~CPicking(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const HWND& hWnd, ID3D11DeviceContext* const& pContext);
public:
	const _bool Is_Picking(class CCamera* const& pCamera, class CObject* const& pObject, _float* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CObject* const& pObject, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CTransform* const& pTransform, class CBuffer* const& pBuffer, _float* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CTransform* const& pTransform, class CBuffer* const& pBuffer, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CTransform* const& pTransform, class CModel* const& pModel, _float* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CTransform* const& pTransform, class CModel* const& pModel, _float3* const& pOut = nullptr);

	const _bool Is_Picking(class CCamera* const& pCamera, class CBuffer_Terrain* const& pBuffer, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CNavi* const& pNavi, _float3* const& pOut = nullptr);
	const _bool Is_Picking(class CCamera* const& pCamera, class CCell* const& pCell, _float3* const& pOut = nullptr);
private:
	void Get_Ray(class CCamera* const& pCamera, class CTransform* const& pTransform, _float3* const& vOutDir, _float3* const& vOutPos);
	void Get_Ray(class CCamera* const& pCamera, _fmatrix Matrix, _float3* const& vOutDir, _float3* const& vOutPos);
private:
	HWND m_hWnd = 0;
	ID3D11DeviceContext* m_pContext = nullptr;
};
END_NAMESPACE