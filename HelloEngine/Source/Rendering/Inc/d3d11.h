#pragma once

#include <d3d11.h>
#include <d3dcommon.h>
#include <dxgi.h>
//#include <d3dxd10math.h>

struct D3d11Interface
{
	D3d11Interface();

protected:

	template < typename T >
	struct ScopedPtr
	{
		ScopedPtr( T* ptr ) : _ptr( ptr ) {}
		~ScopedPtr()
		{
			if ( _ptr )
			{
				_ptr->Release();
				delete _ptr;
			}
		}

		void** GetRaw()
		{
			return (void**)&_ptr;
		}

		T* operator -> () ( ) const
		{
			return _ptr;
		}

	protected:
		T* _ptr = nullptr;
	};

	ScopedPtr<IDXGIFactory> _factory;
	ScopedPtr<IDXGIAdapter> _adapter;
};