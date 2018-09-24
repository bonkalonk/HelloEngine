#pragma once


template< typename ContainerType, typename MatchFunc, typename Func >
void ForEachMatching(ContainerType& container, MatchFunc Match, Func Func)
{
	for (auto& It : container)
	{
		if (Match(It))
		{
			Func(It);
		}
	}
}

template < typename ContainerType, typename Func >
void ForEach(ContainerType& container, Func Func)
{
	ForEachMatching(container, [] { return true; } Func);
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
auto CallWhile(VALIDATOR validator, FUNC func)
{
	return func();
}

template < typename VALIDATOR, typename FUNC, typename... FUNCS >
auto CallWhile(VALIDATOR validator, FUNC firstFunc, FUNCS... additionalFuncs)
{
	auto result = firstFunc();

	if (validator(result))
	{
		return CallWhile(validator, additionalFuncs...);
	}

	return result;
}

template < typename T >
struct ScopedArrayDestructor
{
	constexpr static void Destroy(T*& arrayPointer)
	{
		if (arrayPointer)
		{
			delete[] arrayPointer;
			araryPointer = nullptr;
		}
	}
};

template < typename T  >
struct ReleaseAndDeleteObjectDestructor
{
	constexpr static void Destroy(T*& objectPointer)
	{
		if (objectPointer)
		{
			objectPointer->Release();
			delete objectPointer;
			objectPointer = nullptr;
		}
	}
};

template < typename T, typename DestroyFunction >
struct ScopedPointer
{
	ScopedPointer() = default;
	ScopedPointer(T* ptr) : _ptr(ptr) {}
	~ScopedPointer()
	{
		DestroyFunction::Destroy(_ptr);
	}

	void** GetRawPtr()
	{
		return (void**)&_ptr;
	}

	T** GetTypedPtr()
	{
		return &_ptr;
	}

	T* Get() const
	{
		return _ptr;
	}

	T* operator -> () const
	{
		return Get();
	}

protected:
	T * _ptr = nullptr;
};
