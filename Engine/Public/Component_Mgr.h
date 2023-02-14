#pragma once
#include "Audio.h"
#include "Buffer_Rect.h"
#include "Buffer_Cube.h"
#include "Buffer_Terrain.h"
#include "Buffer_Trail.h"
#include "Collider.h"
#include "Model.h"
#include "Navi.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "RectTransform.h"

BEGIN_NAMESPACE(Engine)
class CComponent_Mgr final : public CBase {
	DECLARE_SINGLETON(CComponent_Mgr)
private:
	explicit CComponent_Mgr(void);
	virtual ~CComponent_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const _ubyte& byScene);
public:
	map<wstring, class CComponent*>& Get_Prototype(void);
	class CComponent* Get_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey);

	const HRESULT Add_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey, class CComponent* const& pComponent) const;
	class CComponent* const CreateGet_Clone(const _ubyte& byScene, const _tchar* const& pPrototypeKey, void* const& pArg = nullptr) const;
	const HRESULT Clear_Prototype(const _ubyte& byScene) const;
private:
	class CComponent* Find_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey) const;

private:
	_ubyte m_byScene = 0;
	map<wstring, class CComponent*>* m_pMapPrototype = nullptr;
};
END_NAMESPACE