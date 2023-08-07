////
//// Created by samuel on 08/11/22.
////
//
//#ifndef FOX_LEXER_MACROS_HPP
//#define FOX_LEXER_MACROS_HPP
//
//#include "Hash.hpp"
//
//#include "Type.hpp"
//
//namespace fox::reflection
//{
//#define BEGIN_TYPENAME(TYPE, NAME) \
//    template<> \
//    TypeInfo* GetType<TYPE>() {    \
//        using Type = TYPE;          \
//        static TypeInfo type;      \
//          if (type.isInit) return &type;                         \
//          type.name = NAME;                 \
//          type.id = Hash::FNVHash(NAME);    \
//          type.size = sizeof(Type); \
//          bool IsVoid = std::is_void_v<Type>; \
//          bool IsArithmetic = std::is_arithmetic_v<Type>; \
//          bool IsArray = std::is_array_v<Type>;           \
//          bool IsClass = std::is_class_v<std::remove_pointer_t<Type>>;           \
//          bool IsUnion = std::is_union_v<Type>;           \
//          bool IsEnum = std::is_enum_v<Type>;           \
//          bool IsFunction = std::is_function_v<Type>;           \
//                                    \
//          if (IsArithmetic) {       \
//                type.kind = Kind::Primitive;   \
//          } else if (IsVoid) {       \
//                type.kind = Kind::Void;   \
//          } else if (IsArray) {       \
//                type.kind = Kind::Array;   \
//          } else if (IsClass) {       \
//                type.kind = Kind::Class;   \
//          } else if (IsUnion) {       \
//                type.kind = Kind::Union;   \
//          } else if (IsEnum) {       \
//                type.kind = Kind::Enum;   \
//          } else if (IsFunction) {       \
//                type.kind = Kind::Function;   \
//          }                        \
//            type.isInit = true;
//
//#define END_TYPENAME \
//          return &type; \
//    }
//
//#define DEFINE_TYPENAME(TYPE, NAME) \
//    BEGIN_TYPENAME(TYPE, NAME) \
//    END_TYPENAME
//
//
//#define ADD_CONVERSION(TO) type.AddConversion<Type, TO>();
//
//#define DEFINE_TYPE(TYPE) DEFINE_TYPENAME(TYPE, #TYPE)
//#define BEGIN_TYPE(TYPE) BEGIN_TYPENAME(TYPE, #TYPE)
//#define END_TYPE END_TYPENAME
//
//
//#define BEGIN_ATTRIBUTES_FOR(CLASS) \
//    DEFINE_TYPE(CLASS)    \
//    template<> \
//    ClassInfo* GetClass<CLASS>() { \
//      using ClassType = CLASS; \
//      static ClassInfo localClass; \
//      enum { BASE = __COUNTER__ }; \
//
//#define DEFINE_MEMBER(NAME) \
//    {                      \
//          enum { NAME##Index = __COUNTER__ - BASE - 1}; \
//          std::shared_ptr<MemberFieldInfo<ClassType, decltype(ClassType::NAME)>> field = std::make_shared<MemberFieldInfo<ClassType, decltype(ClassType::NAME)>>(); \
//          field->type = GetType<decltype(ClassType::NAME)>();\
//          field->name = { #NAME };  \
//          field->memberPtr = &ClassType::NAME;    \
//          localClass.fields[NAME##Index] = field; \
//    }
//
//#define DEFINE_MEMBER_FUNC(GET, SET, NAME) \
//    {                      \
//          enum { NAME##Index = __COUNTER__ - BASE - 1}; \
//          std::shared_ptr<SetterGetterFieldInfo<&GET, &SET, ClassType>> field = std::make_shared<SetterGetterFieldInfo<&GET, &SET, ClassType>>(); \
//          field->name = { #NAME };      \
//          localClass.fields[NAME##Index] = field; \
//    }
//
//#define END_ATTRIBUTES \
//      return &localClass; \
//    }
//
//}
//
//#endif //FOX_LEXER_MACROS_HPP
