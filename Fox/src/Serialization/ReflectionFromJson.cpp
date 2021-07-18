//
// Created by samuel on 18/07/2021.
//

#include <json.hpp>
#include "Serialization/ReflectionFromJson.hpp"

namespace fox
{
    namespace serialization
    {

/////////////////////////////////////////////////////////////////////////////////////////

        void from_json_recursively(rttr::instance obj, json::Value &json_object);

/////////////////////////////////////////////////////////////////////////////////////////

        rttr::variant extract_basic_types(json::Value &json_value)
        {
            switch (json_value.get_type()) {
                case json::json_type::string:
                    return json_value.get<std::string>();
                case json::json_type::null:
                    break;
                case json::json_type::boolean:
                    return json_value.get<bool>();
                case json::json_type::number_integer:
                    return json_value.get<int>();
                case json::json_type::number_unsigned:
                    return json_value.get<uint32_t>();
                case json::json_type::number_float:
                    return json_value.get<double>();
                    // we handle only the basic types here
                case json::json_type::object:
                case json::json_type::array:
                    return rttr::variant();
            }

            return rttr::variant();
        }


/////////////////////////////////////////////////////////////////////////////////////////

        static void write_array_recursively(rttr::variant_sequential_view &view, json::Value &json_array_value)
        {
            json::Array json_array = json_array_value.get<json::Array>();
            view.set_size(json_array.size());
            const rttr::type array_value_type = view.get_rank_type(1);

            for (std::size_t i = 0; i < json_array.size(); ++i) {
                auto &json_index_value = json_array_value[i];
                if (json_index_value.is_array()) {
                    auto sub_array_view = view.get_value(i).create_sequential_view();
                    write_array_recursively(sub_array_view, json_index_value);
                } else if (json_index_value.is_object()) {
                    rttr::variant var_tmp = view.get_value(i);
                    rttr::variant wrapped_var = var_tmp.extract_wrapped_value();
                    from_json_recursively(wrapped_var, json_index_value);
                    view.set_value(i, wrapped_var);
                } else {
                    rttr::variant extracted_value = extract_basic_types(json_index_value);
                    if (extracted_value.convert(array_value_type))
                        view.set_value(i, extracted_value);
                }
            }
        }

        rttr::variant extract_value(json::Value &itr, const rttr::type &t)
        {
            auto &json_value = itr;
            rttr::variant extracted_value = extract_basic_types(json_value);
            const bool could_convert = extracted_value.convert(t);
            if (!could_convert) {
                if (json_value.is_object()) {
                    rttr::constructor ctor = t.get_constructor();
                    for (auto &item : t.get_constructors()) {
                        if (item.get_instantiated_type() == t)
                            ctor = item;
                    }
                    extracted_value = ctor.invoke();
                    from_json_recursively(extracted_value, json_value);
                }
            }

            return extracted_value;
        }

        static void write_associative_view_recursively(rttr::variant_associative_view &view, json::Value &json_array_value)
        {
            json::Array json_array = json_array_value.get<json::Array>();

            for (std::size_t i = 0; i < json_array.size(); ++i) {
                auto &json_index_value = json_array_value[i];
                if (json_index_value.is_object()) // a key-value associative view
                {
                    json::Value key_itr = json_index_value["key"];
                    json::Value value_itr = json_index_value["value"];

                    if (!key_itr.is_null() && !value_itr.is_null()) {
                        auto key_var = extract_value(key_itr, view.get_key_type());
                        auto value_var = extract_value(value_itr, view.get_value_type());
                        if (key_var && value_var) {
                            view.insert(key_var, value_var);
                        }
                    }
                } else // a key-only associative view
                {
                    rttr::variant extracted_value = extract_basic_types(json_index_value);
                    if (extracted_value && extracted_value.convert(view.get_key_type()))
                        view.insert(extracted_value);
                }
            }
        }

/////////////////////////////////////////////////////////////////////////////////////////

        void from_json_recursively(rttr::instance obj2, json::Value &json_object)
        {
            rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
            const auto prop_list = obj.get_derived_type().get_properties();

            for (auto prop : prop_list) {
                json::Value ret = json_object[prop.get_name().data()];
                if (ret.is_null())
                    continue;
                const rttr::type value_t = prop.get_type();

                auto &json_value = ret;
                switch (json_value.get_type()) {
                    case json::json_type::array: {
                        rttr::variant var;
                        if (value_t.is_sequential_container()) {
                            var = prop.get_value(obj);
                            auto view = var.create_sequential_view();
                            write_array_recursively(view, json_value);
                        } else if (value_t.is_associative_container()) {
                            var = prop.get_value(obj);
                            auto associative_view = var.create_associative_view();
                            write_associative_view_recursively(associative_view, json_value);
                        }

                        prop.set_value(obj, var);
                        break;
                    }
                    case json::json_type::object: {
                        rttr::variant var = prop.get_value(obj);
                        from_json_recursively(var, json_value);
                        prop.set_value(obj, var);
                        break;
                    }
                    default: {
                        rttr::variant extracted_value = extract_basic_types(json_value);
                        if (extracted_value.convert(value_t)) // REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
                            prop.set_value(obj, extracted_value);
                    }
                }
            }
        }

        bool from_json(const std::string &json, rttr::instance obj)
        {
            json::Value document = json::parse(json);

            if (document.is_null())
                return true;

            from_json_recursively(obj, document);

            return true;
        }
    } // end namespace serialization
} // end namespace fox