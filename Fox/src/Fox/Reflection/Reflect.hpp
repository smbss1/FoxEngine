//
// Created by samuel on 12/11/22.
//

//#include <rttr/registration.h>

#ifndef FOX_LEXER_REFLECT_HPP
#define FOX_LEXER_REFLECT_HPP

//#include "rttr/array_range.h"
//#include "rttr/constructor.h"
//#include "rttr/destructor.h"
//#include "rttr/enumeration.h"
//#include "rttr/method.h"
//#include "rttr/property.h"
//#include "rttr/registration.h"
//#include "rttr/rttr_cast.h"
//#include "rttr/rttr_enable.h"
//#include "rttr/type.h"
//
//#define CAT_IMPL_(a, b) a##b
//#define CAT_(a, b) CAT_IMPL_(a, b)
//#ifdef __COUNTER__
//#define ANONYMOUS_VARIABLE(str) CAT_(str, __COUNTER__)
//#else
//#define ANONYMOUS_VARIABLE(str) CAT_(str, __LINE__)
//#endif

#define REFLECT_INLINE(cls)                                                                                  \
	template <typename T>                                                                                    \
	extern void rttr_auto_register_reflection_function_t();                                                  \
	template <>                                                                                              \
	void rttr_auto_register_reflection_function_t<cls>();                                                    \
	static const int ANONYMOUS_VARIABLE(auto_register__) = []() {                                            \
		rttr_auto_register_reflection_function_t<cls>();                                                     \
		return 0;                                                                                            \
	}();                                                                                                     \
	template <>                                                                                              \
	inline void rttr_auto_register_reflection_function_t<cls>()

#define REFLECT_EXTERN(cls)                                                                                  \
	template <typename T>                                                                                    \
	extern void rttr_auto_register_reflection_function_t();                                                  \
	template <>                                                                                              \
	void rttr_auto_register_reflection_function_t<cls>();                                                    \
	static const int ANONYMOUS_VARIABLE(auto_register__) = []() {                                            \
		rttr_auto_register_reflection_function_t<cls>();                                                     \
		return 0;                                                                                            \
	}();

#define REFLECT(cls)                                                                                         \
	template <>                                                                                              \
	void rttr_auto_register_reflection_function_t<cls>()

namespace fox
{
//    class Type;
//
//    class Value
//    {
//    public:
//        Value(rttr::variant value) : m_Value(value) {}
//
//    private:
//        rttr::variant m_Value;
//
//        friend class Member;
//    };
//
//    class Member
//    {
//    public:
//        Member(rttr::property prop) : m_Property(prop) {}
//
//        Value Get(rttr::instance instance) const
//        {
//            return m_Property.get_value(instance);
//        }
//
//        void Set(rttr::instance instance, Value value)
//        {
//            m_Property.set_value(instance, value.m_Value);
//        }
//
//        std::string_view GetName() const
//        {
//            return m_Property.get_name().data();
//        }
//
//        Type GetType() const;
//
//    private:
//        rttr::property m_Property;
//    };
//
//    class Type
//    {
//    public:
//        Type(rttr::type type) : m_Type(type) {}
//
//        const std::vector<Member>& GetMembers() const
//        {
//            std::vector<Member> members;
//            for (auto property : m_Type.get_properties())
//            {
//                members.push_back(Member(property));
//            }
//
//            return members;
//        }
//
//    private:
//        rttr::type m_Type;
//    };

//    template<typename>
//    struct wrapper_inner_type;
//
//    template<typename InnerType>
//    struct wrapper_inner_type<wrapperT<InnerType>> {
//        using type = InnerType;
//    };
//    template<typename T>
//    using wrapper_inner_type_t = typename wrapper_inner_type<T>::type;

//    template <typename wrapperType>
//    class InData{
//        getInnerType_t<wrapperType> var;   //<-- How to get inner type?
//    };

    class Reflection
    {
    public:
        static void Init();

//        template<typename T>
//        static Type Get()
//        {
//            return rttr::type::get<T>();
//        }
    };
}


#endif //FOX_LEXER_REFLECT_HPP
