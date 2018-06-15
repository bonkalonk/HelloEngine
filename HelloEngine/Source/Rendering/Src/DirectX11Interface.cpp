#pragma once

#include "HighestHeader.h"
#include "Utility.h"

#include "DirectX11Interface.h"

#include <assert.h>
#include <iostream>
#include <stdio.h>

#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")


D3d11Interface::D3d11Interface( HWND windowHandle )
{
	constexpr UINT screenWidth = 1920;
	constexpr UINT screenHeight = 1080;

	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ScopedPtr<ID3D11Texture2D> backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	ScopedPtr<IDXGIFactory> _factory;
	ScopedPtr<IDXGIAdapter> _adapter;
	ScopedPtr<IDXGIOutput> _adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	DXGI_MODE_DESC* displayModeList;

	auto Validator = []( const HRESULT &result ) { return SUCCEEDED( result ); };

	HRESULT GotRefreshRatesResult = CallWhile( Validator,
		[&]
	{
		return CreateDXGIFactory( __uuidof( IDXGIFactory ), _factory.GetRaw() );
	}, [&]
	{
		return _factory->EnumAdapters( 0, _adapter.GetTyped() );
	}, [&]
	{
		return _adapter->EnumOutputs( 0, _adapterOutput.GetTyped() );
	}, [&]
	{
		return _adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL );
	}, [&]
	{
		assert( numModes > 0 );
		displayModeList = new DXGI_MODE_DESC[ numModes ];
		assert( displayModeList != nullptr );

		return _adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList );
	}, [&]
	{
		numerator = displayModeList[ 0 ].RefreshRate.Numerator;
		denominator = displayModeList[ 0 ].RefreshRate.Denominator;

		return S_OK;
	} );

	delete[] displayModeList;
	displayModeList = nullptr;


	// Init Swap Chain Desc
	{
		ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
		swapChainDesc.BufferCount = 1;

		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;

		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		constexpr bool bVsyncEnabled = false;
		if ( bVsyncEnabled )
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = windowHandle;

		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		constexpr bool fullscreen = false;
		swapChainDesc.Windowed = fullscreen;

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Don't set the advanced flags.
		swapChainDesc.Flags = 0;

		featureLevel = D3D_FEATURE_LEVEL_11_0;

		// Initialize the description of the depth buffer.
		ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );

		// Set up the description of the depth buffer.
		depthBufferDesc.Width = screenWidth;
		depthBufferDesc.Height = screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
	}

	{
		CallWhile( Validator,
			[&]
		{
			return D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext );
		}, [&]
		{
			// Get the pointer to the back buffer.
			return m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), backBufferPtr.GetRaw() );
		}, [&]
		{
			// Create the render target view with the back buffer pointer.
			return m_device->CreateRenderTargetView( backBufferPtr.Get(), NULL, &m_renderTargetView );
		}, [&]
		{
			// Create the texture for the depth buffer using the filled out description.
			return m_device->CreateTexture2D( &depthBufferDesc, NULL, &m_depthStencilBuffer );
		} );
	}
}