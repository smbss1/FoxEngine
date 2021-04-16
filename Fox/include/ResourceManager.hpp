#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Fox
{
	class IAssetManager
	{
	public:
		virtual bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept = 0;
		// virtual bool Add(const std::string& strAssetName, void* asset) noexcept = 0;
		virtual void* Get(const std::string& strAssetName) = 0;
		virtual bool Remove(const std::string& strAssetName) noexcept = 0;
		virtual bool Exists(const std::string& strAssetName) const = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////

	// template<typename TAsset>
	// class AssetManager : public IAssetManager
	// {
	// public:
	// 	using TAssetPtr = std::unique_ptr<TAsset>;

	// private:
	// 	std::unordered_map<std::string, TAssetPtr> assets;

	// public:
	// 	// bool Add(const std::string& strAssetName, void* pAsset) noexcept override
	// 	// {
	// 	// 	if (!Exists(strAssetName))
	// 	// 	{
	// 	// 		assets.emplace(strAssetName, static_cast<TAsset*>(pAsset));
	// 	// 		return true;
	// 	// 	}
	// 	// 	else
	// 	// 		return false;
	// 	// }

	// 	bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept override
	// 	{
	// 		if (!Exists(strAssetName))
	// 		{
	// 			assets.emplace(strAssetName, static_cast<TAsset*>(pAsset));
	// 			return true;
	// 		}
	// 		else
	// 			return false;
	// 	}

	// 	bool Exists(const std::string& strAssetName) const
	//     {
	//         return assets.find(strAssetName) != assets.end();
	//     }

	// 	void* Get(const std::string& strAssetName) override
	// 	{
	// 		if (Exists(strAssetName))
	// 			return static_cast<void*>(assets[strAssetName].get());
	// 		else
	// 			return nullptr;
	// 	}

	// 	bool Remove(const std::string& strAssetName) noexcept override
	// 	{
	// 		if (Exists(strAssetName))
	// 		{
	// 			assets.erase(strAssetName);
	// 			return true;
	// 		}
	// 		else
	// 			return false;
	// 	}
	// };

	/////////////////////////////////////////////////////////////////////////////////

	class ResourceManager
	{
	public:
		using IAssetManagerPtr = std::unique_ptr<IAssetManager>;

	private:
		std::unordered_map<std::string, IAssetManagerPtr> m_vAssetManagers;

	public:
		bool AddManager(const std::string& strAssetManagerName, IAssetManager* pAssetManager) noexcept
		{
			if (m_vAssetManagers.find(strAssetManagerName) == m_vAssetManagers.end())
			{
				m_vAssetManagers.emplace(strAssetManagerName, IAssetManagerPtr(pAssetManager));
				return true;
			}
			else
				return false;
		}

		bool Add(const std::string& strAssetManagerName, const std::string& strAssetName, const std::string& strAssetPath) noexcept
		{
			if (m_vAssetManagers.find(strAssetManagerName) != m_vAssetManagers.end())
				return m_vAssetManagers[strAssetManagerName]->Add(strAssetName, strAssetPath);
			else
				return false;
		}

		template<typename TAsset>
		TAsset* Get(const std::string& strAssetManagerName, const std::string& strAssetName)
		{
			if (m_vAssetManagers.find(strAssetManagerName) != m_vAssetManagers.end())
				return static_cast<TAsset*>(m_vAssetManagers[strAssetManagerName]->Get(strAssetName));
			else
				return nullptr;
		}

		bool Remove(const std::string& strAssetManagerName, const std::string& strAssetName) noexcept
		{
			if (m_vAssetManagers.find(strAssetManagerName) != m_vAssetManagers.end())
				return m_vAssetManagers[strAssetManagerName]->Remove(strAssetName);
			else
				return false;
		}
	};
} // namespace Fox