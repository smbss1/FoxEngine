//
// Created by samuel on 22/10/22.
//

#include "Ref.hpp"
#include "Assert.hpp"
#include <unordered_set>
#include <mutex>

namespace fox
{
    static std::unordered_set<void*> s_LiveReferences;
    static std::mutex s_LiveReferenceMutex;

    namespace RefUtils {

        void AddToLiveReferences(void* instance)
        {
            std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
            FOX_CORE_ASSERT(instance);
            s_LiveReferences.insert(instance);
        }

        void RemoveFromLiveReferences(void* instance)
        {
            std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
            FOX_CORE_ASSERT(instance);
            FOX_CORE_ASSERT(s_LiveReferences.find(instance) != s_LiveReferences.end());
            s_LiveReferences.erase(instance);
        }

        bool IsLive(void* instance)
        {
            FOX_CORE_ASSERT(instance);
            return s_LiveReferences.find(instance) != s_LiveReferences.end();
        }
    }


}
