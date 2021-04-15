
#ifndef FOX_NATIVE_SCRIPT_HPP_
#define FOX_NATIVE_SCRIPT_HPP_

#include "../ScriptableEntity.hpp"

struct NativeScript
{
	ScriptableEntity* m_pInstance = nullptr;

	ScriptableEntity* (*InstantiateScript)();
	void (*DestroyScript)(NativeScript*);

	template <typename T>
	void bind()
	{
		InstantiateScript = []() {
			return static_cast<ScriptableEntity*>(new T());
		};
		DestroyScript = [](NativeScript* pScript) {
			delete pScript->m_pInstance;
			pScript->m_pInstance =  nullptr;
		};
	}
};

#endif