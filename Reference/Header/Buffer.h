#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CBuffer abstract : public CComponent{
public:
	virtual CComponent* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CBuffer(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CBuffer(const CBuffer& rhs);
	virtual ~CBuffer(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Render(void);
	const HRESULT Render(class CShader* const& pShader, const _uint & iPass);
public:
	const _float3* const& Get_VertexPos(void) { return m_pVertexPos; }
	const void* const& Get_Vertex(void) { return m_pVertex; }
	const void* const& Get_Index(void) { return m_pIndex; }
	const DXGI_FORMAT& Get_Format(void) { return m_eFormat; }
	const _uint& Get_Face(void) { return m_iFace; }
protected:
	const HRESULT Create_VertexBuffer(const D3D11_BUFFER_DESC & Buffer_Desc, const D3D11_SUBRESOURCE_DATA & SubResource_Data);
	const HRESULT Create_IndexBuffer(const D3D11_BUFFER_DESC & Buffer_Desc, const D3D11_SUBRESOURCE_DATA & SubResource_Data);
protected:
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	_uint m_iNumBuffer = 0;
	_uint m_iStride = 0;
	_uint m_iNumVertices = 0;
protected:
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	DXGI_FORMAT m_eFormat = DXGI_FORMAT_FORCE_UINT;
	D3D11_PRIMITIVE_TOPOLOGY m_eTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	_uint m_iIndexCount = 0;
	_uint m_iNumIndicesPerPrimitive = 0;
	_uint m_iBytePerPrimitive = 0;

protected:
	_float3* m_pVertexPos = nullptr;
	void* m_pVertex = nullptr;
	void* m_pIndex = nullptr;
	_uint m_iFace = 0;
};
END_NAMESPACE
