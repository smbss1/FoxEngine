//
// Created by samuel on 10/10/22.
//

#ifndef FOXENGINE_GCMANAGER_HPP
#define FOXENGINE_GCMANAGER_HPP

#include <cstdint>

typedef struct _MonoObject MonoObject;

namespace fox
{
    using GCHandle = uint32_t;

    class GCManager
    {
    public:
        static void Init();
        static void Shutdown();

        static void CollectGarbage(bool blockUntilFinalized = true);

        static GCHandle CreateObjectReference(MonoObject* managedObject, bool weakReference, bool pinned = false, bool track = true);
        static MonoObject* GetReferencedObject(GCHandle handle);
        static void ReleaseObjectReference(GCHandle handle);
    };
}

#endif //FOXENGINE_GCMANAGER_HPP
