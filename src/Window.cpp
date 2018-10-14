//=====================================================================================================================
//
//   Window.cpp
//
//   Definition of class: Simpleton::Window
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

  
#include "Window.h"


#include "WindowController.h"
#include "WindowPainter.h"
#include <windows.h>

namespace Simpleton
{

    static POINT ConvertPoint( LPARAM lparam, HWND hwnd )
    {
        POINT pt;
        pt.x = LOWORD(lparam);
        pt.y = HIWORD(lparam);
        ScreenToClient(hwnd,&pt);
        return pt;
    }

    class WindowFriends
    {
    public:

        static LRESULT __stdcall BaseWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {    
            // find the window instance based on the window handle
            Window* pWin = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd, GWLP_USERDATA ));
       
            if( !pWin )
               return DefWindowProc( hWnd, message, wParam, lParam );

            // get the window's controller
            WindowController* pController = pWin->GetController();
            if( !pController )
                return DefWindowProc( hWnd, message, wParam, lParam );

            // do any subclass-specific processing
            if( pWin->WndProcHook(hWnd,message,wParam,lParam) )
                return 0;

            // process the message.....
            if( message == WM_KEYDOWN || message == WM_KEYUP )
            {
                if( pController )
                {
                    WindowController::KeyCode eKey=WindowController::KEY_INVALID;
                    switch( wParam )
                    {
                    case VK_SNAPSHOT: eKey = WindowController::KEY_PRINTSCREEN; break;
                    case VK_RETURN: eKey = WindowController::KEY_ENTER; break;
                    case VK_SPACE:  eKey = WindowController::KEY_SPACE; break;
                    case VK_ESCAPE: eKey = WindowController::KEY_ESCAPE; break;
                    default:
                        if( wParam >= 'A' && wParam <= 'Z' )
                            eKey = static_cast<WindowController::KeyCode>( WindowController::KEY_A + (wParam - 'A') );
                        if( wParam >= '0' && wParam <= '9' )
                            eKey = static_cast<WindowController::KeyCode>( WindowController::KEY_0 + (wParam - '0') );
                            
                    }

                    if( eKey != WindowController::KEY_INVALID )
                    {
                        if( message == WM_KEYDOWN )
                            pController->OnKeyDown( pWin, eKey );
                        if( message == WM_KEYUP )
                            pController->OnKeyUp( pWin, eKey );
                    }
                }
            }
            else if( message == WM_POINTERDOWN )
            {
                DWORD id = GET_POINTERID_WPARAM(wParam);
                POINT pt = ConvertPoint(lParam,hWnd);
                pController->OnTouchDown(pWin,id,pt.x, pt.y);
                return 0;
            }
            else if( message == WM_POINTERUP )
            {
                DWORD id = GET_POINTERID_WPARAM(wParam);
                POINT pt = ConvertPoint(lParam,hWnd);
                pController->OnTouchUp(pWin,id,pt.x,pt.y);
                return 0;
            }
            else if( message == WM_POINTERCAPTURECHANGED )
            {
                DWORD id = GET_POINTERID_WPARAM(wParam);
                POINT pt = ConvertPoint(lParam,hWnd);
                pController->OnTouchUp(pWin,id,pt.x,pt.y);
                return 0;
            }
            else if( message == WM_POINTERUPDATE )
            {
                DWORD id = GET_POINTERID_WPARAM(wParam);
                POINT pt = ConvertPoint(lParam,hWnd);
                pController->OnTouchMove(pWin,id,pt.x,pt.y);
                return 0;
            }
            else if( message == WM_LBUTTONDOWN )
            {
                SetCapture( hWnd ); // make sure that mouse events get captured even if dragging outside window
                pController->OnKeyDown( pWin, WindowController::KEY_MOUSE_LEFT );
            }
            else if( message == WM_LBUTTONUP )
            {
                ReleaseCapture();
                pController->OnKeyUp( pWin, WindowController::KEY_MOUSE_LEFT );
            }
            else if( message == WM_RBUTTONDOWN )
            {
                SetCapture( hWnd ); // make sure that mouse events get captured even if dragging outside window
                pController->OnKeyDown( pWin, WindowController::KEY_MOUSE_RIGHT );
            }
            else if( message == WM_RBUTTONUP )
            {
                ReleaseCapture();
                pController->OnKeyUp( pWin, WindowController::KEY_MOUSE_RIGHT );
            }
            else if( message == WM_MOUSEMOVE )
            {
                int xPos = LOWORD(lParam); 
                int yPos = HIWORD(lParam); 
                if(xPos & 1 << 15) xPos -= (1 << 16);
	            if(yPos & 1 << 15) yPos -= (1 << 16);

                pController->OnMouseMove( pWin, xPos, yPos );
            }
            else if( message == WM_CLOSE )
            {
                pController->OnClose( pWin );
                return 0;
            }
            else if( message == WM_SIZE )
            {
                pController->OnSized( pWin, LOWORD(lParam), HIWORD(lParam) );
            }
            else if( message == WM_DESTROY )
            {
                return 0;
            }
            
