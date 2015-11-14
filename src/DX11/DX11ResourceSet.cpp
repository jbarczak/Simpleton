//=====================================================================================================================
//
//   DX11ResourceSet.cpp
//
//   Implementation of class: Simpleton::DX11ResourceSet
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include "MiscMath.h"
#include "DX11/DX11ResourceSet.h"

namespace Simpleton
{
    //=====================================================================================================================
    //
    //            Constructors/Destructors
    //
    //=====================================================================================================================

    DX11ResourceSet::DX11ResourceSet() : m_pSchema(0)
    {
    }
    DX11ResourceSet::~DX11ResourceSet()
    {
        m_pSchema->DestroyResourceSet(this);
    }
    
    //=====================================================================================================================
    //
    //            Public methods
    //
    //=====================================================================================================================

    void DX11ResourceSet::BeginUpdate(ID3D11DeviceContext* pCtx )
    {
        // TODO: Eventually set state to optimize CB updates here.
    }

    void DX11ResourceSet::BindSRV( uint nName, ID3D11ShaderResourceView* pSRV )
    {
        m_pSRVsByName[nName] = pSRV;
    }
    void DX11ResourceSet::BindSampler( uint nName, ID3D11SamplerState* pSampler )
    {
        m_pSamplersByName[nName] = pSampler;
    }
    void DX11ResourceSet::BindConstant( uint nName, const void* pBytes, uint nBytes )
    {
        byte* pLocation = m_pConstantStaging + m_pConstantsByName[nName].nStageOffset;
        nBytes = MIN(nBytes,m_pConstantsByName[nName].nStageSize );
        memcpy( pLocation, pBytes, nBytes );
    }
    void DX11ResourceSet::EndUpdate( ID3D11DeviceContext* pCtx )
    {
        byte* pMappedBuffers[256];
        for( uint i=0; i<m_nUniqueCBs; i++ )
        {
            D3D11_MAPPED_SUBRESOURCE mapped;
            pCtx->Map( m_pConstantBuffers[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
            pMappedBuffers[i] = reinterpret_cast<uint8*>( mapped.pData );
        }

        for( uint m=0; m<m_nCBMovements; m++ )
        {
            const byte* pSrc = m_pConstantStaging + m_pCBMovements[m].nStageOffset;
            byte* pDst = pMappedBuffers[ m_pCBMovements[m].nBufferIndex ] + m_pCBMovements[m].nBufferOffset;
            memcpy( pDst, pSrc, m_pCBMovements[m].nSize );
        }

        for( uint i=0; i<m_nUniqueCBs; i++ )
            pCtx->Unmap( m_pConstantBuffers[i], 0 );
    }


 }

