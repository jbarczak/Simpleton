//=====================================================================================================================
//
//   DX11ResourceSchema.cpp
//
//   Implementation of class: Simpleton::DX11ResourceSchema
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include <d3dcompiler.h>
#include <algorithm>

#include "CRC.h"
#include "MiscMath.h"
#include "DX11/DX11ResourceSchema.h"
#include "DX11/DX11ResourceSet.h"

namespace Simpleton
{

    
    struct ConstantBuffer
    {
        uint nShaderStage;
        uint nSlotIndex;
        uint nBufferSize;
        uint nFirstVar;
        uint nVarCount;

        uint nName; 
    };

    struct VariableRef
    {
        uint nShaderStage;
        uint nCBufferIndex;
        uint nCBufferOffset;
        uint nVariableSize;
        uint nName;
        uint nStageOffset;
    };
    struct TextureRef
    {
        uint nName;
        uint nShaderStage;
        uint nSlotIndex;
    };
    struct SamplerRef
    {
        uint nName;
        uint nShaderStage;
        uint nSlotIndex;
    };



    //=====================================================================================================================
    static void ParseShaderReflection( ID3D11ShaderReflection* pReflection, 
                                       uint nStage,
                                       std::vector<VariableRef>& vars,
                                       std::vector<TextureRef>& textures,
                                       std::vector<SamplerRef>& samplers,
                                       std::vector<ConstantBuffer>& cbs )                         
    {
        D3D11_SHADER_DESC desc;
        pReflection->GetDesc(&desc);
       
        // yank constant buffers
        for( uint i=0; i<desc.ConstantBuffers; i++ )
        {
            D3D11_SHADER_BUFFER_DESC desc;
            ID3D11ShaderReflectionConstantBuffer* pCB = pReflection->GetConstantBufferByIndex(0) ;
            pCB->GetDesc(&desc);
            
            D3D11_SHADER_INPUT_BIND_DESC bind;
            pReflection->GetResourceBindingDescByName( desc.Name, &bind );

            ConstantBuffer cb;
            cb.nShaderStage      = nStage;
            cb.nSlotIndex  = bind.BindPoint;
            cb.nBufferSize = (desc.Size + 15) & ~15;
            cbs.push_back(cb);

            // yank constant buffer variables
            for( uint v=0; v<desc.Variables; v++ )
            {
                ID3D11ShaderReflectionVariable* pVar = pCB->GetVariableByIndex(v);
                D3D11_SHADER_VARIABLE_DESC desc;
                pVar->GetDesc(&desc);

                VariableRef var;
                var.nShaderStage = nStage;
                var.nCBufferIndex = cbs.size()-1;
                var.nCBufferOffset = desc.StartOffset;
                var.nVariableSize = desc.Size;
                var.nName = crcOfString(desc.Name);
                vars.push_back(var);
            }

        }
        
        // yank sampler, texture bindings
        for( uint i=0; i<desc.BoundResources; i++ )
        {
            D3D11_SHADER_INPUT_BIND_DESC inputDesc;
            pReflection->GetResourceBindingDesc(i, &inputDesc );
            switch( inputDesc.Type )
            {
            case D3D_SIT_SAMPLER:
                {
                    SamplerRef ref;
                    ref.nName = crcOfString( inputDesc.Name );
                    ref.nSlotIndex = inputDesc.BindPoint;
                    ref.nShaderStage = nStage;
                    samplers.push_back(ref);
                }
                break;

            case D3D_SIT_TBUFFER:
            case D3D_SIT_TEXTURE:
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
                {
                    TextureRef ref;
                    ref.nName = crcOfString(inputDesc.Name);
                    ref.nSlotIndex = inputDesc.BindPoint;
                    ref.nShaderStage = nStage;
                    textures.push_back(ref);
                }
                break;
            }
        }      
    }

    template< class T >
    static void SortByName( std::vector<T>& Things )
    {
        std::sort( Things.begin(), Things.end(), []( const T& a, const T& b ) { return a.nName < b.nName; } );
    }
        
