//
// Created by samuel on 11/10/22.
//

#include "Utils.hpp"
#include "Utils/FileSystem.hpp"

#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "ScriptEngine.hpp"

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
            };

        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
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
                fox::info("%.%", nameSpace, name);
            }
        }

        ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
        {
            std::string typeName = mono_type_get_name(monoType);

            auto it = s_ScriptFieldTypeMap.find(typeName);
            if (it == s_ScriptFieldTypeMap.end())
            {
                fox::error("Unknown type: %", typeName);
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
                    fox::error(Utils::MonoToString(string));
                else
                    fox::error("Mono Error !!");
                return true;
            }
            return false;
        }
    }
}
