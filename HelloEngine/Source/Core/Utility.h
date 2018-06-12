#pragma once

template < typename ContainerType, typename Func >
void ForEach( ContainerType& vector, Func func )
{
	for ( auto& It : vector )
	{
		func( It );
	}
}

// Funcs with args attempt...
/*
template < typename Func, typename Arg >
void CallWhile( Func func, Arg arg )
{
	func( arg );
}

template < typename FirstFunc, typename FirstArg, typename... Funcs >
void CallWhile( FirstFunc firstFunc, Funcs... additionalFuncs, FirstArg firstArg )
{
	if ( auto result = firstFunc( firstArg ) )
	{
		CallWhile( result, additionalFuncs... );
	}
}

template < typename Func, typename... Funcs >
void CallWhile( Func firstFunc, Funcs... additionalFuncs )
{
	if ( auto result = firstFunc() )
	{
		CallWhile( additionalFuncs..., result );
	}
}*/

template < typename VALIDATOR, typename FUNC >
auto CallWhile( VALIDATOR validator, FUNC func )
{
	return func();
}

template < typename VALIDATOR, typename FUNC, typename... FUNCS >
auto CallWhile( VALIDATOR validator, FUNC firstFunc, FUNCS... additionalFuncs )
{
	auto result = firstFunc();

	if ( validator( result ) )
	{
		return CallWhile( validator, additionalFuncs... );
	}

	return result;
}