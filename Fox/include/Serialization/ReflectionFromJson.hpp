//
// Created by samuel on 18/07/2021.
//

#ifndef FOXENGINE_REFLECTIONFROMJSON_HPP
#define FOXENGINE_REFLECTIONFROMJSON_HPP

#include <string>
#include <rttr/type>

namespace fox
{
    namespace json
    {
        class Value;
    }

    namespace serialization
    {

        void from_json_recursively(rttr::instance obj2, json::Value &json_object);

/*!
 * Serialize the given instance to a json encoded string.
 */
        bool from_json(const std::string& json, rttr::instance obj);

    }
}

#endif //FOXENGINE_REFLECTIONTOJSON_HPP