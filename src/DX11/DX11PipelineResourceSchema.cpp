//=====================================================================================================================
//
//   DX11PipelineResourceSchema.cpp
//
//   Implementation of class: Simpleton::DX11PipelineResourceSchema
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "Types.h"
#include "DX11/DX11PipelineResourceSchema.h"
#include "DX11/DX11PipelineResourceSet.h"
#include "ComPtr.h"
#include "CRC.h"
#include "MiscMath.h"

#include <d3dcompiler.h>
#include <algorithm>

namespace Simpleton
{


    //=====================================================================================================================
    //
    //            Public Methods
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    //=====================================================================================================================
    DX11PipelineResourceSchema* DX11PipelineResourceSchema::Create( const void* pVS, const void* pGS, const void* pPS,  uint nVSLength, uint nGSLength, uint nPSLength, uint nVertexBuffers )
    {
        ID3D11ShaderReflection* ppReflections[STAGE_COUNT] = {0};

        D3DReflect( pVS, nVSLength, __uuidof(ID3D11ShaderReflection), (void**)&ppReflections[STAGE_VS] );
        
        if( pPS && nPSLength )
            D3DReflect( pPS, nPSLength, __uuidof(ID3D11ShaderReflection), (void**)&ppReflections[STAGE_PS] );
       
        if( pGS && nGSLength )
            D3DReflect( pGS, nGSLength, __uuidof(ID3D11ShaderReflection), (void**)&ppReflections[STAGE_GS] );
       
        DX11PipelineResourceSchema* pS = new DX11PipelineResourceSchema();
        pS->BuildSchema(ppReflections, STAGE_COUNT);

        for( size_t i=0; i<STAGE_COUNT; i++ )
        {
            pS->m_pStageCBCounts[i]      = DX11ResourceSchema::CountCBs(ppReflections[i]);
            pS->m_pStageSamplerCounts[i] = DX11ResourceSchema::CountSamplers(ppReflections[i]);
            pS->m_pStageSRVCounts[i]     = DX11ResourceSchema::CountTextures(ppReflections[i]);
        }
        
        return pS;
    }

    
    //=====================================================================================================================
    //=====================================================================================================================    
    void DX11PipelineResourceSchema::CreateResourceSet( DX11PipelineResourceSet* pSet, ID3D11Device* pDevice ) const
    {
        if( pSet->GetSchema() )
            pSet->GetSchema()->DestroyResourceSet(pSet);
        
        DX11ResourceSchema::CreateResourceSet( pSet, pDevice );

        for( uint s=0; s<STAGE_COUNT; s++ )
        {
            pSet->m_pSamplerCounts[s] = m_pStageSamplerCounts[s];
            pSet->m_pSRVCounts[s]     = m_pStageSRVCounts[s];
            pSet->m_pCBCounts[s]      = m_pStageCBCounts[s];
        }
    }
   
  

    //=====================================================================================================================
    //
    //            Private Methods
    //
    //=====================================================================================================================


}

