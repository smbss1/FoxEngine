
#pragma once
#include "Locator.hpp"

template<typename T>
void Locator::RegisterInstance(T* pInstance)
{
	int iHash = typeid(T).hash_code();
	if (m_vInstances.find(iHash) == m_vInstances.end())
		m_vInstances.emplace(iHash, pInstance);
}

template<typename T>
void Locator::RegisterInstance(std::shared_ptr<T> pInstance)
{
	int iHash = typeid(T).hash_code();
	if (m_vInstances.find(iHash) == m_vInstances.end())
		m_vInstances.emplace(iHash, pInstance);
}

template<typename T>
void Locator::RegisterInstance(std::unique_ptr<T> pInstance)
{
	int iHash = typeid(T).hash_code();
	if (m_vInstances.find(iHash) == m_vInstances.end())
		m_vInstances.emplace(iHash, pInstance);
}

template<typename T>
void Locator::RegisterCreator(std::function<std::shared_ptr<T>()> pCreator)
{
	int iHash = typeid(T).hash_code();
	if (m_vCreators.find(iHash) == m_vCreators.end())
		m_vCreators.emplace(iHash, pCreator);
}

template<typename T>
std::shared_ptr<T> Locator::Resolve() const
{
	int iHash = typeid(T).hash_code();
	auto itr1 = m_vInstances.find(iHash);
	if (itr1 != m_vInstances.end())
		return std::static_pointer_cast<T>(itr1->second);

	auto itr2 = m_vCreators.find(iHash);
	if (itr2 != m_vCreators.end())
		return std::static_pointer_cast<T>(itr2->second());

	return nullptr;
}