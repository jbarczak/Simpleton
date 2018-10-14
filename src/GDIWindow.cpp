
//=====================================================================================================================
//
//   GDIWindow.cpp
//
//   Implementation of class: Simpleton::GDIWindow
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================
#include "GDIWindow.h"
#include "WindowPainter.h"
#include <Windows.h>

namespace Simpleton
{
    
    class Win32WindowPainter : public WindowPainter
    {
    public:
        
        inline Win32WindowPainter( HDC dc, RECT paintRect, RECT clientRect ) : m_hdc(dc), m_clientRect(clientRect), m_paintRect(paintRect)
        {
        };

        void GetClientSize( unsigned int* pWidth, unsigned int* pHeight )
        {
            *pWidth  = (UINT)(m_clientRect.right - m_clientRect.left);
            *pHeight = (UINT)(m_clientRect.bottom - m_clientRect.top);
        }

        void GetPaintRect( int* pTop, int* pLeft, int *pWidth, int* pHeight )
        {
            *pTop    = m_paintRect.top;
            *pLeft   = m_paintRect.left;
            *pWidth  = m_paintRect.right - m_paintRect.left;
            *pHeight = m_paintRect.bottom - m_paintRect.top;
        }

        void BlitBGRAPixels( const BlitParameters& rBlit )
        {
            BITMAPINFO bm;
            memset( &bm, 0, sizeof(BITMAPINFO));
            bm.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
            bm.bmiHeader.biWidth       = rBlit.nSrcImageWidth;
            bm.bmiHeader.biHeight      = -rBlit.nSrcImageHeight; // negative height implies a top-down image
            bm.bmiHeader.biPlanes      = 1;
            bm.bmiHeader.biBitCount    = 32;
            bm.bmiHeader.biCompression = BI_RGB;
            bm.bmiHeader.biSizeImage   = 0;
            bm.bmiHeader.biClrUsed     = 256;


            // StretchDIBits is a little bit insane about the vertical orientation.  See:
            // http://wiki.allegro.cc/index.php?title=StretchDIBits
            StretchDIBits( m_hdc, 
                   rBlit.nLeft, rBlit.nHeight+rBlit.nTop-1, rBlit.nWidth, -rBlit.nHeight,
                   rBlit.nSrcLeft, rBlit.nSrcImageHeight-rBlit.nSrcTop+1, rBlit.nSrcWidth, -rBlit.nSrcHeight,
                   rBlit.pBits, 
                   &bm, DIB_RGB_COLORS, SRCCOPY );
        }

    private:
        HDC m_hdc;
        RECT m_paintRect;
        RECT m_clientRect;
    };



    GDIWindow* GDIWindow::Create( unsigned int nWidth, unsigned int nHeight,  GDIWindowController* pController )
    {
        GDIWindow* pWin = new GDIWindow( pController );
        if( pWin->FinishCreate( nWidth, nHeight, 0 ) )
            return pWin;
        delete pWin;
        return 0;
    }

    GDIWindow::GDIWindow( GDIWindowController* pController ) : Window(pController) 
    {
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void GDIWindow::RepaintAll()
    {
        InvalidateRect( (HWND) GetHandle(), NULL, false );
    }

    
    bool GDIWindow::WndProcHook(void* pHWnd, unsigned int msg, intptr_t wParam, intptr_t lParam )
    {
        HWND hWnd = (HWND)pHWnd;
        if( msg == WM_PAINT )
        {
            PAINTSTRUCT ps;
            RECT clientRect;
                
            if( GetUpdateRect( hWnd, NULL, false ) )
            {
                HDC dc = BeginPaint( hWnd, &ps );
                GetClientRect( hWnd, &clientRect );
                
                Win32WindowPainter painter( dc, ps.rcPaint, clientRect );

                GDIWindowController* pController = static_cast<GDIWindowController*>( GetController() );
                pController->OnPaint( this, &painter );

                EndPaint( hWnd, &ps );
            }

            DefWindowProc( hWnd, msg, wParam, lParam );
            return true;
        }

        return false;
    }
}