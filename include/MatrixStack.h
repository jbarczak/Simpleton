//=====================================================================================================================
//
//   MatrixStack.h
//
//   Definition of class: MatrixStack
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _MATRIXSTACK_H_
#define _MATRIXSTACK_H_

#include "Matrix.h"
#include "VectorMath.h"
#include <vector>

namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief This class encapsulates a simple transformation stack
    ///    For simplicity (and to avoid the need for a general matrix inversion routine), this class assumes
    ///      that the inverse of a matrix is always available, and maintains two seperate stacks.
    ///
    ///    This is necessary because the inverse of a transform is needed to transform normals
    ///
    //=====================================================================================================================
    class MatrixStack
    {
    public:
    
        MatrixStack();

        /// Pushes a copy of the current transformation onto the stack
        void Push( );

        /// Pops the current transformation from the stack.  Has no effect if stack size is 1
        void Pop( );

        /// Multiplies the stack top by an arbitrary transformation
        void ApplyMatrix( const Matrix4f& rMatrix, const Matrix4f& rMatrixInverse );

        /// Multiplies the top transform by a translation matrix
        void ApplyTranslation( float x, float y, float z );

        /// Multiplies the top transform by a rotation matrix
        void ApplyRotation( float x, float y, float z, float fTheta );

        /// Multiplies the top transform by a scaling matrix
        void ApplyScale( float x, float y, float z );
        void ApplyScale( float s ) { ApplyScale(s,s,s); };

        void ChangeCoordinateFrame( const Vec3f& X, const Vec3f& Y, const Vec3f& z );

        /// Sets the top transform to identity
        void SetIdentity();

        /// Returns the top transformation on the stack
        const Matrix4f& GetTransform() const { return m_MTWStack.back(); };

        /// Returns the inverse of the top transformation
        const Matrix4f& GetInverse() const { return m_WTMStack.back(); };

        /// Removes all pushed transformations from the stack, and initializes the top transform to identity
        void Clear();

    private:

        std::vector< Matrix4f > m_MTWStack;
        std::vector< Matrix4f > m_WTMStack;

    };
    
}

#endif // _LUMATRIXSTACK_H_
