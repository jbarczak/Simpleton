
//=====================================================================================================================
//
//   DX11Window.h
//
//   Definition of class: Simpleton::DX11Window
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_WINDOW_H_
#define _DX11_WINDOW_H_

#include "Window.h"
#include "WindowController.h"
#include "Timer.h"
#include "ComPtr.h"

#include <d3d11.h>
#include <string>

namespace Simpleton
{
    class DX11WindowController;

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A class which encapsulates the ugly mechanics of the windowing system
    ///
    ///  This class encapsulates a DX11 rendering window
    ///
    //=====================================================================================================================
    class DX11Window : public Window
    {
    public:
                
        enum Flags
        {
            USE_DEBUG_LAYER=1,
            FPS_TITLE=2,
            USE_WARP=4,
            NO_RESIZE=8,
        };

        static DX11Window* Create( unsigned int nWidth, unsigned int nHeight, unsigned int flags, DX11WindowController* pController );

        /// Calls the controller's 'OnFrame' method to render a new frame
        void DoFrame( );

        /// Tell the window to take a screenshot after its next frame
        void RequestScreenshot( const char* pWhere );

        ID3D11Device* GetDevice() { return m_pDevice; }
        ID3D11DeviceContext* GetDeviceContext() { return m_pImmediateContext; }
        ID3D11RenderTargetView* GetBackbufferRTV() const { return m_pBackbuffer; };
        ID3D11UnorderedAccessView* GetBackbufferUAV() const { return m_pBackbufferUAV; };
        ID3D11DepthStencilView* GetBackbufferDSV() const { return m_pZBuffer; }

        D3D11_VIEWPORT BuildViewport();
        D3D11_RECT BuildScissorRect();

        UINT GetViewportWidth() const { return m_nClientWidth; }
        UINT GetViewportHeight() const { return m_nClientHeight; }

        double GetElapsedTime() const { return m_fElapsedTime; };           /// Elapsed time since start of prior frame
        double GetAccumulatedTime() const { return m_fAccumulatedTime; };   /// Elapsed time since window creation
        float GetAspectRatio() const { return (float)m_nClientWidth/(float)m_nClientHeight; };

        void SetTickInterval( double fTickIntervalInSeconds ) { m_fTickInterval = fTickIntervalInSeconds; }
        void SetTimeAccumulationPaused( bool bPause ) { m_bTimePaused = bPause; }
        bool IsTimeAccumulationPaused() const { return m_bTimePaused; }

    private:

        virtual bool WndProcHook( void* pHWND, unsigned int nMsg, intptr_t wParam, intptr_t lParam );
        
        void CreateBackBuffer();
  
        DX11Window( DX11WindowController* pController);

        ComPtr<ID3D11Device>        m_pDevice;
        ComPtr<ID3D11DeviceContext> m_pImmediateContext;
        ComPtr<IDXGISwapChain>      m_pSwapChain;
        ComPtr<ID3D11RenderTargetView> m_pBackbuffer;
        ComPtr<ID3D11UnorderedAccessView> m_pBackbufferUAV;
        ComPtr<ID3D11DepthStencilView> m_pZBuffer;
        ComPtr<ID3D11Texture2D> m_pBackbufferResource;
        ComPtr<ID3D11Texture2D> m_pZBufferResource;

        LARGE_INTEGER m_LastTime;
        double m_fTimerFreq;
        double m_fAccumulatedTime;
        double m_fElapsedTime;
        double m_fTickInterval;
        UINT   m_nFrameCount;
        UINT m_nClientWidth;
        UINT m_nClientHeight;
        UINT m_nWindowFlags;
        bool m_bTimePaused;

        LARGE_INTEGER m_FPSLastTime;
        std::string m_ScreenshotPath;
        
    };


    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief DX11 window controller
    ///
    ///
    //=====================================================================================================================
    class DX11WindowController : public WindowController
    {
    public:

        /// Implementation may perform one-time init
        ///  False return value will result in window destruction
        virtual bool OnCreate( DX11Window* pWindow ) = 0;

        /// Implementation should render a frame to the window's backbuffer
        ///   
        virtual void OnFrame( DX11Window* pWindow ) = 0;
    };
}

#endif