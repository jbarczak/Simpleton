//=====================================================================================================================
//
//   DX11ComputeResourceSet.cpp
//
//   Implementation of class: Simpleton::DX11ComputeResourceSchema
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "DX11/DX11ComputeResourceSet.h"
#include <d3d11.h>

namespace Simpleton
{
    void DX11ComputeResourceSet::Apply( ID3D11DeviceContext* pContext )
    {
        ID3D11SamplerState* pSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
        ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
        ID3D11Buffer* pCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

        const uint8* pBindIndices= m_pBindIndices;
        
        for( uint i=0; i < m_nUniqueSamplers; i++ )
            pSamplers[i] = m_pSamplersByName[*(pBindIndices++)];
        for( uint i=0; i < m_nUniqueTextures; i++ )
            pSRVs[i] = m_pSRVsByName[*(pBindIndices++)];
        for( uint i=0; i < m_nUniqueCBs; i++ )
            pCBs[i++] = m_pConstantBuffers[*(pBindIndices++)];
       
        pContext->CSSetSamplers(0, m_nUniqueSamplers, pSamplers );
        pContext->CSSetShaderResources( 0, m_nUniqueTextures, pSRVs );
        pContext->CSSetConstantBuffers( 0, m_nUniqueCBs, pCBs );
    }



}