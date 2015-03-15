//=====================================================================================================================
//
//   DX11Mesh.cpp
//
//   Implementation of class: Simpleton::DX11Mesh
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include <vector>
#include "Types.h"
#include "Tessellate.h"
#include "DX11/DX11Mesh.h"
#include "PlyLoader.h"

namespace Simpleton
{

    
    //=====================================================================================================================
    //
    //         Constructors/Destructors
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    //
    //            Public Methods
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    //=====================================================================================================================    
    void DX11Mesh::InitFromPly( ID3D11Device* pDevice, const Simpleton::PlyMesh& m )
    {
        m_VertexElements[0].AlignedByteOffset = 0;
        m_VertexElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        m_VertexElements[0].InputSlot = 0;
        m_VertexElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        m_VertexElements[0].InstanceDataStepRate = 0;
        m_VertexElements[0].SemanticIndex = 0;
        m_VertexElements[0].SemanticName = "POSITION";

        uint nElems=1;
        uint nFloatsPerVert = 3;
        if( m.pNormals )
        {
            m_VertexElements[nElems].AlignedByteOffset = nFloatsPerVert*sizeof(float);
            m_VertexElements[nElems].Format = DXGI_FORMAT_R32G32B32_FLOAT;
            m_VertexElements[nElems].InputSlot = 0;
            m_VertexElements[nElems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            m_VertexElements[nElems].InstanceDataStepRate = 0;
            m_VertexElements[nElems].SemanticIndex = 0;
            m_VertexElements[nElems].SemanticName = "NORMAL";
            nFloatsPerVert += 3;
            nElems++;
        }

        if( m.pUVs )
        {
            m_VertexElements[nElems].AlignedByteOffset = nFloatsPerVert*sizeof(float);
            m_VertexElements[nElems].Format = DXGI_FORMAT_R32G32_FLOAT;
            m_VertexElements[nElems].InputSlot = 0;
            m_VertexElements[nElems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            m_VertexElements[nElems].InstanceDataStepRate = 0;
            m_VertexElements[nElems].SemanticIndex = 0;
            m_VertexElements[nElems].SemanticName = "TEXCOORD";
            nFloatsPerVert += 2;
            nElems++;
        }

        m_nVertexElements = nElems;
        m_nBytesPerIndex  = 4;
        m_nBytesPerVertex = sizeof(float)*nFloatsPerVert;
        m_eIndexFormat = DXGI_FORMAT_R32_UINT;
        m_nIndexCount = m.nTriangles*3;
        m_nVertexCount = m.nVertices;
        
        void* pVBData = malloc( m_nVertexCount*m_nBytesPerVertex);
        float* pFloats = (float*)pVBData;
        for( uint i=0; i<m_nVertexCount; i++ )
        {
            pFloats[0] = m.pPositions[i][0];
            pFloats[1] = m.pPositions[i][1];
            pFloats[2] = m.pPositions[i][2];
            pFloats += 3;
            if( m.pNormals )
            {
                pFloats[0] = m.pNormals[i][0];
                pFloats[1] = m.pNormals[i][1];
                pFloats[2] = m.pNormals[i][2];
                pFloats += 3;
            }
            if( m.pUVs )
            {
                pFloats[0] = m.pUVs[i][0];
                pFloats[1] = m.pUVs[i][1];
                pFloats += 2;
            }
        }

        ID3D11Buffer* pVB;
        ID3D11Buffer* pIB;
        void* pIBData = m.pVertexIndices;
        CreateVBIB( pDevice, &pVB, &pIB, pVBData, pIBData, m_nVertexCount*m_nBytesPerVertex, m_nIndexCount*m_nBytesPerIndex );

        m_pVB.Owns(pVB);
        m_pIB.Owns(pIB);
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11Mesh::InitXZQuad( ID3D11Device* pDevice )
    {
        //  0    1
        //  
        //  2    3
        //
        uint16 ib[] = { 0,1,2, 1,3,2 };
        float vb[] = {
            -1,0,1,   0,1,0, 0,0,
             1,0,1,   0,1,0, 1,0,
             1,0,-1,  0,1,0, 1,1,
            -1,0,-1,  0,1,0, 0,1
        };

        D3D11_INPUT_ELEMENT_DESC elems[] ={
            { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "UV",0,DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        m_nBytesPerIndex  = 2;
        m_nBytesPerVertex = 32;
        m_nVertexElements = 3;
        m_nVertexCount    = 4;
        m_nIndexCount     = 6;
        m_eIndexFormat    = DXGI_FORMAT_R16_UINT;
        memcpy( m_VertexElements, elems, sizeof(D3D11_INPUT_ELEMENT_DESC)*m_nVertexElements );
        CreateBuffers(pDevice,vb,ib);
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11Mesh::InitXYQuad( ID3D11Device* pDevice )
    {
        //  0    1
        //  
        //  2    3
        //
        uint16 ib[] = { 0,1,2, 1,3,2 };
        float vb[] = {
            -1,1,0,   0,0,1, 0,0,
             1,1,0,   0,0,1, 1,0,
            -1,-1,0,  0,0,1, 0,1,
             1,-1,0,  0,0,1, 1,1,
        
        };

        D3D11_INPUT_ELEMENT_DESC elems[] ={
            { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        m_nBytesPerIndex  = 2;
        m_nBytesPerVertex = 32;
        m_nVertexElements = 3;
        m_nVertexCount    = 4;
        m_nIndexCount     = 6;
        m_eIndexFormat    = DXGI_FORMAT_R16_UINT;
        memcpy( m_VertexElements, elems, sizeof(D3D11_INPUT_ELEMENT_DESC)*m_nVertexElements );
        CreateBuffers(pDevice,vb,ib);
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11Mesh::InitCylinder( uint nRadialSplits, uint nAxialSegments, ID3D11Device* pDevice )
    {
        std::vector<TessVertex> vb;
        std::vector<uint> ib;
        Simpleton::CreateCylinder( nRadialSplits, nAxialSegments,vb,ib );

        D3D11_INPUT_ELEMENT_DESC elems[] ={
            { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        m_nBytesPerIndex=4;
        m_nBytesPerVertex=32;
        m_nVertexCount = (uint) vb.size();
        m_nIndexCount  = (uint) ib.size();
        m_nVertexElements=3;
        m_eIndexFormat = DXGI_FORMAT_R32_UINT;
        memcpy( m_VertexElements, elems, sizeof(D3D11_INPUT_ELEMENT_DESC)*m_nVertexElements );
        CreateBuffers(pDevice,vb.data(), ib.data() );
    }

    void DX11Mesh::Draw( ID3D11DeviceContext* pContext )
    {
        UINT nStride = m_nBytesPerVertex;
        UINT nOffset = 0;
        ID3D11Buffer* pVB = m_pVB;
        pContext->IASetVertexBuffers( 0,1,&pVB, &nStride, &nOffset );
        pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0 );
        pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        pContext->DrawIndexed( m_nIndexCount, 0, 0 );
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

    void DX11Mesh::CreateBuffers( ID3D11Device* pDevice, const void* pVB, const void* pIB )
    {         
        D3D11_BUFFER_DESC vbdesc;
        vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbdesc.ByteWidth = m_nBytesPerVertex*m_nVertexCount;
        vbdesc.CPUAccessFlags = 0;
        vbdesc.MiscFlags = 0;
        vbdesc.StructureByteStride = 0;
        vbdesc.Usage = D3D11_USAGE_DEFAULT;
       
        D3D11_BUFFER_DESC ibdesc;
        ibdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibdesc.ByteWidth = m_nBytesPerIndex*m_nIndexCount;
        ibdesc.CPUAccessFlags = 0;
        ibdesc.MiscFlags = 0;
        ibdesc.StructureByteStride = 0;
        ibdesc.Usage = D3D11_USAGE_DEFAULT;
        
        D3D11_SUBRESOURCE_DATA vbdata;
        vbdata.pSysMem = pVB;
        vbdata.SysMemPitch = vbdesc.ByteWidth;
        vbdata.SysMemSlicePitch = vbdata.SysMemPitch;
        D3D11_SUBRESOURCE_DATA ibdata;
        ibdata.pSysMem = pIB;
        ibdata.SysMemPitch = ibdesc.ByteWidth;
        ibdata.SysMemSlicePitch = ibdata.SysMemPitch;

        ID3D11Buffer* pDXVB;
        ID3D11Buffer* pDXIB;
        pDevice->CreateBuffer( &vbdesc, &vbdata, &pDXVB );
        pDevice->CreateBuffer( &ibdesc, &ibdata, &pDXIB );
        m_pVB.Owns(pDXVB);
        m_pIB.Owns(pDXIB);
    }


    void CreateVBIB( ID3D11Device* pDevice, ID3D11Buffer** pVB, ID3D11Buffer** pIB, const void* pVBData, const void* pIBData, uint nVBSize, uint nIBSize )
    {
        D3D11_BUFFER_DESC vbdesc;
        vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbdesc.ByteWidth = nVBSize;
        vbdesc.CPUAccessFlags = 0;
        vbdesc.MiscFlags = 0;
        vbdesc.StructureByteStride = 0;
        vbdesc.Usage = D3D11_USAGE_DEFAULT;
       
        D3D11_BUFFER_DESC ibdesc;
        ibdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibdesc.ByteWidth = nIBSize;
        ibdesc.CPUAccessFlags = 0;
        ibdesc.MiscFlags = 0;
        ibdesc.StructureByteStride = 0;
        ibdesc.Usage = D3D11_USAGE_DEFAULT;
        
        D3D11_SUBRESOURCE_DATA vbdata;
        vbdata.pSysMem = pVBData;
        vbdata.SysMemPitch = vbdesc.ByteWidth;
        vbdata.SysMemSlicePitch = vbdata.SysMemPitch;
        D3D11_SUBRESOURCE_DATA ibdata;
        ibdata.pSysMem = pIBData;
        ibdata.SysMemPitch = ibdesc.ByteWidth;
        ibdata.SysMemSlicePitch = ibdata.SysMemPitch;

        pDevice->CreateBuffer( &vbdesc, &vbdata, pVB );
        pDevice->CreateBuffer( &ibdesc, &ibdata, pIB );

    }

}

