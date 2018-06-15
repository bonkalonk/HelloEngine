#pragma once

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>


struct D3d11Interface
{
	D3d11Interface( HWND windowHandle );

protected:

	template < typename T >
	struct ScopedPtr
	{
		ScopedPtr() = default;
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

		T** GetTyped()
		{
			return &_ptr;
		}

		T* Get()
		{
			return _ptr;
		}

		T* operator -> () const
		{
			return _ptr;
		}

	protected:
		T* _ptr = nullptr;
	};

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	//XMMATRIX m_projectionMatrix;
	//XMMATRIX m_worldMatrix;
	//XMMATRIX m_orthoMatrix;
};