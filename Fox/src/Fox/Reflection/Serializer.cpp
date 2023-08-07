////
//// Created by samuel on 09/11/22.
////
//
//#include "Serializer.hpp"
//#include "Core/Logger/Logger.hpp"
//#include "glm/vec3.hpp"
//#include "glm/vec2.hpp"
//#include "glm/vec4.hpp"
//#include <iostream>
//#include <cassert>
//
//YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
//{
//    out << YAML::Flow;
//    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
//    return out;
//}
//
//YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
//{
//    out << YAML::Flow;
//    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
//    return out;
//}
//
//YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
//{
//    out << YAML::Flow;
//    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
//    return out;
//}
//
//namespace fox
//{
//    using namespace rttr;
//
//    bool AttemptWriteFundementalType(const type& t, const variant& var, YAML::Emitter& out);
//    static void WriteArray(const variant_sequential_view& view, YAML::Emitter& out);
//    static void WriteAssociativeContainer(const variant_associative_view& view, YAML::Emitter& out);
//    bool WriteVariant(const variant& var, YAML::Emitter& out);
//    void SerialiazeObject(const instance& obj, YAML::Emitter& out, bool isRoot = false);
//
//    void SerializeYAML(const instance& obj, YAML::Emitter& out)
//    {
//        SerialiazeObject(obj, out, true);
//    }
//
//    void SerialiazeObject(const instance& obj, YAML::Emitter& out, bool isRoot)
//    {
//        // Dealing with wrapped objects
//        std::unique_ptr<instance> localObj;
//        if(obj.get_type().get_raw_type().is_wrapper())
//            localObj = std::make_unique<instance>(obj.get_wrapped_instance());
//        else
//            localObj = std::make_unique<instance>(obj);
//        // Handling pointer polymorphism cases
//
////        if (!isRoot)
////        {
////            out << YAML::Key << localObj->get_type().get_raw_type().get_name().data();
////        }
//        out << YAML::BeginMap; // Class
//
//        auto prop_list = localObj->get_derived_type().get_properties();
//        for (auto prop : prop_list)
//        {
//            // Retrieving value from property
//            variant prop_value = prop.get_value(*localObj);
//            if (!prop_value)
//                continue; // cannot serialize, because we cannot retrieve the value
//            // Retrieve name of property
//            const auto name = prop.get_name();
//
//            out << YAML::Key << name.data();
//
//            // Serialize value of property using name
//            if (!WriteVariant(prop_value, out))
//            {
//                std::cerr << "Failed to serialize " << name << std::endl;
//            }
//        }
//        out << YAML::EndMap; // Class
//    }
//
//    bool WriteVariant(const variant& var, YAML::Emitter& out)
//    {
//        // Deal with wrapped type
//        variant localVar = var;
//        type varType = var.get_type();
//        if(varType.is_wrapper())
//        {
//            varType = varType.get_wrapped_type();
//            localVar = localVar.extract_wrapped_value();
//        }
//
//        if (AttemptWriteFundementalType(varType, localVar, out))
//        {
//            // Successful write!
//        }
//            // If its not a fundamental, is it a sequential?
//        else if (var.is_sequential_container())
//        {
//            WriteArray(var.create_sequential_view(), out);
//        }
//            // Is it associative
//        else if (var.is_associative_container())
//        {
//            WriteAssociativeContainer(var.create_associative_view(), out);
//        }
//        else
//        {
//            // Not a fundemental, or a container. Check if it's an object
//            auto child_props = varType.get_properties();
//            if (!child_props.empty())
//            {
//                // We have properties, thus we can be serialized as an object
//                SerialiazeObject(var, out);
//            }
//            else
//            {
//                // Assert
//                // Some unknown type that is not a fundamental, has no properties, and is not a data structure
//                // Probably some registration issue
//                // Or it's a pointer! I handled pointers in here my game code
//                assert("Unknown RTTR serilization edge case that we haven't discovered");
//                return false;
//            }
//        }
//
//        return true;
//    }
//
//    bool AttemptWriteFundementalType(const type& t, const variant& var, YAML::Emitter& out)
//    {
//        // Json Number
//        if (t.is_arithmetic())
//        {
//            if (t == type::get<bool>())
//                out << YAML::Value << var.to_bool();
//            else if (t == type::get<char>())
//                out << YAML::Value << var.to_bool();
//            else if (t == type::get<int8_t>())
//                out << YAML::Value << var.to_int8();
//            else if (t == type::get<int16_t>())
//                out << YAML::Value << var.to_int16();
//            else if (t == type::get<int32_t>())
//                out << YAML::Value << var.to_int32();
//            else if (t == type::get<int64_t>())
//                out << YAML::Value << var.to_int64();
//            else if (t == type::get<uint8_t>())
//                out << YAML::Value << var.to_uint8();
//            else if (t == type::get<uint16_t>())
//                out << YAML::Value << var.to_uint16();
//            else if (t == type::get<uint32_t>())
//                out << YAML::Value << var.to_uint32();
//            else if (t == type::get<uint64_t>())
//                out << YAML::Value << var.to_uint64();
//            else if (t == type::get<float>())
//                out << YAML::Value << var.to_float();
//            else if (t == type::get<double>())
//                out << YAML::Value << var.to_double();
//            return true;
//        }
//            // Enumeration as string
//        else if (t.is_enumeration())
//        {
//            bool ok = false;
//            // Attempt to serialize as string
//            auto result = var.to_string(&ok);
//            if (ok)
//            {
//                out << YAML::Value << result;
//            }
//            else
//            {
//                // Attempt to serialize as number
//                auto value = var.to_uint64(&ok);
//                if (ok)
//                    out << YAML::Value << uint64_t(value);
////                else
////                    writer = nullptr;
//            }
//            return true;
//        }
//            // Strings!
//        else if (t == type::get<std::string>())
//        {
//            out << YAML::Value << var.to_string();
//            return true;
//        }
//        else if (t == type::get<glm::vec3>())
//        {
//            out << YAML::Value << var.convert<glm::vec3>();
//            return true;
//        }
//        else if (t == type::get<glm::vec2>())
//        {
//            out << YAML::Value << var.convert<glm::vec2>();
//            return true;
//        }
//        else if (t == type::get<glm::vec4>())
//        {
//            out << YAML::Value << var.convert<glm::vec4>();
//            return true;
//        }
//
//        // Not a fundamental type we know how to process
//        return false;
//    }
//
//    static void WriteArray(const variant_sequential_view& view, YAML::Emitter& out)
//    {
//        out << YAML::Value;
////        if (view.get_type().get_metadata("FLOW") == true) {
////            FOX_INFO("kjsdnldsljvds");
////            out << YAML::Flow;
////        }
//
//        // Init array
//        out << YAML::BeginSeq;
//
//        int i = 0;
//        for (const auto& item : view)
//        {
//            WriteVariant(item, out);
//            i++;
//        }
//        out << YAML::EndSeq;
//    }
//
//    static void WriteAssociativeContainer(const variant_associative_view& view, YAML::Emitter& out)
//    {
//        static const string_view key_name("key");
//        static const string_view value_name("value");
//
//        out << YAML::Value << YAML::BeginMap;
//
//        int i = 0;
//        // Dealing with keys = values containers like sets
//        if (view.is_key_only_type())
//        {
//            for (auto& item : view)
//            {
//                WriteVariant(item.first, out);
//                i++;
//            }
//        }
//        else
//        {
//            for (auto& item : view)
//            {
//                out << YAML::Key;
//                WriteVariant(item.first, out);
//                out << YAML::Value;
//                WriteVariant(item.second, out);
//                i++;
//            }
//        }
//        out << YAML::EndMap;
//    }
//}
