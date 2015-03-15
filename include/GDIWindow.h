
//=====================================================================================================================
//
//   GDIWindow.h
//
//   Definition of class: Simpleton::GDIWindow
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _GDIWINDOW_H_
#define _GDIWINDOW_H_

#include "Window.h"
#include "WindowController.h"

namespace Simpleton
{
    class GDIWindowController;
  
    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A class which encapsulates the ugly mechanics of the windowing system
    ///
    ///  This class encapsulates a window that is rendered to using Win32 GDI calls
    ///    This is useful for playing with offline renderers and raytracers
    ///
    //=====================================================================================================================
    class GDIWindow : public Window
    {
    public:

        static GDIWindow* Create( unsigned int nWidth, unsigned int nHeight, GDIWindowController* pController );
        
        /// Redraws the entire window
        void RepaintAll( );

    private:
  
        virtual bool WndProcHook(void* pHWnd, unsigned int msg, intptr_t wParam, intptr_t lParam );
        GDIWindow( GDIWindowController* pController);
    };


    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A base class for an object which receives messages from the window system
    ///
    ///  To use the 'Window' utility class, you must define a window controller class which handles messages from the window
    ///    
    //=====================================================================================================================
    class GDIWindowController : public WindowController
    {
    public:
        
        /// Called when the window is re-painted
        virtual void OnPaint( GDIWindow* pWindow, WindowPainter* pPainter ) {};

    };

    
}

#endif // _GDIWINDOW_H_
