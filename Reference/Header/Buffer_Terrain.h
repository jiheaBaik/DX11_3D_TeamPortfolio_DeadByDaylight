#pragma once
#include "Buffer.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CBuffer_Terrain final : public CBuffer{
public:
	static CBuffer_Terrain* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CBuffer_Terrain(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CBuffer_Terrain(const CBuffer_Terrain& rhs);
	virtual ~CBuffer_Terrain(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual void Update_Edit(const _int & iPushID) override;
private:
	const HRESULT Create_Terrain(const _int & iCountX, const _int & iCountZ);
	const HRESULT Set_Vertex(const _int & iIndex, const _float & iDepth);
	const HRESULT Add_Vertex(const _int & iIndex, const _float & iDepth);
	const HRESULT Set_Dist(const _float& fDist);
	const HRESULT Set_TexCoord(const _float & fTexCoord);
	const HRESULT Set_Texture(const _int & i, const _int& j, const _float & iDepth);
public:
	const HRESULT Set_Resource(class CShader* const& pShader, const _char* const& pName);
private:
	const _uint Find_Index(const _float3& vPos);
public:
	virtual void Save_Data(Json::Value & root) override;
	virtual void Load_Data(Json::Value & root) override;
private:
	char m_szScene[MAX_PATH] = "";
	_uint m_iCountX = 1, m_iCountZ = 1;
	_float m_fDist = 1.f;
	_float m_fTexCoord = 1.f;
	ID3D11Texture2D* m_pTexture2D = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
private:
	_bool m_bMode[2] = { true, false };
private:
	_float m_fRadius = 0.f;
	_float m_fDepth = 0.f;
	_bool m_bDepth[3]= { true, false };
private:
	_bool m_bBrush[5] = { true, false, false, false, false };
};
END_NAMESPACE