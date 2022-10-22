//
// Created by samuel on 22/10/22.
//

#ifndef FOXENGINE_FIELDSTORAGE_HPP
#define FOXENGINE_FIELDSTORAGE_HPP

#include <cstddef>
#include "ValueWrapper.hpp"
#include "ScriptTypes.hpp"
#include "Utils.hpp"

namespace fox
{
    class FieldStorage : public RefCounted
    {
    public:
        FieldStorage(ManagedClass* managedClass, ManagedField* field);

        void RegisterInstance(GCHandle instance)
        {
            m_RuntimeInstance = instance;

//            if (!Field->Type.IsArray())
//            {
//                if (!m_DefaultValue.HasValue())
//                    return;
//
////                if (m_ValueCache.find(instance) != m_ValueCache.end())
////                    return;
//                m_ValueCache = m_DefaultValue;
//            }
////            else
////            {
////                if (m_ArrayStorage.find(instance) != m_ArrayStorage.end())
////                    return;
////
////                m_ArrayStorage[instance] = m_DefaultArrayValue.Copy();
////            }
        }

        void UnregisterInstance(GCHandle instance)
        {
            m_RuntimeInstance = -1;

//            if (!Field->Type.IsArray())
//            {
////                if (m_ValueCache.find(instance) == m_ValueCache.end())
////                    return;
//
//                m_ValueCache = m_DefaultValue;
//            }
////            else
////            {
////                if (m_ArrayStorage.find(instance) == m_ArrayStorage.end())
////                    return;
////
////                m_ArrayStorage.erase(instance);
////            }
        }

        template<typename T>
        T GetValue() const
        {
            return GetValue().template Get<T>();
        }

        Utils::ValueWrapper GetValue() const
        {
            if (m_RuntimeInstance != -1)
            {
                return Utils::GetFieldValue(GCManager::GetReferencedObject(m_RuntimeInstance), Field);
            }
            return m_ValueCache;
        }

        template<typename T>
        void SetValue(T value)
        {
            Utils::ValueWrapper wrappedValue = Utils::ValueWrapper(value);
            if (m_RuntimeInstance != -1)
            {
                Utils::SetFieldValue(GCManager::GetReferencedObject(m_RuntimeInstance), Field, wrappedValue);
            }
            else
            {
                m_ValueCache = wrappedValue;
            }
        }

        void SetValue(void* value, size_t size)
        {
            Utils::ValueWrapper wrappedValue = Utils::ValueWrapper(value, size);
            if (m_RuntimeInstance != -1)
            {
                Utils::SetFieldValue(GCManager::GetReferencedObject(m_RuntimeInstance), Field, wrappedValue);
            }
            else
            {
                m_ValueCache = wrappedValue;
            }
        }

        void Init(ManagedClass* managedClass, ManagedField* field);

        ManagedField* GetField() const { return Field; }

    private:
        ManagedField* Field;

        Utils::ValueWrapper m_ValueCache;
        GCHandle m_RuntimeInstance;

        Utils::ValueWrapper m_DefaultValue;

        friend class ScriptEngine;
        friend class ScriptInstance;
    };
}


#endif //FOXENGINE_FIELDSTORAGE_HPP
