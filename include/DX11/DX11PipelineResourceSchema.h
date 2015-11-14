//=====================================================================================================================
//
//   DX11PipelineResourceSchema.h
//
//   Description of the resource bindings for a given DX11 pipeline state
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_PIPELINE_RESOURCE_SCHEMA_H_
#define _DX11_PIPELINE_RESOURCE_SCHEMA_H_

#include "DX11ResourceSchema.h"

#include <vector>
#include <d3d11.h>

typedef unsigned int uint;
typedef unsigned char uint8;

namespace Simpleton
{

    class DX11PipelineResourceSet;


    class DX11PipelineResourceSchema : public DX11ResourceSchema
    {
    public:
             
        enum Stages
        {
            STAGE_VS,
            STAGE_GS,
            STAGE_PS,
            STAGE_COUNT
        };


        static DX11PipelineResourceSchema* Create( const void* pVS, const void* pGS, const void* pPS, uint nVSLength, uint nGSLength, uint nPSLength, uint nVertexBuffers );
        
        void CreateResourceSet( DX11PipelineResourceSet* pSet, ID3D11Device* pDevice ) const;
        
    private:
   
        
        friend class DX11PipelineResourceSet;

        DX11PipelineResourceSchema() {};
        virtual ~DX11PipelineResourceSchema() {};
       
        uint8 m_pStageSRVCounts[STAGE_COUNT];
        uint8 m_pStageSamplerCounts[STAGE_COUNT];
        uint8 m_pStageCBCounts[STAGE_COUNT];
        
    };


};

#endif