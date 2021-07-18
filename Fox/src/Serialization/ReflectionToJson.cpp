//
// Created by samuel on 18/07/2021.
//

#include <json.hpp>
#include <Logger/Logger.hpp>
#include "Serialization/ReflectionToJson.hpp"

namespace fox
{
namespace serialization
{

/////////////////////////////////////////////////////////////////////////////////////////

        void to_json_recursively(const rttr::instance &obj, fox::json::Value &writer);

/////////////////////////////////////////////////////////////////////////////////////////

        bool write_variant(const rttr::variant &var, fox::json::Value &writer);

        bool write_atomic_types_to_json(const rttr::type &t, const rttr::variant &var, fox::json::Value &writer)
        {
            if (t.is_arithmetic()) {
                if (t == rttr::type::get<bool>())
                    writer = var.to_bool();
                else if (t == rttr::type::get<char>())
                    writer = var.to_bool();
                else if (t == rttr::type::get<int8_t>())
                    writer = var.to_int();
                else if (t == rttr::type::get<int16_t>())
                    writer = var.to_int();
                else if (t == rttr::type::get<int32_t>())
                    writer = var.to_int();
                else if (t == rttr::type::get<int64_t>())
                    writer = var.to_int();
                else if (t == rttr::type::get<uint8_t>())
                    writer = var.to_uint8();
                else if (t == rttr::type::get<uint16_t>())
                    writer = var.to_uint16();
                else if (t == rttr::type::get<uint32_t>())
                    writer = var.to_uint32();
                else if (t == rttr::type::get<uint64_t>())
                    writer = var.to_uint64();
                else if (t == rttr::type::get<float>())
                    writer = var.to_double();
                else if (t == rttr::type::get<double>())
                    writer = var.to_double();

                return true;
            } else if (t.is_enumeration()) {
                bool ok = false;
                auto result = var.to_string(&ok);
                if (ok) {
                    writer = var.to_string();
                } else {
                    ok = false;
                    auto value = var.to_uint64(&ok);
                    if (ok)
                        writer = value;
                }

                return true;
            } else if (t == rttr::type::get<std::string>()) {
                writer = var.to_string();
                return true;
            }

            return false;
        }

/////////////////////////////////////////////////////////////////////////////////////////

        static void write_array(const rttr::variant_sequential_view &view, fox::json::Value &writer)
        {
            fox::json::Value array = fox::json::Array();
            std::size_t i = 0;
            for (const auto &item : view) {
                if (item.is_sequential_container()) {
                    write_array(item.create_sequential_view(), array[i]);
                } else {
                    rttr::variant wrapped_var = item.extract_wrapped_value();
                    rttr::type value_type = wrapped_var.get_type();
                    if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() ||
                        value_type.is_enumeration()) {
                        write_atomic_types_to_json(value_type, wrapped_var, array[i]);
                    } else // object
                    {
                        to_json_recursively(wrapped_var, array[i]);
                    }
                }
                ++i;
            }
            writer = array;
        }


/////////////////////////////////////////////////////////////////////////////////////////

        static void write_associative_container(const rttr::variant_associative_view &view, fox::json::Value &writer)
        {
            static const std::string_view key_name("key");
            static const std::string_view value_name("value");

            std::size_t i = 0;

            if (view.is_key_only_type()) {
                for (auto &item : view) {
                    write_variant(item.first, writer[i]);
                    ++i;
                }
            } else {
                for (auto &item : view) {
                    write_variant(item.first, writer[key_name.data()]);
                    write_variant(item.second, writer[value_name.data()]);
                }
            }
        }

/////////////////////////////////////////////////////////////////////////////////////////

        bool write_variant(const rttr::variant &var, fox::json::Value &writer)
        {

            auto value_type = var.get_type();
            auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
            bool is_wrapper = wrapped_type != value_type;

            if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
                                           is_wrapper ? var.extract_wrapped_value() : var, writer)) {
            } else if (var.is_sequential_container()) {
                write_array(var.create_sequential_view(), writer);
            } else if (var.is_associative_container()) {
                write_associative_container(var.create_associative_view(), writer);
            } else {
                auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
                if (!child_props.empty()) {
                    to_json_recursively(var, writer);
                } else {
                    bool ok = false;
                    auto text = var.to_string(&ok);
                    if (!ok) {
                        writer = text;
                        return false;
                    }
                    writer = text;
                }
            }
            return true;
        }

/////////////////////////////////////////////////////////////////////////////////////////

        void to_json_recursively(const rttr::instance &obj2, fox::json::Value &writer)
        {
            rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

            auto prop_list = obj.get_derived_type().get_properties();
            for (auto prop : prop_list) {
                if (prop.get_metadata("NO_SERIALIZE"))
                    continue;

                rttr::variant prop_value = prop.get_value(obj);
                if (!prop_value)
                    continue; // cannot serialize, because we cannot retrieve the value

                const auto name = prop.get_name();
                if (!write_variant(prop_value, writer[name.data()])) {
                    std::cerr << "cannot serialize property: " << name << std::endl;
                }
            }
        }

        std::string to_json(rttr::instance obj)
        {
            if (!obj.is_valid())
                return std::string();

            fox::json::Value writer;
            to_json_recursively(obj, writer);

            return writer.dump();
        }
    } // end namespace serialization
} // end namespace fox