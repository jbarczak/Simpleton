//=====================================================================================================================
//
//   DX11PipelineResourceSet.cpp
//
//   Implementation of class: Simpleton::DX11PipelineResourceSet
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include "DX11/DX11PipelineResourceSet.h"
#include "MiscMath.h"

namespace Simpleton
{
   
    //=====================================================================================================================
    //
    //         Pipeline Resource Set
    //
    //=====================================================================================================================
    
    DX11PipelineResourceSet::DX11PipelineResourceSet() 
    {
    }
    DX11PipelineResourceSet::~DX11PipelineResourceSet()
    {
    }
    
   
    //=====================================================================================================================
    //=====================================================================================================================
    void DX11PipelineResourceSet::Apply( ID3D11DeviceContext* pContext )
    {
        enum { 
            STAGE_COUNT = DX11PipelineResourceSchema::STAGE_COUNT,
            STAGE_VS = DX11PipelineResourceSchema::STAGE_VS,
            STAGE_GS = DX11PipelineResourceSchema::STAGE_GS,
            STAGE_PS = DX11PipelineResourceSchema::STAGE_PS,
        };

        ID3D11SamplerState* pSamplers[STAGE_COUNT][D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
        ID3D11ShaderResourceView* pSRVs[STAGE_COUNT][D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
        ID3D11Buffer* pCBs[STAGE_COUNT][D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

        const uint8* pBindIndices= m_pBindIndices;
        
        for( uint s=0; s<STAGE_COUNT; s++ )
            for( uint i=0; i < m_pSamplerCounts[s]; i++ )
                pSamplers[s][i] = m_pSamplersByName[*(pBindIndices++)];
        for( uint s=0; s<STAGE_COUNT; s++ )
            for( uint i=0; i<m_pSRVCounts[s]; i++ )
                pSRVs[s][i] = m_pSRVsByName[*(pBindIndices++)];
        for( uint s=0; s<STAGE_COUNT; s++ )
            for( uint i=0; i<m_pCBCounts[s]; i++ )
                pCBs[s][i] = m_pConstantBuffers[*(pBindIndices++)];
       
        pContext->VSSetSamplers(0, m_pSamplerCounts[STAGE_VS], pSamplers[STAGE_VS]);
        pContext->GSSetSamplers(0, m_pSamplerCounts[STAGE_GS], pSamplers[STAGE_GS]);
        pContext->PSSetSamplers(0, m_pSamplerCounts[STAGE_PS], pSamplers[STAGE_PS]);
        pContext->VSSetShaderResources(0,m_pSRVCounts[STAGE_VS], pSRVs[STAGE_VS]);
        pContext->GSSetShaderResources(0,m_pSRVCounts[STAGE_GS], pSRVs[STAGE_GS]);
        pContext->PSSetShaderResources(0,m_pSRVCounts[STAGE_PS], pSRVs[STAGE_PS]);
        pContext->VSSetConstantBuffers(0,m_pCBCounts[STAGE_VS], pCBs[STAGE_VS]);
        pContext->GSSetConstantBuffers(0,m_pCBCounts[STAGE_GS], pCBs[STAGE_GS]);
        pContext->PSSetConstantBuffers(0,m_pCBCounts[STAGE_PS], pCBs[STAGE_PS]);
    }


    //=====================================================================================================================
    //
    //           Compute Resource Set
    //
    //=====================================================================================================================
    
}

