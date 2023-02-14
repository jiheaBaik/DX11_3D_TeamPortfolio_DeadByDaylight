#pragma once
#include "Buffer.h"
#include "Model.h"

BEGIN_NAMESPACE(Engine)
class CMesh final : public CBuffer {
public:
	static CMesh* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const CModel::EType& eType, const _uint& iInst, aiMesh* const& paiMesh, const vector<class CBone*>& pVecBone, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CMesh* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const CModel::EType& eType, const _uint& iInst, ifstream& ifs, const vector<class CBone*>& pVecBone);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CMesh(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const CModel::EType& eType, const _uint& iInst, aiMesh* const& paiMesh, const vector<class CBone*>& pVecBone, _fmatrix PivotMatrix = XMMatrixIdentity());
	const HRESULT Init_Create(const CModel::EType& eType, const _uint& iInst, ifstream& ifs, const vector<class CBone*>& pVecBone);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
public:
	const HRESULT Render(class CShader* const& pShader, const _uint& iPass, ID3D11Buffer* const& pVertexBufferInst, const _uint& iStrideInst, const _uint& iInst, const _uint& iInstCount);
public:
	const _uint& Get_Material(void) const;
	void Set_BoneFloat4x4(_float4x4* const& pBoneFloat4x4, _fmatrix PivotMatrix);
	void Set_BoneFloat4x4_Transpose(_float4x4* const& pBoneFloat4x4, _fmatrix PivotMatrix);
public:
	void Save_Data(ofstream& ofs, const CModel::EType& eType);
private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iVertex = 0;
	vector<class CBone*> m_vecBone;
	_uint m_iMaterial = 0;
};
END_NAMESPACE
