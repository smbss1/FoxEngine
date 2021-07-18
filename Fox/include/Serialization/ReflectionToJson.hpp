//
// Created by samuel on 18/07/2021.
//

#ifndef FOXENGINE_REFLECTIONTOJSON_HPP
#define FOXENGINE_REFLECTIONTOJSON_HPP

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

        void to_json_recursively(const rttr::instance &obj2, json::Value& writer);

/*!
 * Serialize the given instance to a json encoded string.
 */
        std::string to_json(rttr::instance obj);

    }
}

#endif //FOXENGINE_REFLECTIONTOJSON_HPP