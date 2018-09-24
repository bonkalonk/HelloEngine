#pragma once

#include <assert.h>
#include <type_traits>

template< typename T >
struct NotNull
{
	template< typename U >
	NotNull(U* value) : _value(value)
	{
		static_assert( std::is_convertible_v<U, T>, "Cannot instantiate NotNull from incompatible type, no conversion available" );
		AssertIsValid();
	}

	template< typename U >
	NotNull(const NotNull<U>& other) : _value(*other)
	{
		static_assert(std::is_convertible_v<U, T>, "Cannot instantiate NotNull from incompatible type, no conversion available");
		AssertIsValid();
	}

	NotNull(std::nullptr_t) = delete;

	
	T* operator ->() const
	{
		AssertIsValid();
		return _value;
	}

	operator T*() const
	{
		AssertIsValid();
		return _value;
	}

	T* operator *() const
	{
		AssertIsValid();
		return _value;
	}

	bool ValidateAsArgument() const
	{
		return _value != nullptr;
	}

protected:
	void AssertIsValid() const
	{
		assert( _value != nullptr );
	}
	
	T* _value;
};