    template< class T >
    static void SortByStageThenSlot( std::vector<T>& Things )
    {
        std::sort( Things.begin(), Things.end(), 
                  []( const T& a, const T& b ) 
                    { 
                        if(a.nShaderStage == b.nShaderStage)
                            return a.nSlotIndex < b.nSlotIndex;
                        return a.nShaderStage < b.nShaderStage;
                    } );
    }

    //=====================================================================================================================    
    template< class T >
    static void BuildNameArray( std::vector<uint>& Names, std::vector<T>& Things )
    {
 
        uint nNames=0;
        for( uint i=0; i<Things.size(); )
        {
            uint i0=i;
            uint nName=Things[i].nName;

            // skip run of things with same name
            do
            {
                i++;
            } while( i < Things.size() && Things[i].nName == nName );
            
            while( i0 < i ) // replace hash with unique name index
                Things[i0++].nName = nNames;

            // store unique name
            Names.push_back( nName );
            nNames++;
        }
    }


    //=====================================================================================================================    
    template< class T> 
    static void BuildBindArray( std::vector<uint8>& Indices, std::vector<T>& Things )
    {

        for( uint i=0; i<Things.size(); )
        {
            uint nStage = Things[i].nShaderStage;
            uint nSlot=0; // reset slot counter between stages
            do
            {
                // If there are holes in the slot list, fill them with copies of the same bind name
                do
                {
                    Indices.push_back(Things[i].nName);
                    nSlot++;
                }while( nSlot < Things[i].nSlotIndex );
                
                nSlot = Things[i].nSlotIndex;
                i++; // iterate over run of things in the same stage

            } while( i < Things.size() && Things[i].nShaderStage == nStage );
        }
    }


    //=====================================================================================================================    
    static void SortByBufferLocation( std::vector<VariableRef>& vars )
    {
        // sort variables by buffer number and offset
        std::sort( vars.begin(), vars.end(),
                  []( const VariableRef& a, const VariableRef& b )
                    {
                        if( a.nCBufferIndex == b.nCBufferIndex )
                            return a.nCBufferOffset < b.nCBufferOffset;
                        return a.nCBufferIndex < b.nCBufferIndex;
                    }
                 ); 
    }

    //=====================================================================================================================    
    static void FuseConstantBuffers( std::vector<ConstantBuffer>& cbs, std::vector<VariableRef>& vars )
    {
       

        // find variable ranges for each constant buffer
        for( uint v=0; v<vars.size(); )
        {
            uint v0 = v;
            do
            {
                v++;
            } while( v < vars.size() && vars[v].nCBufferIndex == vars[v0].nCBufferIndex );
            cbs[vars[v0].nCBufferIndex].nFirstVar = v0;
            cbs[vars[v0].nCBufferIndex].nVarCount = v-v0;
        }

        // give each CB an initial name
        for( uint i=0; i<cbs.size(); i++ )
            cbs[i].nName = i;

        // Loop over all pairs of CBs and attempt merging
        for( uint i=0; i<cbs.size(); i++ )
        {
            if( cbs[i].nName != i )
                continue; // already merged with something else

            uint nVars = cbs[i].nVarCount;
            for( uint j=i+1; j<cbs.size(); j++ )
            {
                if( cbs[j].nVarCount != nVars )
                    continue; // mismatched variable layouts
                if( cbs[j].nName != j )
                    continue; // already merged with something else

                // two CBs can be combined if each uses the exact same variables at the exact same offsets
                uint v=0;
                while( v < nVars )
                {
                    uint vi = cbs[i].nFirstVar+v;
                    uint vj = cbs[j].nFirstVar+v;
                    if( vars[vi].nName != vars[vj].nName || vars[vi].nCBufferOffset != vars[vj].nCBufferOffset )
                        break;
                    v++;
                }

                if( v == nVars )
                    cbs[j].nName = cbs[i].nName;
            }
        }

        // flatten CB indices in variables so their CBufferIndex fields equal the unique name of the corresponding CB
        for( uint v=0; v<vars.size(); v++ )
        {
            uint n = vars[v].nCBufferIndex;
            while( cbs[n].nName != n )
                n = cbs[n].nName;
            vars[v].nCBufferIndex = n;
        }
    }
     
