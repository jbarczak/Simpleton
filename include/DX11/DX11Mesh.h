//=====================================================================================================================
//
//   DX11Mesh.h
//
//   Simple renderable mesh helper class
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "ComPtr.h"

namespace Simpleton
{
    struct PlyMesh;

    class DX11Mesh
    {
    public:
        enum
        {
            MAX_VERTEX_ELEMENTS=8
        };

        
        void InitFromPly( ID3D11Device* pDevice, const Simpleton::PlyMesh& mesh );
        void InitXZQuad( ID3D11Device* pDevice );
        void InitXYQuad( ID3D11Device* pDevice );
        void InitCylinder( uint nRadialSplits, uint nAxialSegments, ID3D11Device* pDevice );

        ID3D11Buffer* GetVB() const { return m_pVB; }
        ID3D11Buffer* GetIB() const { return m_pIB; };
        const D3D11_INPUT_ELEMENT_DESC* GetVertexElements() const { return m_VertexElements; };
        uint GetVertexElementCount() const { return m_nVertexElements; };
        uint GetVertexCount() const { return m_nVertexCount; };
        uint GetIndexCount() const { return m_nIndexCount; };
        uint GetVertexSize() const { return m_nBytesPerVertex; };
        DXGI_FORMAT GetIndexFormat() const { return m_eIndexFormat; }

        
        void Draw( ID3D11DeviceContext* pCtx );

    private:

        void CreateBuffers( ID3D11Device* pDevice, const void* pVB, const void* pIB );

        ComPtr<ID3D11Buffer> m_pVB;
        ComPtr<ID3D11Buffer> m_pIB;
        D3D11_INPUT_ELEMENT_DESC m_VertexElements[MAX_VERTEX_ELEMENTS];
        uint m_nBytesPerIndex;
        uint m_nVertexElements;
        uint m_nBytesPerVertex;
        uint m_nVertexCount;
        uint m_nIndexCount;
        DXGI_FORMAT m_eIndexFormat;
    };


    void CreateVBIB( ID3D11Device* pDev, ID3D11Buffer** pVB, ID3D11Buffer** pIB, const void* pVBData, const void* pIBData, uint nVBSize, uint nIBSize );

};