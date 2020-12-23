#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>

class Locator
{
public:
	Locator();
	~Locator();

	void Clear();

	template<typename T>
	void RegisterInstance(T* instance = new T());
	template<typename T>
	void RegisterInstance(std::shared_ptr<T> pInstance);
	template<typename T>
	void RegisterInstance(std::unique_ptr<T> pInstance);

	template<typename T>
	void RegisterCreator(std::function<std::shared_ptr<T>()> creator = []() { return std::make_shared<T>(); });

	template<typename T>
	std::shared_ptr<T> Resolve() const;

private:
	std::unordered_map<int, std::shared_ptr<void>> m_vInstances;
	std::unordered_map<int, std::function<std::shared_ptr<void>()>> m_vCreators;
};

#include "Locator.inl"