            return DefWindowProc( hWnd, message, wParam, lParam );
           
        }
    };

    HWND CreateWin( unsigned int nWidth, unsigned int nHeight, const char* pName, const char* pCaption, WNDPROC proc, bool resize )
    {
        // register window class
        WNDCLASS wc;
	    wc.style			= CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc	    = proc;
	    wc.cbClsExtra		= 0;
	    wc.cbWndExtra		= 0;
	    wc.hInstance		= 0;
	    wc.hIcon			= NULL;
	    wc.hCursor          = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground    = 0;
	    wc.lpszMenuName	    = NULL;
	    wc.lpszClassName	= pName;
        RegisterClass(&wc);

        DWORD nStyle = WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU ;
        if( !resize )
            nStyle = nStyle & ~(WS_MAXIMIZEBOX|WS_THICKFRAME);

        // make window
        HWND hWnd = CreateWindow( pName,
                    pCaption, 
                    nStyle, 
                    0, 
                    0, 
                    nWidth, 
                    nHeight, 
                    0, 
                    NULL, 
                    0, 
                    NULL );


        InvalidateRect( hWnd, NULL, true );

        // show the window and run the message pump
        ShowWindow( hWnd, SW_SHOW );

        return hWnd;
    }


   

    //=====================================================================================================================
    //
    //         Constructors/Destructors
    //
    //=====================================================================================================================

    Window::Window( WindowController* pController ) 
        : m_pController( pController ), m_bWasDestroyed(false)
    {
    }

    Window::~Window()
    {
    }

    //=====================================================================================================================
    //
    //            Public Methods
    //
    //=====================================================================================================================

    //=====================================================================================================================
    /// Processes window messages and forwards them to the window's controller.  If a window is destroyed
    ///  by calling the 'Destroy' method, this method must still be called to complete the window's destruction
    ///
    /// \return false if the window was destroyed.  True otherwise.  If false is returned, it means that
    ///    the 'DoEvents' call recursively deleted the window instance (via a call to a window controller)
    //=====================================================================================================================
    bool Window::DoEvents()
    {
        m_bInEventHandler = true;

        // process window messages
        HWND hHandle = reinterpret_cast<HWND>( m_pHandle );           
        MSG msg;
        while( PeekMessage( &msg, hHandle, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        m_bInEventHandler = false;

        // if a 'Destroy' call occurred, then deallocate the window now
        if( m_bWasDestroyed )
        {
            delete this;
            return false;
        }

        return true;
    }


    //=====================================================================================================================
    /// This method destroys the window and deallocates the Window insance
    //=====================================================================================================================
    void Window::Destroy()
    {
        // set a flag indicating that this window is being destroyed.  The next DoEvents call will
        //   deallocate it and return false
        m_bWasDestroyed = true;

        // tell windows to destruct the window
        DestroyWindow( (HWND) m_pHandle );   

        // if we are handling an event, defer destruction until after event handling
        //  otherwise, destroy now
        if( !m_bInEventHandler )
        {
            delete this;
        }
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void Window::SetCaption( const char* what )
    {
        SetWindowText( (HWND)m_pHandle,what);
    }


    //=====================================================================================================================
    //
    //            Protected/Private Methods
    //
    //=====================================================================================================================

    
    //=====================================================================================================================
    /// \param nWidth       Width of the window, in pixels
    /// \param nHeight      Height of the window, in pixels
    /// \param pName        The window name.  If NULL, then "UNNAMED_WINDOW" is used.
    /// \param pCaption     The window caption.  If NULL, then "UNNAMED_WINDOW" is used.
    /// \param pController  A window controller which the window will pass events to.  
    //=====================================================================================================================
    bool Window::FinishCreate( unsigned int nWidth,  unsigned int nHeight, unsigned int flags  )
    {
        WNDPROC proc = &WindowFriends::BaseWindowProc;

        bool resize = (flags & FLAG_NO_RESIZE) != 0;
        
        HWND hWnd = CreateWin( nWidth, nHeight, "Simpleton", "Simpleton", proc, resize );
        if( !hWnd )
        {
            return false;
        }

        SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR) this );
        m_pHandle = hWnd;
        return true;
    }

}

