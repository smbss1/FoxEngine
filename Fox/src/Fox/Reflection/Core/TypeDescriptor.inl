#ifndef TYPE_DESCRIPTOR_INL
#define TYPE_DESCRIPTOR_INL

#include "TypeDescriptor.hpp"
#include "DataMember.hpp"
#include "Function.hpp"
#include "Constructor.hpp"
#include "Base.hpp"
#include "Conversion.hpp"

namespace fox::Reflect
{
    template <typename Type, typename... Args>
	void TypeDescriptor::AddConstructor()
	{
		Constructor *constructor = new ConstructorImpl<Type, Args...>();

		mConstructors.push_back(std::unique_ptr<Constructor>(constructor));
	}

	template <typename Type, typename... Args>
	void TypeDescriptor::AddConstructor(Type(*ctorFun)(Args...))
	{
		Constructor *constructor = new FreeFunConstructor<Type, Args...>(ctorFun);

		mConstructors.push_back(std::unique_ptr<Constructor>(constructor));
	}

	template <typename B, typename T>
	void TypeDescriptor::AddBase()
	{
		Base *base = new BaseImpl<B, T>;

		mBases.push_back(std::unique_ptr<Base>(base));
	}

	template <typename C, typename T>
	void TypeDescriptor::AddDataMember(T C::*dataMemPtr, const std::string &name)
	{
		DataMember *dataMember = new PtrDataMember<C, T>(dataMemPtr, name);

		mDataMembers.push_back(std::unique_ptr<DataMember>(dataMember));
	}

	template <auto Setter, auto Getter, typename Type>
	void TypeDescriptor::AddDataMember(const std::string &name)
	{
		DataMember *dataMember = new SetGetDataMember<Setter, Getter, Type>(name);

		mDataMembers.push_back(std::unique_ptr<DataMember>(dataMember));
	}

	template <typename Ret, typename... Args>
	void TypeDescriptor::AddMemberFunction(Ret freeFun(Args...), const std::string &name)
	{
		Function *memberFunction = new FreeFunction<Ret, Args...>(freeFun, name);

		mMemberFunctions.push_back(std::unique_ptr<Function>(memberFunction));
	}

	template <typename C, typename Ret, typename... Args>
	void TypeDescriptor::AddMemberFunction(Ret(C::*memFun)(Args...), const std::string &name)
	{
		Function *memberFunction = new MemberFunction<C, Ret, Args...>(memFun, name);

		mMemberFunctions.push_back(std::unique_ptr<Function>(memberFunction));
	}

	template <typename C, typename Ret, typename... Args>
	void TypeDescriptor::AddMemberFunction(Ret(C::*memFun)(Args...) const, const std::string &name)
	{
		Function *memberFunction = new ConstMemberFunction<C, Ret, Args...>(memFun, name);

		mMemberFunctions.push_back(std::unique_ptr<Function>(memberFunction));
	}

	template <typename From, typename To>
	void TypeDescriptor::AddConversion()
	{
		Conversion *conversion = new ConversionImpl<From, To>;

		mConversions.push_back(std::unique_ptr<Conversion>(conversion));
	}

	template<typename T>
	Enumeration* TypeDescriptor::AddEnumeration(const std::string &name)
	{
		auto enumeration = std::make_unique<Enumeration>();
		enumeration->Setup<T>();
		auto* e = enumeration.get();
		m_Enums.push_back(std::move(enumeration));
		return e;
	}

	inline std::string const &TypeDescriptor::GetName() const
	{
		return mName;
	}

	inline std::size_t TypeDescriptor::GetSize() const
	{
		return mSize;
	}

	inline const std::vector<std::unique_ptr<Constructor>>& TypeDescriptor::GetConstructors() const
	{
		return mConstructors;
	}

	template <typename... Args>
	const Constructor *TypeDescriptor::GetConstructor() const
	{
		for (const auto& constructor : mConstructors)
			if (constructor->CanConstruct<Args...>(std::index_sequence_for<Args...>()))
				//if (constructor->CanConstruct<Args...>(std::make_index_sequence<sizeof...(Args)>()))
				return constructor.get();

		return nullptr;
	}

	inline const std::vector<std::unique_ptr<Base>>& TypeDescriptor::GetBases() const
	{
		return mBases;
	}

	template <typename B>
	Base *TypeDescriptor::GetBase() const
	{
		for (const auto& base : mBases)
			if (base->GetType() == Details::Resolve<B>)
				return base.get();

		return nullptr;
	}

	inline std::vector<DataMember*> TypeDescriptor::GetDataMembers() const
	{
		std::vector<DataMember*> dataMembers;
        for (const auto& data : mDataMembers)
            dataMembers.push_back(data.get());

		for (const auto& base : mBases)
			for (auto dataMember : base->GetType()->GetDataMembers())
				dataMembers.push_back(dataMember);

		return dataMembers;
	}

	inline DataMember *TypeDescriptor::GetDataMember(const std::string &name) const
	{
		for (const auto& dataMember : mDataMembers)
			if (dataMember->GetName() == name)
				return dataMember.get();

		for (const auto& base : mBases)
			if (auto *baseDataMember = base->GetType()->GetDataMember(name))
				return baseDataMember;

		return nullptr;
	}

	inline std::vector<Function*> TypeDescriptor::GetMemberFunctions() const
	{
		std::vector<Function*> memberFunctions;

        for (const auto& data : mMemberFunctions)
            memberFunctions.push_back(data.get());

		for (const auto& base : mBases)
			for (auto memberFunction : base->GetType()->GetMemberFunctions())
				memberFunctions.push_back(memberFunction);

		return memberFunctions;
	}

	inline const Function *TypeDescriptor::GetMemberFunction(const std::string &name) const
	{
		for (const auto& memberFunction : mMemberFunctions)
			if (memberFunction->GetName() == name)
				return memberFunction.get();

		for (const auto& base : mBases)
			if (auto *memberFunction = base->GetType()->GetMemberFunction(name))
				return memberFunction;

		return nullptr;
	}

	inline const std::vector<std::unique_ptr<Conversion>>& TypeDescriptor::GetConversions() const
	{
		return mConversions;
	}

	template <typename To>
	Conversion *TypeDescriptor::GetConversion() const
	{
		for (const auto& conversion : mConversions)
			if (conversion->GetToType() == Details::Resolve<To>())
				return conversion.get();

		return nullptr;
	}
}  // namespace Reflect

#endif  // TYPE_DESCRIPTOR_INL
