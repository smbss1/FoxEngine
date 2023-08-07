////
//// Created by samuel on 08/11/22.
////
//
//#ifndef FOX_LEXER_TYPE_HPP
//#define FOX_LEXER_TYPE_HPP
//
//#include <optional>
//#include "Core/Any.hpp"
//
//namespace fox::reflection
//{
//    enum class Kind
//    {
//        Primitive,
//        Class,
//        Array,
//        Void,
//        Function,
//        Enum,
//        Union,
//    };
//
//    std::string KindToString(Kind kind)
//    {
//        switch (kind)
//        {
//            case Kind::Primitive: return "Primitive";
//            case Kind::Class: return "Class";
//            case Kind::Array: return "Array";
//            case Kind::Void: return "Void";
//            case Kind::Function: return "Function";
//            case Kind::Enum: return "Enum";
//            case Kind::Union: return "Union";
//        }
//        return "Unknown Kind";
//    }
//
//    class Conversion;
//
//    struct Attributes
//    {
//        std::vector<uint32_t> attributes;
//
//        void RegisterAttribute(uint32_t attributeID)
//        {
//            auto it = std::find(attributes.begin(), attributes.end(), attributeID);
//            if (it == attributes.end())
//                attributes.push_back(attributeID);
//        }
//
//        void UnregisterAttribute(uint32_t attributeID)
//        {
//            attributes.erase(std::remove(attributes.begin(), attributes.end(), attributeID), attributes.end());
//        }
//
//        bool Exist(uint32_t attributeID)
//        {
//            auto it = std::find(attributes.begin(), attributes.end(), attributeID);
//            return it != attributes.end();
//        }
//    };
//
//    Attributes* GetAttributes()
//    {
//        static Attributes attributes;
//        return &attributes;
//    }
//
//    struct TypeInfo
//    {
//        std::string name;
//        uint32_t id;
//        size_t size;
//        Kind kind;
//        bool isInit;
//
//        std::vector<Conversion*> m_Conversions;
//
//        template <typename From, typename To>
//        void AddConversion();
//
//        template <typename To>
//        Conversion *GetConversion() const;
//    };
//
//    template<typename T>
//    TypeInfo* GetType()
//    {
//        return nullptr;
//    }
//
//    struct Any
//    {
//        void* Data = nullptr;
//        uint32_t Size = 0;
//        TypeInfo* type = nullptr;
//
//        Any()
//            : Data(nullptr), Size(0)
//        {
//        }
//
//        template <typename T, typename U = typename std::remove_cv<std::remove_reference_t<std::decay_t<T>>>::type, typename = typename std::enable_if<!std::is_same_v<U, Any>>::type>
//        Any(T&& value)
//        {
//            type = GetType<U>();
//            Allocate(sizeof(U));
//            memcpy(Data, &value, sizeof(U));
//        }
//
//        Any(const Any& other)
//        {
//            Allocate(other.Size);
//            type = other.type;
//            memcpy(Data, other.Data, other.Size);
//        }
//
////        Any(Any&& other)  noexcept : Data(other.Data), Size(other.Size), type(other.type)
////        {
////            other.Data = nullptr;
////            other.Size = 0;
////        }
//
//        ~Any()
//        {
//            Release();
//        }
//
//        static Any Copy(const Any& other)
//        {
//            Any buffer;
//            buffer.Allocate(other.Size);
//            buffer.type = other.type;
//            memcpy(buffer.Data, other.Data, other.Size);
//            return buffer;
//        }
//
//        void Allocate(uint32_t size)
//        {
//            delete[] (byte*)Data;
//            Data = nullptr;
//
//            if (size == 0)
//                return;
//
//            Data = new byte[size];
//            Size = size;
//
//            ZeroInitialize();
//        }
//
//        void Release()
//        {
//            delete[] (byte*)Data;
//            Data = nullptr;
//            Size = 0;
//        }
//
//        void ZeroInitialize()
//        {
//            if (Data)
//                memset(Data, 0, Size);
//        }
//
//        template<typename T>
//        T& Read(uint32_t offset = 0)
//        {
//            return *(T*)((byte*)Data + offset);
//        }
//
//        template<typename T>
//        const T& Read(uint32_t offset = 0) const
//        {
//            return *(T*)((byte*)Data + offset);
//        }
//
//        void Write(const void* data, uint32_t size, uint32_t offset = 0)
//        {
//            FOX_CORE_ASSERT(offset + size <= Size, "Any overflow!");
//            memcpy((byte*)Data + offset, data, size);
//        }
//
//        operator bool() const
//        {
//            return Data;
//        }
//
//        Any& operator=(const Any& other)
//        {
//            // Guard self assignment
//            if (this == &other)
//                return *this;
//
//            Allocate(other.Size);
//            type = other.type;
//            memcpy(Data, other.Data, other.Size);
//            return *this;
//        }
//
//        template<typename T>
//        T* As() const
//        {
//            return (T*)Data;
//        }
//
//        template<typename T>
//        T& AsRef() const
//        {
//            return *static_cast<T*>(Data);
//        }
//
//        void* Raw() const
//        {
//            return Data;
//        }
//
//        inline uint32_t GetSize() const { return Size; }
//
//        template <typename T>
//        Any TryConvert() const;
//
//        template <typename T>
//        const T* TryCast() const
//        {
//            const TypeInfo *typeDesc = GetType<T>();
//
//            void *casted = nullptr;
//
//            if (!*this)
//                return static_cast<T const*>(casted);
//
//            if (typeDesc == type)
//                casted = Data;
//            else
//            {
////                for (auto *base : type->GetBases())
////                    if (base->GetType() == typeDesc)
////                        casted = base->Cast(Data);
//            }
//
//            return static_cast<T const*>(casted);
//        }
//
//        template <typename T>
//        std::optional<T&> TryCastAsRef()
//        {
//            T* value = TryCast<T>();
//            if (value == nullptr)
//                return std::nullopt;
//            return *value;
//        }
//
//        template <typename T>
//        T* TryCast()
//        {
//            return const_cast<T*>(static_cast<const Any&>(*this).TryCast<T>());
//        }
//    };
//
//    struct FieldInfo
//    {
//        TypeInfo* type;
//        std::string name;
//
//        virtual void Set(Any objectRef, const Any value) = 0;
//        virtual Any Get(Any object) = 0;
//    };
//
//    // MAX_NUMBER_OF_FIELDS is arbitrarily large
//    struct ClassInfo
//    {
//        std::array<std::shared_ptr<FieldInfo>, 20> fields;
//    };
//
//    template<typename T>
//    ClassInfo* GetClass() = delete;
//
//
//    template <typename Class, typename T>
//    struct MemberFieldInfo : public FieldInfo
//    {
//        using member_ptr_t = T Class::*;
//
//        member_ptr_t memberPtr;
//
//        inline void Set(Any objectRef, const Any value) override
//        {
//
//        }
//
//        inline Any Get(Any object) override
//        {
//            Class& obj = object.AsRef<Class>();
//            return Any(obj.*memberPtr);
//        }
//
////        uint8_t* GetPtr(void* object) const
////        {
////            return reinterpret_cast<uint8_t*>(object) + offset;
////        }
//    };
//
//    // helper meta function to get info about functions passed as auto non type params (C++17)
//	template <typename>
//	struct FunctionHelper;
//
//	template <typename Ret, typename... Args>
//	struct FunctionHelper<Ret(Args...)>
//	{
//		using ReturnType = Ret;
//		using ParamsTypes = std::tuple<Args...>;
//	};
//
//	template <typename Class, typename Ret, typename... Args>
//	/*constexpr*/ FunctionHelper<Ret(/*Class, */Args...)> ToFunctionHelper(Ret(Class::*)(Args...));
//
//	template <typename Class, typename Ret, typename... Args>
//	/*constexpr*/ FunctionHelper<Ret(/*Class, */Args...)> ToFunctionHelper(Ret(Class::*)(Args...) const);
//
//	template <typename Ret, typename... Args>
//	/*constexpr*/ FunctionHelper<Ret(Args...)> ToFunctionHelper(Ret(*)(Args...));
//
//    template <auto Getter, auto Setter, typename Class>
//    struct SetterGetterFieldInfo : public FieldInfo
//    {
//        template <typename T>
//		using RawType = typename std::remove_cv<std::remove_reference_t<T>>::type;
//        using MemberType = RawType<typename decltype(ToFunctionHelper(Getter))::ReturnType>;
//
//        SetterGetterFieldInfo()
//        {
////            type = GetType<MemberType>();
//        }
//
//        inline void Set(Any objectRef, const Any value) override
//        {
//            Class* obj = objectRef.As<Class>();
//
////			if (!obj)
////				throw BadCastException(Details::Resolve<Class>()->GetName(), Any(objectRef).GetType()->GetName(), "object:");
//
//			MemberType const *casted = nullptr;
//			Any val;
//			if (casted = value.As<MemberType>(); !casted)
//			{
//				val = value.TryConvert<MemberType>();
//				casted = val.As<MemberType>();
//			}
//
////			if (!casted)
////				throw BadCastException(Details::Resolve<MemberType>()->GetName(), value.GetType()->GetName(), "value:");
//
//			if constexpr (std::is_member_function_pointer_v<decltype(Setter)>)
//				(obj->*Setter)(*casted);
//			else
//			{
//				static_assert(std::is_function_v<std::remove_pointer_t<decltype(Setter)>>);
//
//				Setter(*obj, *casted);
//			}
//        }
//
//        inline Any Get(Any object) override
//        {
//            Class* obj = object.As<Class>();
//
////			if (!obj)
////				throw BadCastException(Details::Resolve<Class>()->GetName(), object.GetType()->GetName());
//
//			if constexpr (std::is_member_function_pointer_v<decltype(Setter)>)
//				return Any((obj->*Getter)());
//			else
//			{
//				static_assert(std::is_function_v<std::remove_pointer_t<decltype(Getter)>>);
//				return Any(Getter(*obj));
//			}
//        }
//    };
//
//
//	class Conversion
//	{
//	public:
//		const TypeInfo* GetFromType() const { return m_From; }
//		const TypeInfo* GetToType() const { return m_To; }
//
//		virtual Any Convert(const void *object) const = 0;
//
//	protected:
//		Conversion(const TypeInfo* from, const TypeInfo* to)
//			: m_From(from), m_To(to) {}
//
//	private:
//		const TypeInfo *m_From;  // type to convert from
//		const TypeInfo *m_To;    // type to convert to
//	};
//
//	template <typename From, typename To>
//	class ConversionImpl : public Conversion
//	{
//	public:
//		ConversionImpl() : Conversion(GetType<From>(), GetType<To>()) {}
//
//		Any Convert(const void *object) const override
//		{
//			return static_cast<To>(*static_cast<const From*>(object));
//		}
//	};
//
//    template <typename T>
//    Any Any::TryConvert() const
//    {
//        Any converted;
//
//        if (!*this)
//            return converted;
//
//        if (TypeInfo const *typeDesc = GetType<T>(); typeDesc == type)
//            converted = *this;
//        else
//        {
//            for (auto* conversion : type->m_Conversions)
//                if (conversion->GetToType() == typeDesc) {
//                    converted = conversion->Convert(Data);
//                    break;
//                }
//        }
//        return converted;
//    }
//
//    template <typename From, typename To>
//	void TypeInfo::AddConversion()
//	{
//		Conversion *conversion = new ConversionImpl<From, To>;
//        m_Conversions.push_back(conversion);
//	}
//
//    template <typename To>
//    Conversion* TypeInfo::GetConversion() const
//    {
//        for (auto conversion : m_Conversions)
//            if (conversion->GetToType() == GetType<To>())
//                return conversion;
//        return nullptr;
//    }
//}
//
//#endif //FOX_LEXER_TYPE_HPP
