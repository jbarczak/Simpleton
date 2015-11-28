
//=====================================================================================================================
//
//   DX11Window.cpp
//
//   Implementation of class: Simpleton::DX11Window
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "DX11/DX11Window.h"
#include "PPMImage.h"
#include <stdio.h>

namespace Simpleton
{

    

    //=====================================================================================================================
    //=====================================================================================================================
    DX11Window* DX11Window::Create( unsigned int nWidth, unsigned int nHeight, unsigned int Flags, DX11WindowController* pController )
    {
        DX11Window* pWin = new DX11Window(pController);
        if( !pWin->FinishCreate(nWidth,nHeight) )
        {
            delete pWin;
            return 0;
        }

        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };

        UINT nLevels = sizeof(levels)/sizeof(D3D_FEATURE_LEVEL);

        D3D_DRIVER_TYPE eDriverType = D3D_DRIVER_TYPE_HARDWARE;
        if( Flags & USE_WARP )
            eDriverType = D3D_DRIVER_TYPE_WARP;

        HWND hWnd = (HWND) pWin->GetHandle();

        IDXGISwapChain* pSwapChain=0;
        ID3D11Device* pDevice=0;
        ID3D11DeviceContext* pImmediateContext=0;          
        ID3D11Texture2D* pBackBuffer=0;
        ID3D11RenderTargetView* pBackBufferRTV=0;
        ID3D11DepthStencilView* pBackBufferDSV=0;
        ID3D11Texture2D* pZBuffer=0;
        
        RECT rect;
        GetClientRect(hWnd,&rect);
        nWidth = rect.right-rect.left;
        nHeight = rect.bottom-rect.top;

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferCount = 2;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.Width  = nWidth;
        sd.BufferDesc.Height = nHeight;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.Flags = 0;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Windowed = TRUE;
        
        D3D11_RENDER_TARGET_VIEW_DESC rtv;
        rtv.Format             = sd.BufferDesc.Format;
        rtv.Texture2D.MipSlice = 0;
        rtv.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;

        D3D11_TEXTURE2D_DESC zbuffer;
        zbuffer.ArraySize = 1;
        zbuffer.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        zbuffer.CPUAccessFlags = 0;
        zbuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        zbuffer.Height = sd.BufferDesc.Height;
        zbuffer.Width = sd.BufferDesc.Width;
        zbuffer.MipLevels = 1;
        zbuffer.MiscFlags = 0;
        zbuffer.Usage = D3D11_USAGE_DEFAULT;
        zbuffer.SampleDesc = sd.SampleDesc;
        
        D3D11_DEPTH_STENCIL_VIEW_DESC dsv;
        dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsv.Flags = 0;
        dsv.Texture2D.MipSlice = 0;

        D3D_FEATURE_LEVEL eActualFeatureLevel;
        DWORD DeviceFlags = 0;
        if( Flags & USE_DEBUG_LAYER )
            DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

        HRESULT hr = D3D11CreateDeviceAndSwapChain( 0, eDriverType, 0, DeviceFlags, 
                                       levels, nLevels, 
                                       D3D11_SDK_VERSION, 
                                       &sd, &pSwapChain, &pDevice, &eActualFeatureLevel, &pImmediateContext );
        if( FAILED(hr) ) // no 11.1, fallback to 11
            D3D11CreateDeviceAndSwapChain( 0, eDriverType, 0, DeviceFlags, 
                                       levels+1, nLevels-1, 
                                       D3D11_SDK_VERSION, 
                                       &sd, &pSwapChain, &pDevice, &eActualFeatureLevel, &pImmediateContext );
        if( FAILED(hr) )
            goto fail;

