#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CLayer final : public CBase{
public:
	static CLayer* const Create(void);
private:
	explicit CLayer(void);
	virtual ~CLayer(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	const _ubyte Update_Object(const _double & dTimeDelta);
	void Late_Update_Object(const _double & dTimeDelta) const;
public:
	class CObject* const Get_Object(const _uint & iIndex = 0) const;
	class CObject* const Get_ObjectID(const _uint& iID = 0) const;
	class CObject* const Get_ObjectControl(void) const;
	 list<class CObject*>& Get_listObject(void);
	const HRESULT Add_Object(class CObject* const& pObject);
	const HRESULT Delete_Object(const class CObject* const& pObject);
private:
	list<class CObject*> m_listObject;
};
END_NAMESPACE