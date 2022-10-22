//
// Created by samuel on 22/10/22.
//

#include "FieldStorage.hpp"
#include "ScriptEngine.hpp"

namespace fox
{
    FieldStorage::FieldStorage(ManagedClass* managedClass, ManagedField* field) : Field(field)
    {
        Init(managedClass, field);
    }

    void FieldStorage::Init(ManagedClass* managedClass, ManagedField* field)
    {
        Field = field;
        m_RuntimeInstance = -1;

        MonoObject* tempInstance = ScriptEngine::CreateManagedObject_Internal(managedClass);

//        if (Field->Type.IsArray())
//        {
//            m_DefaultArrayValue = ManagedArray::FromArrayObject((MonoArray*)ScriptUtils::GetFieldValueObject(tempInstance, field), m_Field->Type);
//        }
//        else
//        {
            m_DefaultValue = Utils::GetFieldValue(tempInstance, field);

            if (!m_DefaultValue.HasValue())
                m_DefaultValue = Utils::GetDefaultValueForType(field->Type);
//        }
        m_ValueCache = m_DefaultValue;
    }
}
