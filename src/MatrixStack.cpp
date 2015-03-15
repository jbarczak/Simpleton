//=====================================================================================================================
//
//   MatrixStack.cpp
//
//   Definition of class: Simpleton::MatrixStack
//
//   The lazy man's utility library.
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "MatrixStack.h"

namespace Simpleton
{

    inline float DegreeToRad( float f ) { return f*(3.1415926f/180.0f); };
    
    //=====================================================================================================================
    //
    //         Constructors/Destructors
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    /// Default constructor, constructs an empty matrix stack
    //=====================================================================================================================
    MatrixStack::MatrixStack()
    {
        m_MTWStack.push_back( Matrix4f::Identity() ); // initialize top transform to identity.  This first matrix can never be popped
        m_WTMStack.push_back( Matrix4f::Identity() );
    }

    //=====================================================================================================================
    //
    //            Public Methods
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    /// Calling this method invalidates any references returned by GetTransform() or GetInverse()
    //=====================================================================================================================
    void MatrixStack::Push()
    {
        m_MTWStack.push_back( m_MTWStack.back() );
        m_WTMStack.push_back( m_WTMStack.back() );
    }

    //=====================================================================================================================
    /// Calling this method invalidates any references returned by GetTransform() or GetInverse()
    //=====================================================================================================================
    void MatrixStack::Pop()
    {
        // never pop the last matrix
        if( m_MTWStack.size() == 1 )
            return;

        m_MTWStack.pop_back();
        m_WTMStack.pop_back();
    }

    //=====================================================================================================================
    /// \param rMatrix          The transformation to apply to the top of the stack
    /// \param rMatrixInverse   The inverse of this transformation
    //=====================================================================================================================
    void MatrixStack::ApplyMatrix( const Matrix4f& rMatrix, const Matrix4f& rMatrixInverse )
    {    
        m_MTWStack.back() = rMatrix * m_MTWStack.back() ;
        m_WTMStack.back() = m_WTMStack.back() * rMatrixInverse ;
    }

    //=====================================================================================================================
    /// \param x, y, z  Axis of rotation
    /// \param fTheta   Rotation angle in degrees
    //=====================================================================================================================
    void MatrixStack::ApplyRotation( float x, float y, float z, float fTheta )
    {
        Matrix4f xform = MatrixRotate(x,y,z, DegreeToRad(fTheta) );
        Matrix4f inv = MatrixRotate(x,y,z,DegreeToRad(-fTheta) );
        ApplyMatrix(xform,inv);
    }

    //=====================================================================================================================
    /// \param x,y,z  Translation vector
    //=====================================================================================================================
    void MatrixStack::ApplyTranslation( float x, float y, float z )
    {
        Matrix4f xform = MatrixTranslate(x,y,z);
        Matrix4f inv = MatrixTranslate(-x,-y,-z);
        ApplyMatrix(xform, inv);
    }

    //=====================================================================================================================
    /// \param x,y,z  Scale factors
    //=====================================================================================================================
    void MatrixStack::ApplyScale( float x, float y, float z )
    {
        Matrix4f xform = MatrixScale(x,y,z);
        Matrix4f inv = MatrixScale(1.0f/x, 1.0f/y, 1.0f/z);
        ApplyMatrix(xform, inv);
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void MatrixStack::ChangeCoordinateFrame( const Vec3f& X, const Vec3f& Y, const Vec3f& Z )
    {
        Matrix4f xform = MatrixCoordinateFrame(X,Y,Z);
        Matrix4f xpose = Inverse(xform);
        ApplyMatrix(xform,xpose);
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void MatrixStack::SetIdentity()
    {
        m_WTMStack.back() = Matrix4f::Identity(); // identity
        m_MTWStack.back() = Matrix4f::Identity();
    }

    //=====================================================================================================================
    /// Calling this method invalidates references returned by GetTransform() or GetInverse()
    //=====================================================================================================================
    void MatrixStack::Clear()
    {
        m_WTMStack.clear();
        m_MTWStack.clear();
        m_WTMStack.push_back( Matrix4f::Identity() );
        m_MTWStack.push_back( Matrix4f::Identity() );
    }


    //=====================================================================================================================
    //
    //           Protected Methods
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    //
    //            Private Methods
    //
    //=====================================================================================================================
}