        hr = pSwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D*), (void**)&pBackBuffer );
        if( FAILED(hr) )
            goto fail;

        hr = pDevice->CreateRenderTargetView( pBackBuffer, &rtv, &pBackBufferRTV );
        if( FAILED(hr) )
            goto fail;

        hr = pDevice->CreateTexture2D( &zbuffer, 0, &pZBuffer );
        if( FAILED(hr) )
            goto fail;

        hr = pDevice->CreateDepthStencilView( pZBuffer,&dsv,&pBackBufferDSV );
        if( FAILED(hr) )
            goto fail;

        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&pWin->m_LastTime);
        QueryPerformanceCounter(&pWin->m_FPSLastTime );
        pWin->m_fAccumulatedTime=0;
        pWin->m_nFrameCount=0;
        pWin->m_fTimerFreq = freq.QuadPart;
        pWin->m_nWindowFlags = Flags;
       
        pWin->m_pDevice.Owns(pDevice);
        pWin->m_pImmediateContext.Owns( pImmediateContext );
        pWin->m_pSwapChain.Owns( pSwapChain );
        pWin->m_pBackbufferResource.Owns( pBackBuffer );
        pWin->m_pZBufferResource.Owns( pZBuffer );
        pWin->m_pBackbuffer.Owns(pBackBufferRTV);
        pWin->m_pZBuffer.Owns(pBackBufferDSV);
        pWin->m_nClientWidth  = nWidth;
        pWin->m_nClientHeight = nHeight;

        pDevice=0;
        pImmediateContext=0;
        pSwapChain=0;
        pBackBuffer=0;
        pZBuffer=0;
        pBackBufferRTV=0;
        pBackBufferDSV=0;

        timeBeginPeriod(1);

        if( !pController->OnCreate( pWin ) )
            goto fail;

        return pWin;

    fail:
        pWin->Destroy();
        SAFE_RELEASE(pBackBuffer);
        SAFE_RELEASE(pZBuffer);
        SAFE_RELEASE(pBackBufferRTV);
        SAFE_RELEASE(pBackBufferDSV);
        SAFE_RELEASE(pSwapChain);
        SAFE_RELEASE(pImmediateContext);
        SAFE_RELEASE(pDevice);
        return 0;
    }

    //=====================================================================================================================
    //=====================================================================================================================       
    void DX11Window::RequestScreenshot( const char* pLocation )
    {
        m_ScreenshotPath = pLocation;
    }

    //=====================================================================================================================
    //=====================================================================================================================    
    void DX11Window::DoFrame()
    {

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        m_fElapsedTime = (now.QuadPart - m_LastTime.QuadPart) / m_fTimerFreq;
        
        if( m_fElapsedTime < m_fTickInterval )
        {
            double leftover = (m_fTickInterval - m_fElapsedTime)*0.5;
            Sleep( leftover*1000 );
            return;
        }

        RECT rect;
        GetClientRect( (HWND) this->GetHandle(), &rect );
        m_nClientWidth  = rect.right - rect.left;
        m_nClientHeight = rect.bottom - rect.top;

        m_LastTime = now;
        
        m_fAccumulatedTime += m_fElapsedTime;

        DX11WindowController* pController = static_cast<DX11WindowController*>(GetController());
        pController->OnFrame( this );

        m_pSwapChain->Present( 0, 0 );
        m_nFrameCount++;

        if( m_ScreenshotPath.size() > 0 )
        {
            ID3D11Texture2D* pBB = m_pBackbufferResource;
            D3D11_TEXTURE2D_DESC desc;
            pBB->GetDesc(&desc);

            ID3D11Texture2D* pScratch=0;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.BindFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            m_pDevice->CreateTexture2D( &desc, 0, &pScratch );

            m_pImmediateContext->CopyResource( pScratch, pBB );

            D3D11_MAPPED_SUBRESOURCE mapped;
            m_pImmediateContext->Map( pScratch, 0, D3D11_MAP_READ, 0, &mapped );

            PPMImage img(desc.Width,desc.Height);
            for( UINT y=0; y<desc.Height; y++ )
            {
                for( UINT x=0; x<desc.Width; x++ )
                {
                    BYTE* pSrc = ((BYTE*)mapped.pData) + y*mapped.RowPitch + 4*x;
                    img.SetPixel(x,y,pSrc[0]/255.0f, pSrc[1]/255.0f, pSrc[2]/255.0f );
                }
            }

            img.SaveFile( m_ScreenshotPath.c_str() );
            m_ScreenshotPath = "";
            pScratch->Release();
        }

        // FPS in title every few frames, if asked
        enum{ FPS_INTERVAL=32};
        if( (m_nWindowFlags & FPS_TITLE) && !(m_nFrameCount % FPS_INTERVAL) )
        {
            QueryPerformanceCounter(&now);
            double elapsed = (now.QuadPart - m_FPSLastTime.QuadPart) /m_fTimerFreq;
            m_FPSLastTime = now;
            double fps = FPS_INTERVAL / elapsed;
            char tmp[256];
            sprintf( tmp, "FPS: %.2f  ms: %.2f\n", fps, 1000/fps );
            SetWindowTextA( (HWND)this->GetHandle(), tmp );
        }

    }

    //=====================================================================================================================
    //=====================================================================================================================    
    D3D11_VIEWPORT DX11Window::BuildViewport( )
    {
        D3D11_VIEWPORT vp;
        vp.TopLeftX=0;
        vp.TopLeftY=0;
        vp.MinDepth = 0;
        vp.MaxDepth = 1;
        vp.Height = m_nClientHeight;
        vp.Width = m_nClientWidth;
        return vp;
    }
    //=====================================================================================================================
    //=====================================================================================================================        
    D3D11_RECT DX11Window::BuildScissorRect()
    {
        D3D11_RECT rect;
        GetClientRect( (HWND)GetHandle(),&rect );
        return rect;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    DX11Window::DX11Window( DX11WindowController* pController ) : Window(pController), m_fTickInterval(0)
    {

    }

    //=====================================================================================================================
    //=====================================================================================================================
    bool DX11Window::WndProcHook( void* pHWND, unsigned int nMsg, intptr_t wParam, intptr_t lParam )
    {
        if( nMsg == WM_SIZE )
        {
            UINT nWidth = LOWORD(lParam);
            UINT nHeight = HIWORD(lParam);
          
            m_pBackbuffer=0;
            m_pBackbufferResource=0;
            m_pZBuffer=0;
            m_pZBufferResource=0;

            m_pSwapChain->ResizeBuffers( 2, nWidth, nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );
            m_nClientWidth = nWidth;
            m_nClientHeight = nHeight;

            DXGI_SWAP_CHAIN_DESC sd;
            m_pSwapChain->GetDesc(&sd);
            sd.BufferDesc.Width = nWidth;
            sd.BufferDesc.Height = nHeight; // because it doesn't take effect right away...

            D3D11_RENDER_TARGET_VIEW_DESC rtv;
            rtv.Format             = sd.BufferDesc.Format;
            rtv.Texture2D.MipSlice = 0;
            rtv.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;

            D3D11_TEXTURE2D_DESC zbuffer;
            zbuffer.ArraySize = 1;
            zbuffer.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            zbuffer.CPUAccessFlags = 0;
            zbuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            zbuffer.Height = sd.BufferDesc.Height;
            zbuffer.Width = sd.BufferDesc.Width;
            zbuffer.MipLevels = 1;
            zbuffer.MiscFlags = 0;
            zbuffer.Usage = D3D11_USAGE_DEFAULT;
            zbuffer.SampleDesc = sd.SampleDesc;
        
            D3D11_DEPTH_STENCIL_VIEW_DESC dsv;
            dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            dsv.Flags = 0;
            dsv.Texture2D.MipSlice = 0;

            ID3D11Texture2D* pBackBuffer;
            ID3D11RenderTargetView* pBackBufferRTV;
            ID3D11Texture2D* pZBuffer;
            ID3D11DepthStencilView* pZBufferDSV;
            m_pSwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D*), (void**)&pBackBuffer );
            m_pDevice->CreateRenderTargetView( pBackBuffer, &rtv, &pBackBufferRTV );
            m_pDevice->CreateTexture2D( &zbuffer, 0, &pZBuffer );
            m_pDevice->CreateDepthStencilView( pZBuffer,&dsv,&pZBufferDSV );

            m_pBackbufferResource.Owns(pBackBuffer);
            m_pZBufferResource.Owns(pZBuffer);
            m_pBackbuffer.Owns(pBackBufferRTV);
            m_pZBuffer.Owns(pZBufferDSV);

            WindowController* pController = GetController();
            pController->OnSized( this, m_nClientWidth, m_nClientHeight );

            return true;
        }
        else if( nMsg == WM_EXITSIZEMOVE )
        {
            QueryPerformanceCounter(&m_LastTime); // ignore time spent in size/move loop
            return false;
        }
        
        return false;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11Window::CreateBackBuffer()
    {
    }
}