#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CScene_Mgr final : public CBase {
	DECLARE_SINGLETON(CScene_Mgr)
private:
	explicit CScene_Mgr(void);
	virtual ~CScene_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	void Update_Scene(const _double& dTimeDelta);
	void Late_Update_Scene(const _double& dTimeDelta);
public:
	const _ubyte& Get_LoadingScene(void) const;
	void Set_LoadingScene(const _ubyte& byLoadingScene);

	const _ubyte& Get_Scene(void) const;
	class CScene* const& Get_Scene2(void) const;
	const HRESULT Change_Scene(const _ubyte& byScene, class CScene* const& pScene);
	const HRESULT Clear_Scene(void);
private:
	class CScene* m_pScene = nullptr;
	_ubyte m_byScene = 0;
	_ubyte m_byLoadingScene = 0;
};
END_NAMESPACE