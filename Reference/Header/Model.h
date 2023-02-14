#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CModel final : public CComponent{
public:
	enum class EType : _ubyte { NOANIM, ANIM, END };
	struct SMaterial {
		_tchar szFilePath[AI_TEXTURE_TYPE_MAX][MAX_PATH];
		class CTexture* pTexture[AI_TEXTURE_TYPE_MAX];
	};
public:
	static CModel* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EType& eType, const _uint& iInst, const char* const& pFilePath, const char* pFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CModel(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const EType & eType, const _uint & iInst, const char* const& pFilePath, const char* const& pFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	const _uint Update_Anim(const _double & dTimeDelta, const _bool & bLoop = false);
	const HRESULT Render(const _uint & iMesh, class CShader* const& pShader, const _uint & iDesc);
public:
	virtual void Update_Edit(const _int & iPushID) override;
public:
	virtual void Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
public:
	const EType& Get_Type(void) const;
	_matrix Get_PivotMatrix(void) const;
	_matrix Get_BoneMatrix(const char* const& pName);
	_matrix Get_RootBoneMatrix(const char* const& pName);
	_matrix Get_RootBoneMatrix(const _uint& idx);
	vector<class CBone*> const& Get_Bone(void) const;
	const _char* const Get_bone_Name(const _uint & idx);
	void Change_Bone(class CModel* const& pModel, vector<const char*> m_vecBoneName);

	vector<class CMesh*> const& Get_Mesh(void) const;
	const _uint Get_NumMesh(void) const;
	const _uint Get_KeyFrame(void) const;

	const _char* const Get_Anim_Name(void);
	const _char* const Get_Anim_IdxName(const _uint & idx);
	const _uint Get_AnimCount(void);
	void Set_Anim(const _uint & iAnim, const _double & dLerpTime = 0.2);
	void Set_Anim(const _char* const& pName, const _double & dLerpTime = 0.2);
	void Set_Anim_TimeAcc(const _double & dTimeAcc);
	const _double& Get_Anim_TimeAcc(void) const;
	const _bool Get_Anim_KeyTimeAcc(const _uint& iKeyTimeAcc) const;
	const _double& Get_Anim_Duration(void) const;
	const _uint& Get_AnimNum(void) const;
	const _uint& Get_Inst(void);
	const _double& GetPosByKeyframe(vector<_float4>* vecPos, vector<_double>* vecdTime)const;
	_float4x4 Get_BoneOffsetMatrix(const char* const& pName);

	const _bool Get_Resource(const _uint& iMesh, const aiTextureType& eTexture, class CShader* const& pShader, const char* const& pName);
	const HRESULT Set_Resource(const _uint & iMesh, const aiTextureType & eTexture, class CShader* const& pShader, const char* const& pName);

	void Set_Inst_Random(const _uint& iInstCount, const _float& fInstDistX, const _float& fInstDistZ);
	void Set_Inst_Rotate(void);
	void Set_Inst_Scale(const _float3& minSize, const _float3& maxSize);
private:
	const HRESULT Create_Bone(aiNode* const& paiNode, class CBone* const& pParent, _uint iDepth);
private:
	Assimp::Importer m_Importer;
	const aiScene* m_paiScene = nullptr;
private:
	vector<class CBone*> m_vecBone;
	vector<class CMesh*> m_vecMesh;
	vector<SMaterial> m_vecMaterial;
	vector<class CAnim*> m_vecAnim;
private:
	EType m_eType = EType::END;
	_uint m_iAnim = 0;
	_uint m_iPreAnim = 0;
	_float4x4 m_PivotFloat4x4;

	_uint m_iInst = 0;
	_uint m_iStrideInst = 0;
	ID3D11Buffer* m_pVertexBufferInst = nullptr;
private:
	_double m_dLerpTime = 1.;
	_double m_dLerpMaxTime = 1.;
	vector<_int> m_vecChangeBone;
private:
	_bool m_bMode = false;
	_uint m_iInstCount = 0;
	_uint m_iInstX = 0, m_iInstZ = 0; 
	_float m_fInstDistX = 1.f, m_fInstDistZ = 1.f;
};
END_NAMESPACE