    //=====================================================================================================================    
    size_t DX11ResourceSchema::CountSamplers( ID3D11ShaderReflection* pReflection )
    {
        if( !pReflection )
            return 0;

        
        D3D11_SHADER_DESC desc;
        pReflection->GetDesc(&desc);

        size_t n=0;
        for( uint i=0; i<desc.BoundResources; i++ )
        {
            D3D11_SHADER_INPUT_BIND_DESC inputDesc;
            pReflection->GetResourceBindingDesc(i, &inputDesc );
            switch( inputDesc.Type )
            {
            case D3D_SIT_SAMPLER:
                {
                    n++;
                }
                break;
            }
        }      

        return n;
    }

    //=====================================================================================================================    
    size_t DX11ResourceSchema::CountTextures( ID3D11ShaderReflection* pReflection )
    {
        if( !pReflection )
            return 0;

        D3D11_SHADER_DESC desc;
        pReflection->GetDesc(&desc);

         // yank sampler, texture bindings
        size_t n=0;
        for( uint i=0; i<desc.BoundResources; i++ )
        {
            D3D11_SHADER_INPUT_BIND_DESC inputDesc;
            pReflection->GetResourceBindingDesc(i, &inputDesc );
            switch( inputDesc.Type )
            {
            case D3D_SIT_TBUFFER:
            case D3D_SIT_TEXTURE:
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
                {
                    n++;
                }
                break;
            }
        }      

        return n;
    }

    //=====================================================================================================================    
    size_t DX11ResourceSchema::CountCBs( ID3D11ShaderReflection* pReflection )
    {
        if( !pReflection )
            return 0;

        D3D11_SHADER_DESC desc;
        pReflection->GetDesc(&desc);
        return desc.ConstantBuffers;
    }
        
   
  
    
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
    //
    //           Protected Methods
    //
    //=====================================================================================================================
    
    //=====================================================================================================================
    //
    //            Private Methods
    //
    //=====================================================================================================================

    
    //=====================================================================================================================    
    uint DX11ResourceSchema::NameLookup( const std::vector<uint>& rNames, const char* pName )
    {
        uint n = crcOfString(pName);
        uint nNames = rNames.size();
        for( uint i=0; i<nNames; i++ )
            if( rNames[i] == n ) 
                return i;
        return nNames;
    }

