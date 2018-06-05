#pragma once

#include "HighestHeader.h"

#include "d3d11.h"

#include <stdio.h>
#include <iostream>

struct SequentialCaller
{
	//using VerifyFunc = bool( void );

	SequentialCaller() = default;

	template < typename Func, typename VerifyFunc >
	SequentialCaller( Func func, VerifyFunc verify )
	{
		( *this )( func, verify );
	}


	template < typename Func, typename VerifyFunc >
	void operator() ( Func func, VerifyFunc verify )
	{
		if ( bIsValid )
		{
			func();

			bIsValid = verify();
		}
	}

	bool operator() () const
	{
		return bIsValid;
	}

protected:

	bool bIsValid = true;
};


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


D3d11Interface::D3d11Interface()
{
	//constexpr bool bUsingTheBadWay = true;
	//
	//if ( bUsingTheBadWay == true )
	//{
	//	HRESULT result;
	//	IDXGIFactory* dxgiFactory = nullptr;
	//	IDXGIAdapter* dxgiAdapter = nullptr;
	//	IDXGIOutput* dxgiOutput = nullptr;
	//	uint numModes, i, numerator, denominator, stringLength;
	//	DXGI_MODE_DESC* displayModeList = nullptr;
	//	DXGI_ADAPTER_DESC adapterDesc;
	//	int error;
	//	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	//	D3D_FEATURE_LEVEL featureLevel;
	//	//ID3D11Texture2D* backBufferPtr = nullptr;
	//	//D3D11_TEXTURE2D_DESC depthBufferDesc;
	//	//D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//	//D3D11_RASTERIZER_DESC rasterDesc;
	//	//D3D11_VIEWPORT viewport;
	//	float fieldOfView, screenAspect;
	//	
	//	result = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**)&dxgiFactory );
	//
	//	if ( FAILED( result ) )
	//	{
	//		return;
	//	}
	//
	//	result = dxgiFactory->EnumAdapters( 0, &dxgiAdapter );
	//
	//	if ( FAILED( result ) )
	//	{
	//		return;
	//	}
	//
	//	result = dxgiAdapter->EnumOutputs( 0, &dxgiOutput );
	//
	//	if ( FAILED( result ) )
	//	{
	//		return;
	//	}
	//
	//	result = dxgiOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr );
	//
	//	if ( FAILED( result ) || ( numModes == 0 ) )
	//	{
	//		return;
	//	}
	//
	//	displayModeList = new DXGI_MODE_DESC[ numModes ];
	//
	//	if ( displayModeList == nullptr )
	//	{
	//		return;
	//	}
	//
	//	// wtf support is not easy =]
	//
	//	/// And so on...
	//}
	//
	//
	//if ( bUsingTheBadWay == false )
	//{
	//	/*HRESULT result;
	//	auto CheckThatFunctionWorked = [&result]
	//	{
	//		return FAILED( result ) == false;
	//	};
	//
	//
	//	IDXGIFactory* dxgiFactory = nullptr;
	//	IDXGIAdapter* dxgiAdapter = nullptr;
	//
	//	auto CreateFactory = [&result] -> union { HRESULT, IDXGIFactory* };
	//	{
	//		result = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**)&dxgiFactory );
	//	};
	//
	//	auto EnumerateAdapters = [&result, &dxgiFactory, &dxgiAdapter]
	//	{
	//		result = dxgiFactory->EnumAdapters( 0, &dxgiAdapter );
	//	};
	//
	//	SequentialCaller andThen;
	//
	//
	//	andThen( CreateFactory, CheckThatFunctionWorked );
	//	andThen( EnumerateAdapters, CheckThatFunctionWorked );
	//	andThen( EnumerateAdapters, []()
	//	{
	//		return false;
	//	} );*/
	//}
}