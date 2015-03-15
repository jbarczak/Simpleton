//=====================================================================================================================
//
//   WindowPainter.h
//
//   Definition of class: Simpleton::WindowPainter
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================
#ifndef _WINDOWPAINTER_H_
#define _WINDOWPAINTER_H_


namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief Interface for the rendering operations provided by the windowing system
    //=====================================================================================================================
    class WindowPainter
    {
    public:

        struct BlitParameters
        {
            // Destination rectangle
            int nTop;       
            int nLeft;
            int nWidth;
            int nHeight;

            // Source rectangle
            int nSrcTop;
            int nSrcLeft;
            int nSrcWidth;
            int nSrcHeight;
            
            // Source image dimensions, and bits (BGRA pixels)
            int nSrcImageWidth;
            int nSrcImageHeight;
            const unsigned char* pBits;
        };

        /// Queries for the rectangle on the window that is to be painted
        virtual void GetPaintRect( int* pTop, int* pLeft, int *pWidth, int* pHeight ) = 0;

        /// Returns the size of the window's "Client area", in pixels
        virtual void GetClientSize( unsigned int* pWidth, unsigned int* pHeight ) = 0;

        /// Copies a block of BGRA pixels onto a region of the window
        virtual void BlitBGRAPixels( const BlitParameters& rBlit ) = 0;
     

    };
    
}

#endif // _WINDOWCONTROLLER_H_
