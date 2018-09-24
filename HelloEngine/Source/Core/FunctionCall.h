#pragma once

#include <functional>
#include <optional>

enum class FunctionCall_AssertLevel
{
	AssertInEditor,
	AssertInGame,
	NeverAssert
};

template< typename T >
struct OptionalReturnType
{
	using Type = std::optional< T >;
	static constexpr Type MakeDefault()
	{
		return {};
	}
};

template< typename T >
struct OptionalReturnType< std::optional< T > >
{
	using Type = std::optional< T >;
	static constexpr Type MakeDefault()
	{
		return {};
	}
};

template<>
struct OptionalReturnType< void >
{
	using Type = void;
	static constexpr void MakeDefault() {}
};

template< typename T, typename = decltype(&T::ValidateAsArgument) >
bool ValidateFunctionArgument(const T& arg)
{
	return arg.ValidateAsArgument();
}

template< typename T >
bool ValidateFunctionArgument(const T* arg)
{
	return arg != nullptr;
}

bool ValidateFunctionArgument(...)
{
	return true;
}

template< typename... Args >
bool ValidateAllFunctionArguments(Args... args)
{
	return (ValidateFunctionArgument(args) && ...);
}

template< typename Func, typename... Args >
auto FunctionCall(Func func, Args... args) -> typename OptionalReturnType< decltype(func(args...)) >::Type
{
	if (ValidateAllFunctionArguments(std::forward<Args...>(args)...))
	{
		return func(std::forward<Args...>(args)...);
	}

	return OptionalReturnType< decltype(func(args...)) >::MakeDefault();
}


template< typename ReturnType, typename Namespace, typename... Args >
using MemberFuncSig = ReturnType(Namespace::*)(Args...);

template< typename ReturnType, typename Namespace, typename... Args >
using MemberConstFuncSig = ReturnType(Namespace::*)(Args...) const;


template< typename ReturnType, typename FuncNamespace, typename... Args >
auto FunctionCall(FuncNamespace* object, MemberFuncSig<ReturnType, FuncNamespace, Args...> func, Args... args)
{
	return FunctionCall(std::bind(func, object, args...), std::forward<Args...>(args)...);
}

template< typename ReturnType, typename FuncNamespace, typename... Args >
auto FunctionCall(FuncNamespace* object, MemberConstFuncSig<ReturnType, FuncNamespace, Args...> func, Args... args)
{
	return FunctionCall(std::bind(func, object, args...), std::forward<Args...>(args)...);
}


template< typename ObjectType, typename ReturnType, typename FuncNamespace, typename... Args >
auto FunctionCall(ObjectType* object, MemberFuncSig<ReturnType, FuncNamespace, Args...> func, Args... args)
{
	static_assert(std::is_base_of_v<ObjectType, FuncNamespace>, "Can't call Functions from unrelated classes");
}
