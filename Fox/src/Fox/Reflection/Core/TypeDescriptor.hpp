#ifndef TYPE_DESCRIPTOR_H
#define TYPE_DESCRIPTOR_H

#include <string>
#include <vector>
#include <map>
#include <type_traits>
#include <memory>
#include <iostream>

namespace fox::Reflect
{
	// fwd declarations
	class DataMember;
	class Function;
	class Constructor;
	class Base;
	class Conversion;
    class Enumeration;

	template <typename>
	class TypeFactory;

	class TypeDescriptor;

	// fwd declarations (for friend declarations inside TypeDescriptor)
	namespace Details
	{

		template <typename Type>
		TypeDescriptor *Resolve();

		template <typename Type>
		TypeDescriptor *Resolve(Type &&);

	}	// namespace Details

	class TypeDescriptor
	{
		template <typename> friend class TypeFactory;

		template <typename Type> friend TypeDescriptor *Details::Resolve();
		template <typename Type> friend TypeDescriptor *Details::Resolve(Type &&);

	public:

		template <typename Type, typename... Args>
		void AddConstructor();

		template <typename Type, typename... Args>
		void AddConstructor(Type (*)(Args...));

		template <typename B, typename T>
		void AddBase();

		template <typename C, typename T>
		void AddDataMember(T C::*dataMemPtr, const std::string &name);

		template <auto Setter, auto Getter, typename Type>
		void AddDataMember(const std::string &name);

		template <typename Ret, typename... Args>
		void AddMemberFunction(Ret freeFun(Args...), const std::string &name);

		template <typename C, typename Ret, typename... Args>
		void AddMemberFunction(Ret(C::*memFun)(Args...), const std::string &name);

		template <typename C, typename Ret, typename... Args>
		void AddMemberFunction(Ret(C::*memFun)(Args...) const, const std::string &name);

		template <typename From, typename To>
		void AddConversion();

        template<typename T>
        Enumeration* AddEnumeration(const std::string &name);

//        Enumeration* GetEnumeration() const;


//        Reflect::Enumeration enume;
//        enume.Add("Static", Rigidbody2D::BodyType::Static);
//        enume.Add("Kinematic", Rigidbody2D::BodyType::Kinematic);
//        enume.Add("Dynamic", Rigidbody2D::BodyType::Dynamic);
//
//        std::cout << enume.GetName(Rigidbody2D::BodyType::Kinematic).value_or("Unknown") << std::endl;
//        std::cout << enume.GetName(Rigidbody2D::BodyType::Static).value_or("Unknown") << std::endl;
//        std::cout << enume.GetName(Rigidbody2D::BodyType::Dynamic).value_or("Unknown") << std::endl;
//        std::cout << (uint32_t) *enume.GetValue<Rigidbody2D::BodyType>("Dynamic") << std::endl;
//        std::cout << (uint32_t) *enume.GetValue<Rigidbody2D::BodyType>("Static") << std::endl;
//        std::cout << (uint32_t) *enume.GetValue<Rigidbody2D::BodyType>("Kinematic") << std::endl;

		std::string const &GetName() const;

		std::size_t GetSize() const;

		const std::vector<std::unique_ptr<Constructor>>& GetConstructors() const;

		template <typename... Args>
		const Constructor *GetConstructor() const;

		const std::vector<std::unique_ptr<Base>>& GetBases() const;

		template <typename B>
		Base *GetBase() const;

		std::vector<DataMember*> GetDataMembers() const;

		DataMember *GetDataMember(const std::string &name) const;

		std::vector<Function*> GetMemberFunctions() const;

		const Function *GetMemberFunction(const std::string &name) const;

		const std::vector<std::unique_ptr<Conversion>>& GetConversions() const;

		template <typename To>
		Conversion *GetConversion() const;

        bool IsArray() const { return mIsArray; }
        bool IsMapView() const { return mIsMap; }

	private:
		std::string mName;
		std::size_t mSize;

		std::vector<std::unique_ptr<Base>> mBases;
		std::vector<std::unique_ptr<Conversion>> mConversions;
		std::vector<std::unique_ptr<Constructor>> mConstructors;
		std::vector<std::unique_ptr<DataMember>> mDataMembers;
		std::vector<std::unique_ptr<Function>> mMemberFunctions;
        std::vector<std::unique_ptr<Enumeration>> m_Enums;

		// C++ primary type categories
		bool mIsVoid;
		bool mIsIntegral;
		bool mIsFloatingPoint;
		bool mIsArray;
        bool mIsMap;
		bool mIsPointer;
		bool mIsPointerToDataMember;
		bool mIsPointerToMemberFunction;
		bool mIsNullPointer;
		//bool mIsLValueReference;
		//bool mIsRValueReference;
		bool mIsClass;
		bool mIsUnion;
		bool mIsEnum;
		bool mIsFunction;
	};

	namespace Details
	{
		/*
		* all cv and reference qualifiers are stripped, but pointers are distinct types
		* (i.e. int and int* have two distint type descriptors)
		*/
		template <typename T>
		using RawType = typename std::remove_cv<std::remove_reference_t<T>>::type;

		template <typename T>
		TypeDescriptor &GetTypeDescriptor()
		{
			static TypeDescriptor typeDescriptor;  // single instance of type descriptor per reflected type

			return typeDescriptor;
		}

		template <typename Type>
		TypeDescriptor *&GetTypeDescriptorPtr()
		{
			static TypeDescriptor *typeDescriptorPtr = nullptr;  // single instance of type descriptor ptr per reflected type

			return typeDescriptorPtr;
		}

		inline auto &GetTypeRegistry()
		{
			static std::map<std::string, TypeDescriptor*> typeRegistry;

			return typeRegistry;
		}

