#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CCollider final : public CComponent{
public:
	enum class EType : _ubyte { AABB, OBB, SPHERE, END };
	struct SCollider {
		_float3 vPos = _float3{ 0.f, 0.f, 0.f };
		_float3 vScale = _float3{ 1.f, 1.f, 1.f };
		_float vRadius = 1.f;
	};
public:
	static CCollider* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EType& eType);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCollider(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const EType & eType);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	void Update(_fmatrix Matrix);
	virtual void Update_Edit(const _int& iPushId) override;
	const HRESULT Render(class CCamera* const& pCamera);
	const HRESULT Render(void);
public:
	const _float3 Get_Center(void);
	const _float3 Get_Center_Local(void);
	const _float3 Get_Extents(void);
	const _float Get_Radius(void);
	void Set_Center(const _float3& vCenter);
	void Set_Extents(const _float3& vExtents);
	const _bool Is_Collision(class CCollider* const& pCollider);
	const _bool Is_CollisionEnter(_fvector vOrigin, _fvector vDir, _float3* const& pOut = nullptr);
	const _bool Is_Collision(_fvector vOrigin, _fvector vDir, _float3* const& pOut = nullptr);
	const _bool Is_Collision(class CCell* const& pCell);
	_float3* const Get_Corner(void);
public:
	virtual void Save_Data(Json::Value & root) override;
	virtual void Load_Data(Json::Value & root) override;
private:
	EType m_eType = EType::END;
	BoundingBox* m_pAABB_Local = nullptr;
	BoundingOrientedBox* m_pOBB_Local = nullptr;
	BoundingSphere* m_pSphere_Local = nullptr;

	BoundingBox* m_pAABB_World = nullptr;
	BoundingOrientedBox* m_pOBB_World = nullptr;
	BoundingSphere* m_pSphere_World = nullptr;
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
private:
	_bool m_bCollision = false;
	_bool m_bCollisionEnter = false;
	_float3 m_vCorner[8];
};
END_NAMESPACE

