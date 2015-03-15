//=====================================================================================================================
//
//   WindowController.h
//
//   Definition of class: Simpleton::WindowController
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _WINDOWCONTROLLER_H_
#define _WINDOWCONTROLLER_H_


namespace Simpleton
{
    class WindowPainter;
    class Window;
    class GDIWindow;

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A base class for an object which receives messages from the window system
    ///
    ///  To use the 'Window' utility class, you must define a window controller class which handles messages from the window
    ///    
    //=====================================================================================================================
    class WindowController
    {
    public:

        enum KeyCode
        {
            KEY_INVALID,
            KEY_A = 'a',
            KEY_B,
            KEY_C,
            KEY_D,
            KEY_E,            
            KEY_F,
            KEY_G,
            KEY_H,
            KEY_I,
            KEY_J,
            KEY_K,
            KEY_L,
            KEY_M,
            KEY_N,
            KEY_O,
            KEY_P,
            KEY_Q,
            KEY_R,
            KEY_S,
            KEY_T,
            KEY_U,
            KEY_V,
            KEY_W,
            KEY_X,
            KEY_Y,
            KEY_Z,
            KEY_0 = '0',
            KEY_1,
            KEY_2,
            KEY_3,
            KEY_4,
            KEY_5,
            KEY_6,
            KEY_7,
            KEY_8,
            KEY_9,
            KEY_SPACE,
            KEY_ENTER,
            KEY_ESCAPE,
            KEY_MOUSE_LEFT,
            KEY_MOUSE_RIGHT,
            KEY_PLUS,
            KEY_MINUS,
            KEY_PRINTSCREEN,
        };

        /// Called when the user clicks the 'close' button on a window
        virtual void OnClose( Window* pWindow )
        {
            pWindow->Destroy();
        };

        /// Called when a key is pressed
        virtual void OnKeyDown( Window* pWindow, KeyCode eKey ) {};

        /// Called when a key is released
        virtual void OnKeyUp( Window* pWindow, KeyCode eKey ) {};

        /// Called when the mouse is moved
        virtual void OnMouseMove( Window* pWindow, int x, int y ) {};

        /// Multi-touch events
        virtual void OnTouchDown( Window* pWindow, unsigned int ptr, int x, int y ) {};
        virtual void OnTouchUp( Window* pWindow, unsigned int ptr, int x, int y ) {};
        virtual void OnTouchMove( Window* pWindow, unsigned int ptr, int x, int y ) {};

        virtual void OnSized( Window* pWindow, unsigned int nNewClientWidth, unsigned int nNewClientHeight ) {};
    };
   
}

#endif // _WINDOWCONTROLLER_H_