    //=====================================================================================================================    
    void DX11ResourceSchema::BuildSchema( ID3D11ShaderReflection** ppReflections, size_t nReflections )
    {
        std::vector<VariableRef> vars;
        std::vector<SamplerRef> samplers;
        std::vector<TextureRef> textures;
        std::vector<ConstantBuffer> buffers;

        for( size_t i=0; i<nReflections; i++ )
        {
            if( ppReflections[i] )
                ParseShaderReflection(ppReflections[i], i, vars, textures, samplers, buffers );
        }


        // extract unique names
        SortByName(samplers);
        SortByName(textures);
        SortByName(vars);
        BuildNameArray( m_SamplerNames,  samplers );
        BuildNameArray( m_SRVNames,      textures );
        BuildNameArray( m_ConstantNames, vars );

        // Layout the constant staging block.  Each unique named constant gets an allocation in the staging block
        m_nConstantStageBytes=0;
        for( uint i=0; i<vars.size(); )
        {
            uint i0=i;

            // different shader stages could have different notions of the size of a given variable 
            //  (e.g. one declares float3, the other float4). 
            //  Deal with this by choosing the largest
            uint nSize=0; 
            do
            {
                nSize = MAX( nSize, vars[i].nVariableSize );
                i++;
            } while( i < vars.size() && vars[i].nName == vars[i0].nName );

            // store this variable's staging buffer offset in each of the 'ref' structures
            uint nOffset = m_nConstantStageBytes;
            while( i0 < i )
                vars[i0++].nStageOffset = nOffset;

            UniqueConstant constant;
            constant.nStageOffset = nOffset;
            constant.nStageSize   = nSize;
            m_StagingLayout.push_back(constant);
            m_nConstantStageBytes += nSize;
        }

        // one more constant so that out-of-range names can be returned for 'not-found'
        UniqueConstant dummy;
        dummy.nStageOffset = 0;
        dummy.nStageSize=0;
        m_StagingLayout.push_back( dummy );

        // collapse constant buffers across stages whose layouts are identical
        SortByBufferLocation( vars );
        FuseConstantBuffers( buffers, vars );

        for( uint i=0; i<buffers.size(); i++ )
        {
            if( buffers[i].nName != i )
                continue; // duplicate

            // store sizes of unique CBs
            m_CBSizes.push_back( buffers[i].nBufferSize );
        
            // build the 'CB movement' set.  There is one CB movement per variable per constant buffer
            for( uint v=0; v<buffers[i].nVarCount; v++ )
            {
                const VariableRef* pVar = &vars[buffers[i].nFirstVar+v];
                CBMovement m;
                m.nBufferIndex  = buffers[i].nName;
                m.nBufferOffset = pVar->nCBufferOffset;
                m.nStageOffset  = pVar->nStageOffset;
                m.nSize         = pVar->nVariableSize;
                m_CBMovements.push_back(m);
            }
        }
       
        // build bind index arrays for samplers/SRVs/CBs
        //  This is an array which contains the name of the resource to be bound to each stage's bind slots
        SortByStageThenSlot( samplers );
        SortByStageThenSlot( textures );
        SortByStageThenSlot( buffers );
        BuildBindArray( m_BindIndices, samplers );
        BuildBindArray( m_BindIndices, textures );
        BuildBindArray( m_BindIndices, buffers );
    }

    //=====================================================================================================================    
    void DX11ResourceSchema::CreateResourceSet( DX11ResourceSet* pSet, ID3D11Device* pDevice ) const
    {
        uint nSamplerPtrs = (m_SamplerNames.size()+1);
        uint nSRVPtrs = (m_SRVNames.size()+1); // add one for 'not-found'
        uint nCBPtrs = m_CBSizes.size();
        
        uint nAllocationSize = 0;
        nAllocationSize += nCBPtrs*sizeof(ID3D11Buffer*);
        nAllocationSize += nSamplerPtrs*sizeof(ID3D11SamplerState*);
        nAllocationSize += nSRVPtrs*sizeof(ID3D11ShaderResourceView*);
        nAllocationSize += m_nConstantStageBytes;

        uint8* pAlloc = (uint8*)malloc(nAllocationSize);
        memset( pAlloc,0,nAllocationSize );

        pSet->m_pSamplersByName  = reinterpret_cast<ID3D11SamplerState**>(pAlloc);
        pSet->m_pSRVsByName      = reinterpret_cast<ID3D11ShaderResourceView**>(pSet->m_pSamplersByName+nSamplerPtrs );
        pSet->m_pConstantBuffers = reinterpret_cast<ID3D11Buffer**>(pSet->m_pSRVsByName + nSRVPtrs );
        pSet->m_pConstantStaging = reinterpret_cast<uint8*>(pSet->m_pConstantBuffers + nCBPtrs );

        // create a set of CBs
        D3D11_BUFFER_DESC cb;
        cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cb.MiscFlags = 0;
        cb.StructureByteStride = 0;
        cb.Usage = D3D11_USAGE_DYNAMIC;
        for( uint i=0; i<nCBPtrs; i++ )
        {
            cb.ByteWidth = m_CBSizes[i];
            pDevice->CreateBuffer( &cb,0, &pSet->m_pConstantBuffers[i] );
        }

        pSet->m_nCBMovements     = m_CBMovements.size();
        pSet->m_nUniqueCBs       = m_CBSizes.size();
        pSet->m_pConstantsByName = m_StagingLayout.data();
        pSet->m_pCBMovements     = m_CBMovements.data();
        pSet->m_pBindIndices     = m_BindIndices.data();
        pSet->m_pSchema = this;
    }

}

