//
// Created by samuel on 11/10/22.
//

#include "Utils.hpp"
#include "Marshal.hpp"
#include "ScriptEngine.hpp"
#include "ScriptCache.hpp"
#include "Utils/FileSystem.hpp"
#include "ValueWrapper.hpp"

#include <mono/metadata/class.h>
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "Asset/Asset.hpp"
#include "mono/metadata/mono-debug.h"

namespace fox
{
    namespace Utils
    {
        static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
            {
                { "System.Single", ScriptFieldType::Float },
                { "System.Double", ScriptFieldType::Double },
                { "System.Boolean", ScriptFieldType::Bool },
                { "System.Char", ScriptFieldType::Char },
                { "System.Int16", ScriptFieldType::Short },
                { "System.Int32", ScriptFieldType::Int },
                { "System.Int64", ScriptFieldType::Long },
                { "System.Byte", ScriptFieldType::Byte },
                { "System.UInt16", ScriptFieldType::UShort },
                { "System.UInt32", ScriptFieldType::UInt },
                { "System.UInt64", ScriptFieldType::ULong },
                { "System.String", ScriptFieldType::String },

                { "Fox.Vector2", ScriptFieldType::Vector2 },
                { "Fox.Vector3", ScriptFieldType::Vector3 },
                { "Fox.Vector4", ScriptFieldType::Vector4 },

                { "Fox.Entity", ScriptFieldType::Entity },
                { "Fox.Prefab", ScriptFieldType::Prefab },
            };

        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPBD)
        {
            uint32_t fileSize = 0;
            char* fileData = FileSystem::ReadBytes(assemblyPath, &fileSize);

            // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

            if (status != MONO_IMAGE_OK)
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }

            if (loadPBD)
            {
                std::filesystem::path pdbPath = assemblyPath;
                pdbPath.replace_extension(".pdb");

                if (FileSystem::Exists(pdbPath))
                {
                    uint32_t pdbfileSize = 0;
                    char* pdbfileData = FileSystem::ReadBytes(pdbPath, &pdbfileSize);
                    mono_debug_open_image_from_memory(image, (const mono_byte*)pdbfileData, pdbfileSize);
                    FOX_INFO("Loaded PDB %", pdbPath);
                    delete[] pdbfileData;
                }
            }

            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
            mono_image_close(image);

            // Don't forget to free the file data
            delete[] fileData;
            return assembly;
        }

        void PrintAssemblyTypes(MonoAssembly* assembly)
        {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

            for (int32_t i = 0; i < numTypes; i++)
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
                FOX_CORE_INFO("%.%", nameSpace, name);
            }
        }

        ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
        {
            std::string typeName = mono_type_get_name(monoType);

            auto it = s_ScriptFieldTypeMap.find(typeName);
            if (it == s_ScriptFieldTypeMap.end())
            {
                FOX_CORE_ERROR("Unknown type: %", typeName);
                return ScriptFieldType::None;
            }

            return it->second;
        }

        std::string MonoToString(MonoString* monoString)
        {
            char* cStr = mono_string_to_utf8(monoString);
            std::string string = cStr;
            free(cStr);
            return std::move(string);
        }

        bool HandleException(MonoObject* exception)
        {
            if (exception != nullptr)
            {
                MonoObject* string_exception = nullptr;
                MonoString* string = mono_object_to_string(exception, &string_exception);
                if (string_exception == nullptr && string != nullptr)
                    FOX_ERROR(Utils::MonoToString(string));
                else
                    FOX_ERROR("Mono Error !!");
                return true;
            }
            return false;
        }

        Utils::ValueWrapper GetFieldValue(MonoObject* monoObject, const ManagedField* field)
        {
//            FOX_PROFILE_FUNC();
            return MonoObjectToValue(GetFieldValueObject(monoObject, field), field->Type);
        }

        MonoObject* GetFieldValueObject(MonoObject* monoObject, const ManagedField* field)
        {
//            FOX_PROFILE_FUNC();

            MonoObject* valueObject = nullptr;

//            if (field->Property)
//                valueObject = mono_property_get_value(field->Property, monoObject, nullptr, nullptr);
//            else
                valueObject = mono_field_get_value_object(ScriptEngine::GetAppDomain(), field->Field, monoObject);

            return valueObject;
        }

        void SetFieldValue(MonoObject* monoObject, const ManagedField* field, const Utils::ValueWrapper& value)
        {
//            FOX_PROFILE_FUNC();

            if (!field->IsWritable())
                return;

            if (monoObject == nullptr)
                return;

            if (!value.HasValue())
                return;

//            if (field->Property)
//            {
//                void* data[1];
//
//                if (field->Type.IsPrimitiveType())
//                    data[0] = value.GetRawData();
//                else
//                    data[0] = ValueToMonoObject(value, field->Type);
//
//                mono_property_set_value(field->Property, monoObject, data, nullptr);
//            }
//            else
//            {
                mono_field_set_value(monoObject, field->Field, field->Type.IsPrimitiveType() ? value.GetRawData() : ValueToMonoObject(value, field->Type));
//            }
        }

