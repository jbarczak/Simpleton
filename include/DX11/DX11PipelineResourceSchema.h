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


#include <vector>
#include <d3d11.h>

typedef unsigned int uint;
typedef unsigned char uint8;

namespace Simpleton
{


    class DX11PipelineResourceSet;
    
    class DX11PipelineResourceSchema
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
        
        void Destroy();

        uint LookupConstant( const char* pName ) const { return NameLookup(m_ConstantNames,pName); }
        uint LookupSampler( const char* pName ) const { return NameLookup(m_SamplerNames,pName); }
        uint LookupSRV( const char* pName ) const { return NameLookup(m_SRVNames,pName); }
        void CreateResourceSet( DX11PipelineResourceSet* pSet, ID3D11Device* pDevice ) const;
        void DestroyResourceSet( DX11PipelineResourceSet* pSet ) const;

    private:
   
        static uint NameLookup( const std::vector<uint>& rNames, const char* pName );

        friend class DX11PipelineResourceSet;

        DX11PipelineResourceSchema() {};
        ~DX11PipelineResourceSchema() {};
        
        struct UniqueConstant
        {
            uint nStageOffset;
            uint nStageSize;
        };
           
        struct CBMovement
        {
            uint nStageOffset;
            uint nSize;
            uint nBufferIndex;
            uint nBufferOffset;
        };

        std::vector< uint > m_ConstantNames;
        std::vector<UniqueConstant> m_StagingLayout;
        std::vector<CBMovement> m_CBMovements;
        std::vector< uint > m_SamplerNames;
        std::vector< uint > m_SRVNames;
        std::vector< uint > m_CBSizes;
        std::vector< uint8 > m_BindIndices;
        uint m_nConstantStageBytes;
        uint8 m_pStageSRVCounts[STAGE_COUNT];
        uint8 m_pStageSamplerCounts[STAGE_COUNT];
        uint8 m_pStageCBCounts[STAGE_COUNT];
        
    };


};

#endif