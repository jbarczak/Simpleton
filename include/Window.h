
//=====================================================================================================================
//
//   Window.h
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

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stddef.h>


namespace Simpleton
{

    class WindowController;


    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A class which encapsulates the ugly mechanics of the windowing system
    ///
    ///  This class encapsulates the window creation and window message handling
    ///   To use this class, simply create a 'WindowController' subclass which implements the behavior your want
    ///    and then call 'Create' to construct a window.  The window will forward messages to your controller instance
    ///
    ///  In order for window messages to be processed, you must call the 'DoEvents' method periodically so that
    ///   the windowing system can process your input events.  During a 'DoEvents' call, calls will be made to the various 
    ///    message handlers on the window controller
    ///
    ///  If you want to get rid of the window, simply call the 'Destroy' method.  The window will automatically deallocate
    ///   itself when 'Destroy' is called on it
    ///
    //=====================================================================================================================
    class Window
    {
    public:

        inline WindowController* GetController() const { return m_pController; };

        /// Processes input messages (such as keyboard and mouse events).  Returns false if the window was destroyed
        virtual bool DoEvents( ) ;

        /// Destroys the window instance
        void Destroy( );

        /// Change window caption
        void SetCaption( const char* pWhat );

    protected:
        
        virtual bool WndProcHook( void* pHWND, unsigned int nMsg, intptr_t wParam, intptr_t lParam ){return false;}
        virtual void DestroyHook() {};

        bool FinishCreate( unsigned int nWidth, unsigned int nHeight );

        void* GetHandle() { return m_pHandle; }
        
        Window( WindowController* pController );
        virtual ~Window();

    private:
  
        friend class WindowFriends;

        WindowController* m_pController;
        void* m_pHandle;
   
        bool m_bWasDestroyed;
        bool m_bInEventHandler;
    };
    
}

#endif // _WINDOW_H_
