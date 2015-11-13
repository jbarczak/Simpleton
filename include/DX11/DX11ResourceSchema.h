
//=====================================================================================================================
//
//   DX11ResourceSchema.h
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

#ifndef _DX11_RESOURCE_SCHEMA_H_
#define _DX11_RESOURCE_SCHEMA_H_

#include "Types.h"
#include <vector>

struct ID3D11Device;
struct ID3D11ShaderReflection;

namespace Simpleton
{
    class DX11ResourceSet;

    /// Base class for resource schemas
    class DX11ResourceSchema
    {
    public:
        
        uint LookupConstant( const char* pName ) const { return NameLookup(m_ConstantNames,pName); }
        uint LookupSampler( const char* pName ) const { return NameLookup(m_SamplerNames,pName); }
        uint LookupSRV( const char* pName ) const { return NameLookup(m_SRVNames,pName); }

        void Destroy() { delete this; }


    protected:

        friend class DX11ResourceSet;

        DX11ResourceSchema() {};
        virtual ~DX11ResourceSchema() {};


        static size_t CountSamplers( ID3D11ShaderReflection* pReflection );
        static size_t CountTextures( ID3D11ShaderReflection* pReflection );
        static size_t CountCBs( ID3D11ShaderReflection* pReflection );
        
        void BuildSchema( ID3D11ShaderReflection** ppReflections, size_t nReflections );
        void CreateResourceSet( DX11ResourceSet* pSet, ID3D11Device* pDevice ) const;

        static uint NameLookup( const std::vector<uint>& rNames, const char* pName );

        struct UniqueConstant
        {
            uint nStageOffset;  ///< Location in constant in resource set staging block
            uint nStageSize;    ///< Size of constant in resource set staging block
        };
           
        struct CBMovement
        {
            uint nStageOffset;  ///< Byte offset in staging block
            uint nSize;         ///< Number of bytes to copy
            uint nBufferIndex;  ///< Which constant buffer
            uint nBufferOffset; ///< Where in constant buffer
        };

        std::vector< uint > m_ConstantNames;
        std::vector<UniqueConstant> m_StagingLayout;
        std::vector<CBMovement> m_CBMovements;
        std::vector< uint > m_SamplerNames;
        std::vector< uint > m_SRVNames;
        std::vector< uint > m_CBSizes;
        std::vector< uint8 > m_BindIndices;

        uint m_nConstantStageBytes;

    };
}

#endif