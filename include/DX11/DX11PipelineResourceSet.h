
#ifndef _DX11_PIPELINE_RESOURCE_SET_H_
#define _DX11_PIPELINE_RESOURCE_SET_H_


#include "DX11/DX11PipelineResourceSchema.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;


namespace Simpleton
{

    class DX11PipelineResourceSchema;

    class DX11PipelineResourceSet
    {
    public:
        DX11PipelineResourceSet();

        ~DX11PipelineResourceSet();
        
        void BeginUpdate( ID3D11DeviceContext* pCtx );
        void BindSRV( uint nName, ID3D11ShaderResourceView* pSRV );
        void BindSampler( uint nName, ID3D11SamplerState* pSampler );
        void BindConstant( uint nName, const void* pBytes, uint nBytes );
        void BindSRV( const char* pName, ID3D11ShaderResourceView* pSRV ) { BindSRV( m_pSchema->LookupSRV(pName),pSRV); }
        void BindSampler( const char* pName, ID3D11SamplerState* pSampler ) { BindSampler( m_pSchema->LookupSampler(pName),pSampler); }
        void BindConstant( const char* pName, const void* pBytes, uint nBytes ) {BindConstant(m_pSchema->LookupConstant(pName),pBytes,nBytes); }
        
        void EndUpdate( ID3D11DeviceContext* pCtx );

        void Apply( ID3D11DeviceContext* pCtx );

    private:
        
        friend class DX11PipelineResourceSchema;
        
        const DX11PipelineResourceSchema* m_pSchema;
        uint8 m_pCBCounts[DX11PipelineResourceSchema::STAGE_COUNT];
        uint8 m_pSRVCounts[DX11PipelineResourceSchema::STAGE_COUNT];
        uint8 m_pSamplerCounts[DX11PipelineResourceSchema::STAGE_COUNT];
        uint8 m_nUniqueCBs;
       
        uint m_nCBMovements;
        
        // Pointers into the resource schema.  These are stored here to remove indirections
        const DX11PipelineResourceSchema::UniqueConstant* m_pConstantsByName;
        const DX11PipelineResourceSchema::CBMovement* m_pCBMovements;
        const uint8* m_pBindIndices;

        // all of these point into a block allocation which also contains the 'resourceset instance'
        // TODO: Replace with, e.g. 2-byte offsets  
        ID3D11ShaderResourceView** m_pSRVsByName; 
        ID3D11SamplerState** m_pSamplersByName;
        ID3D11Buffer** m_pConstantBuffers;
        uint8* m_pConstantStaging;
    };


}

#endif