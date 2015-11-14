
//=====================================================================================================================
//
//   DX11PipelineResourceSet.h
//
//   A set of resources bound to the graphics pipeline for a particular instance of a pipeline state
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_PIPELINE_RESOURCE_SET_H_
#define _DX11_PIPELINE_RESOURCE_SET_H_

#include "DX11ResourceSet.h"
#include "DX11PipelineResourceSchema.h"

namespace Simpleton
{
    class DX11PipelineResourceSet : public DX11ResourceSet
    {
    public:
        DX11PipelineResourceSet();

        virtual ~DX11PipelineResourceSet();
      
        void Apply( ID3D11DeviceContext* pCtx );

        const DX11PipelineResourceSchema* GetSchema() const { 
            return static_cast<const DX11PipelineResourceSchema*>( m_pSchema );
        }
    private:
        
        friend class DX11PipelineResourceSchema;
        
        uint8 m_pCBCounts[DX11PipelineResourceSchema::STAGE_COUNT];
        uint8 m_pSRVCounts[DX11PipelineResourceSchema::STAGE_COUNT];
        uint8 m_pSamplerCounts[DX11PipelineResourceSchema::STAGE_COUNT];
    };


}

#endif