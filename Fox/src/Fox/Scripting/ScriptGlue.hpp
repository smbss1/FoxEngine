//
// Created by samuel on 03/10/22.
//

#ifndef FOXENGINE_SCRIPTGLUE_HPP
#define FOXENGINE_SCRIPTGLUE_HPP

namespace fox
{
    class ScriptGlue
    {
    public:
//        static void RegisterComponents();
        static void RegisterFunctions();
    };
}


//#define GLM_ENABLE_EXPERIMENTAL
//#include "glm/gtx/string_cast.hpp"
//
//template<glm::length_t L, typename T, glm::qualifier Q>
//inline std::ostream& operator<<(std::ostream& os, const glm::vec<L, T, Q>& vector)
//{
//    return os << glm::to_string(vector);
//}
//
//template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
//{
//    return os << glm::to_string(matrix);
//}
//
//template<typename OStream, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
//{
//    return os << glm::to_string(quaternion);
//}

#endif //FOXENGINE_SCRIPTGLUE_HPP
