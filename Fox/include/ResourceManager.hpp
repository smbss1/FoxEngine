
#ifndef FOX_RESOURCE_MANAGER_HPP_
#define FOX_RESOURCE_MANAGER_HPP_

#include <string>
#include <memory>
#include <unordered_map>

namespace fox
{
	class ResourceManager;

	class IAssetManager
	{
	public:
		virtual ~IAssetManager() = 0;
		virtual bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept = 0;
		virtual void* Get(const std::string& strAssetName) = 0;
		virtual bool Remove(const std::string& strAssetName) noexcept = 0;
	};

	template <typename TAsset>
	class AAssetManager : public IAssetManager
	{
	public:
		AAssetManager(ResourceManager& oResourceManager) : m_oResourceManager(oResourceManager)
		{
		}
		virtual ~AAssetManager() = 0;
		virtual bool Add(const std::string& strAssetName, const std::string& strAssetPath) noexcept = 0;
		virtual void* Get(const std::string& strAssetName) = 0;
		virtual bool Remove(const std::string& strAssetName) noexcept = 0;

		bool Exists(const std::string& strAssetName) const
		{
			return m_vAssets.find(strAssetName) != m_vAssets.end();
		}

	protected:
		ResourceManager& m_oResourceManager;

		using AssetPtr = std::unique_ptr<TAsset>;
		std::unordered_map<std::string, AssetPtr> m_vAssets;
	};

	class ResourceManager
	{
	public:
		using IAssetManagerPtr = std::unique_ptr<IAssetManager>;

	public:
		template <typename T>
		bool AddManager(IAssetManager* pAssetManager) noexcept
		{
			std::size_t lHash = typeid(T).hash_code();
			if (m_vAssetManagers.find(lHash) == m_vAssetManagers.end())
			{
				m_vAssetManagers.emplace(lHash, IAssetManagerPtr(pAssetManager));
				return true;
			}
			return false;
		}

		template <typename T>
		bool AddAsset(const std::string& strAssetName, const std::string& strAssetPath) noexcept
		{
			std::size_t lHash = typeid(T).hash_code();
			if (m_vAssetManagers.find(lHash) != m_vAssetManagers.end())
				return m_vAssetManagers[lHash]->Add(strAssetName, strAssetPath);
			return false;
		}

		template<typename TAsset>
		TAsset* GetAsset(const std::string& strAssetName)
		{
			std::size_t lHash = typeid(TAsset).hash_code();
			if (m_vAssetManagers.find(lHash) != m_vAssetManagers.end())
				return static_cast<TAsset*>(m_vAssetManagers[lHash]->Get(strAssetName));
			return nullptr;
		}

		template<typename TAsset>
		bool RemoveAsset(const std::string& strAssetName) noexcept
		{
			std::size_t lHash = typeid(TAsset).hash_code();
			if (m_vAssetManagers.find(lHash) != m_vAssetManagers.end())
				return m_vAssetManagers[lHash]->Remove(strAssetName);
			return false;
		}

	private:
		std::unordered_map<std::size_t, IAssetManagerPtr> m_vAssetManagers;
	};
} // namespace Fox

#endif