//        void SetFieldValue(MonoObject* monoObject, const ManagedField* field, const ManagedArray& value)
//        {
////            FOX_PROFILE_FUNC();
//
//            if (!field->IsWritable())
//                return;
//
//            if (monoObject == nullptr)
//                return;
//
//            if (value.Length() == 0)
//                return;
//
////            if (field->Property)
////            {
////                void* data[] = { value.GetMonoArray() };
////                mono_property_set_value(field->Property, monoObject, data, nullptr);
////            }
////            else
////            {
//                mono_field_set_value(monoObject, field->Field, value.GetMonoArray());
////            }
//        }

        Utils::ValueWrapper GetDefaultValueForType(const ManagedType& type)
        {
//            FOX_PROFILE_FUNC();

            switch (type.NativeType)
            {
                case ScriptFieldType::Bool: return Utils::ValueWrapper(false);
                case ScriptFieldType::Char: return Utils::ValueWrapper(0);
                case ScriptFieldType::Short: return Utils::ValueWrapper(0);
                case ScriptFieldType::Int: return Utils::ValueWrapper(0);
                case ScriptFieldType::Long: return Utils::ValueWrapper(0);
                case ScriptFieldType::Byte: return Utils::ValueWrapper(0);
                case ScriptFieldType::UByte: return Utils::ValueWrapper(0);
                case ScriptFieldType::UShort: return Utils::ValueWrapper(0);
                case ScriptFieldType::UInt: return Utils::ValueWrapper(0);
                case ScriptFieldType::ULong: return Utils::ValueWrapper(0);
                case ScriptFieldType::Float: return Utils::ValueWrapper(0.0f);
                case ScriptFieldType::Double: return Utils::ValueWrapper(0.0);
                case ScriptFieldType::String: return Utils::ValueWrapper("");
                case ScriptFieldType::Vector2: return Utils::ValueWrapper(glm::vec2(0.0f));
                case ScriptFieldType::Vector3: return Utils::ValueWrapper(glm::vec2(0.0f));
                case ScriptFieldType::Vector4: return Utils::ValueWrapper(glm::vec2(0.0f));
                case ScriptFieldType::Entity: return Utils::ValueWrapper(0);
                case ScriptFieldType::Prefab: return Utils::ValueWrapper(0);
                case ScriptFieldType::AssetHandle: return Utils::ValueWrapper(0);
            }

            return Utils::ValueWrapper();
        }

        Utils::ValueWrapper MonoObjectToValue(MonoObject* obj, const ManagedType& type)
        {
//            FOX_PROFILE_FUNC();

            if (obj == nullptr)
                return Utils::ValueWrapper();

            switch (type.TypeEncoding)
            {
                case MONO_TYPE_BOOLEAN:		return Utils::ValueWrapper((bool)Marshal::Unbox<MonoBoolean>(obj));
                case MONO_TYPE_CHAR:		return Utils::ValueWrapper(Marshal::Unbox<uint16_t>(obj));
                case MONO_TYPE_I1:			return Utils::ValueWrapper(Marshal::Unbox<int8_t>(obj));
                case MONO_TYPE_I2:			return Utils::ValueWrapper(Marshal::Unbox<int16_t>(obj));
                case MONO_TYPE_I4:			return Utils::ValueWrapper(Marshal::Unbox<int32_t>(obj));
                case MONO_TYPE_I8:			return Utils::ValueWrapper(Marshal::Unbox<int64_t>(obj));
                case MONO_TYPE_U1:			return Utils::ValueWrapper(Marshal::Unbox<uint8_t>(obj));
                case MONO_TYPE_U2:			return Utils::ValueWrapper(Marshal::Unbox<uint16_t>(obj));
                case MONO_TYPE_U4:			return Utils::ValueWrapper(Marshal::Unbox<uint32_t>(obj));
                case MONO_TYPE_U8:			return Utils::ValueWrapper(Marshal::Unbox<uint64_t>(obj));
                case MONO_TYPE_R4:			return Utils::ValueWrapper(Marshal::Unbox<float>(obj));
                case MONO_TYPE_R8:			return Utils::ValueWrapper(Marshal::Unbox<double>(obj));
                case MONO_TYPE_STRING:		return Utils::ValueWrapper(Marshal::MonoStringToUTF8((MonoString*)obj));
                case MONO_TYPE_VALUETYPE:
                {
                    ManagedClass* typeClass = type.TypeClass;

                    if (typeClass == FOX_CORE_CLASS(AssetHandle))
                        return Utils::ValueWrapper(Marshal::Unbox<AssetHandle>(obj));

                    if (typeClass == FOX_CORE_CLASS(Vector2))
                        return Utils::ValueWrapper(Marshal::Unbox<glm::vec2>(obj));

                    if (typeClass == FOX_CORE_CLASS(Vector3))
                        return Utils::ValueWrapper(Marshal::Unbox<glm::vec3>(obj));

                    if (typeClass == FOX_CORE_CLASS(Vector4))
                        return Utils::ValueWrapper(Marshal::Unbox<glm::vec4>(obj));

                    break;
                }
                case MONO_TYPE_CLASS:
                {
                    ManagedClass* typeClass = type.TypeClass;

                    if (type.CanAssignTo(FOX_CORE_CLASS(Entity)))
                    {
                        const ManagedField* field = ScriptCache::GetFieldByName(typeClass, "ID");
                        return GetFieldValue(obj, field);
                    }

                    const ManagedField* handleField = FOX_CACHED_FIELD(typeClass->FullName, "m_Handle");
                    if (handleField != nullptr)
                        return GetFieldValue(obj, handleField);

                    break;
                }
                case MONO_TYPE_ARRAY:
                case MONO_TYPE_SZARRAY:
                {
                    break;
                }
            }

            return Utils::ValueWrapper();
        }

        MonoObject* ValueToMonoObject(const Utils::ValueWrapper& value, const ManagedType& type)
        {
//            FOX_PROFILE_FUNC();

            switch (type.NativeType)
            {
                case ScriptFieldType::Bool:
                {
                    MonoBoolean val = (MonoBoolean)value.Get<bool>();
                    return Marshal::BoxValue(FOX_CACHED_CLASS("System.Boolean"), val);
                }
                case ScriptFieldType::Char: return Marshal::BoxValue(FOX_CACHED_CLASS("System.SByte"), value.Get<int8_t>());
                case ScriptFieldType::Short: return Marshal::BoxValue(FOX_CACHED_CLASS("System.Int16"), value.Get<int16_t>());
                case ScriptFieldType::Int: return Marshal::BoxValue(FOX_CACHED_CLASS("System.Int32"), value.Get<int32_t>());
                case ScriptFieldType::Long: return Marshal::BoxValue(FOX_CACHED_CLASS("System.Int64"), value.Get<int64_t>());
                case ScriptFieldType::Byte: return Marshal::BoxValue(FOX_CACHED_CLASS("System.Byte"), value.Get<uint8_t>());
                case ScriptFieldType::UShort: return Marshal::BoxValue(FOX_CACHED_CLASS("System.UInt16"), value.Get<uint16_t>());
                case ScriptFieldType::UInt: return Marshal::BoxValue(FOX_CACHED_CLASS("System.UInt32"), value.Get<uint32_t>());
                case ScriptFieldType::ULong: return Marshal::BoxValue(FOX_CACHED_CLASS("System.UInt64"), value.Get<uint64_t>());
                case ScriptFieldType::Float: return Marshal::BoxValue(FOX_CACHED_CLASS("System.Single"), value.Get<float>());
                case ScriptFieldType::Double: return Marshal::BoxValue(FOX_CACHED_CLASS("System.Double"), value.Get<double>());
                case ScriptFieldType::String: return (MonoObject*)Marshal::UTF8StringToMono(value.Get<std::string>());
                case ScriptFieldType::AssetHandle: return Marshal::BoxValue(FOX_CACHED_CLASS("Fox.AssetHandle"), value.Get<AssetHandle>());
                case ScriptFieldType::Vector2: return Marshal::BoxValue(FOX_CACHED_CLASS("Fox.Vector2"), value.Get<glm::vec2>());
                case ScriptFieldType::Vector3: return Marshal::BoxValue(FOX_CACHED_CLASS("Fox.Vector3"), value.Get<glm::vec3>());
                case ScriptFieldType::Vector4: return Marshal::BoxValue(FOX_CACHED_CLASS("Fox.Vector4"), value.Get<glm::vec4>());
                case ScriptFieldType::Prefab: return ManagedInstance::From("Fox.Prefab", value.Get<AssetHandle>())->GetManagedObject();
                case ScriptFieldType::Entity: return ManagedInstance::From("Fox.Entity", value.Get<UUID>())->GetManagedObject();
            }

            FOX_CORE_ASSERT(false, "Unsupported value type!");
            return nullptr;
        }

        void Erase(std::string& str, const char* chars)
        {
            for (size_t i = 0; i < strlen(chars); i++)
                str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
        }

        void Erase(std::string& str, const std::string& chars)
        {
            Erase(str, chars.c_str());
        }


        std::string ResolveMonoClassName(MonoClass* monoClass)
        {
            const char* classNamePtr = mono_class_get_name(monoClass);
            std::string className = classNamePtr != nullptr ? classNamePtr : "";

            if (className.empty())
                return "Unknown Class";

            MonoClass* nestingClass = mono_class_get_nesting_type(monoClass);
            if (nestingClass != nullptr)
            {
                className = ResolveMonoClassName(nestingClass) + "/" + className;
            }
            else
            {
                const char* classNamespacePtr = mono_class_get_namespace(monoClass);
                if (classNamespacePtr)
                    className = std::string(classNamespacePtr) + "." + className;
            }

            MonoType* classType = mono_class_get_type(monoClass);
            if (mono_type_get_type(classType) == MONO_TYPE_SZARRAY || mono_type_get_type(classType) == MONO_TYPE_ARRAY)
                Erase(className, "[]");

            return className;
        }

        bool CheckMonoError(MonoError& error)
        {
            bool hasError = !mono_error_ok(&error);

            if (hasError)
            {
                unsigned short errorCode = mono_error_get_error_code(&error);
                const char* errorMessage = mono_error_get_message(&error);

                FOX_CORE_ERROR_TAG("ScriptEngine", "Mono Error!");
                FOX_CORE_ERROR_TAG("ScriptEngine", "\tError Code: %", errorCode);
                FOX_CORE_ERROR_TAG("ScriptEngine", "\tError Message: %", errorMessage);
                mono_error_cleanup(&error);
                FOX_CORE_ASSERT(false);
            }

            return hasError;
        }

    }
}