		template <typename Type>
		inline constexpr auto GetTypeSize() -> typename std::enable_if<!std::is_same<RawType<Type>, void>::value, std::size_t>::type
		{
			return sizeof(Type);
		}

		template <typename Type>
		inline constexpr auto GetTypeSize() -> typename std::enable_if<std::is_same<RawType<Type>, void>::value, std::size_t>::type
		{
			return 0U;
		}

        template <typename T>
        struct getInnerType
        {
			using type = T;
		};

        template <template <typename> typename wrap, typename Inner>
        struct getInnerType<wrap<Inner>> {
            using type = Inner;
        };

        template <typename T> // For more convenient use as pointed out in comments
        using getInnerType_t = typename getInnerType<T>::type;

        template<typename>
        struct is_array : public std::false_type { };

        template<typename Tp, std::size_t Size>
        struct is_array<Tp[Size]> : public std::true_type { };

        template<typename Tp>
        struct is_array<Tp[]> : public std::true_type { };

        template<class T, std::size_t N>
        struct is_array<std::array<T, N>> : std::true_type {};


        template<typename T>
        struct has_const_iterator
        {
        private:
            typedef char                      yes;
            typedef struct { char array[2]; } no;

            template<typename C> static yes test(typename C::const_iterator*);
            template<typename C> static no  test(...);
        public:
            static const bool value = sizeof(test<T>(0)) == sizeof(yes);
            typedef T type;
        };

        template <typename T>
        struct has_begin_end
        {
            template<typename C> static char (&f(typename std::enable_if<
                std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
                    typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

            template<typename C> static char (&f(...))[2];

            template<typename C> static char (&g(typename std::enable_if<
                std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
                    typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

            template<typename C> static char (&g(...))[2];

            static bool const beg_value = sizeof(f<T>(0)) == 1;
            static bool const end_value = sizeof(g<T>(0)) == 1;
        };


        template <template <typename> typename wrap, typename Inner>
        struct is_array<wrap<Inner>> : std::integral_constant<bool, has_const_iterator<wrap<Inner>>::value && has_begin_end<wrap<Inner>>::beg_value && has_begin_end<wrap<Inner>>::end_value> {};

        template <typename T>
        inline constexpr bool is_array_v = is_array<T>::value;

        template<typename>
        struct is_map : public std::false_type { };

        template <template <typename, typename> typename wrap, typename Inner>
        struct is_map<wrap<Inner, std::allocator<Inner>>> : public std::false_type { };

//        template <typename Inner>
//        struct is_map<std::vector<Inner>> : public std::false_type { };

//        template <template <typename T> typename wrap, typename Inner>
//        struct is_map<wrap<Inner>> : public std::integral_constant<bool, has_const_iterator<wrap<Inner>>::value && has_begin_end<wrap<Inner>>::beg_value && has_begin_end<wrap<Inner>>::end_value> {};

        template <>
        struct is_map<std::string> : public std::false_type { };

        template <template <typename Key, typename Value> typename wrap, typename Key, typename Value>
        struct is_map<wrap<Key, Value>> : public std::integral_constant<bool, has_const_iterator<wrap<Key, Value>>::value && has_begin_end<wrap<Key, Value>>::beg_value && has_begin_end<wrap<Key, Value>>::end_value> {};

        template <typename T>
        inline constexpr bool is_map_v = is_map<T>::value;

        // internal function template that returns a type descriptor by type
		template <typename Type>
		TypeDescriptor *Resolve()
		{
//            std::cout << typeid(getInnerType_t<Type>).name() << std::endl;

			TypeDescriptor *&typeDescPtr = GetTypeDescriptorPtr<RawType<Type>>();

			if (!typeDescPtr)  // create a type descriptor if not present
			{
				TypeDescriptor &typeDesc = GetTypeDescriptor<RawType<Type>>();
				typeDescPtr = &typeDesc;

				typeDesc.mSize = GetTypeSize<Type>();

				typeDesc.mIsVoid = std::is_void_v<Type>;
				typeDesc.mIsIntegral = std::is_integral_v<Type>;
				typeDesc.mIsFloatingPoint = std::is_floating_point_v<Type>;
                typeDesc.mIsArray = is_array_v<Type>;
                typeDesc.mIsMap = is_map_v<Type>;
				typeDesc.mIsPointer = std::is_pointer_v<Type>;
				typeDesc.mIsPointerToDataMember = std::is_member_object_pointer_v<Type>;
				typeDesc.mIsPointerToMemberFunction = std::is_member_function_pointer_v<Type>;
				typeDesc.mIsNullPointer = std::is_null_pointer_v<Type>;
				//typeDesc.mIsLValueReference = std::is_lvalue_reference_v<Type>;
				//typeDesc.mIsRValueReference = std::is_rvalue_reference_v<Type>;
				typeDesc.mIsClass = std::is_class_v<std::remove_pointer_t<Type>>;
				typeDesc.mIsUnion = std::is_union_v<Type>;
				typeDesc.mIsEnum = std::is_enum_v<Type>;
				typeDesc.mIsFunction = std::is_function_v<Type>;
			}

			return typeDescPtr;
		}

		// internal function template that returns a type descriptor by object
		template <typename Type>
		TypeDescriptor *Resolve(Type &&object)
		{
			if (!GetTypeDescriptorPtr<RawType<Type>>())
				Resolve<Type>();

			return GetTypeDescriptorPtr<RawType<Type>>();
		}

	}  // namespace Details

}  // namespace Reflect

#include "DataMember.hpp"
#include "Function.hpp"
#include "Constructor.hpp"
#include "Base.hpp"
#include "Conversion.hpp"

#include "TypeDescriptor.inl"

#endif // TYPE_DESCRIPTOR_H
