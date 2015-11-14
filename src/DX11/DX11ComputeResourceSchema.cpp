//=====================================================================================================================
//
//   DX11ComputeResourceSchema.cpp
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

#include "DX11/DX11ComputeResourceSchema.h"
#include "DX11/DX11ComputeResourceSet.h"

#include <d3dcompiler.h>

namespace Simpleton
{

    bool DX11ComputeResourceSchema::Init( const void* pCS, size_t nLength )
    {
        ID3D11ShaderReflection* pRef;
           
        HRESULT hr = D3DReflect( pCS, nLength, __uuidof(ID3D11ShaderReflection), (void**)&pRef );
        if( !SUCCEEDED(hr) )
            return false;

        DX11ResourceSchema::BuildSchema( &pRef, 1 );
        return true;
    }
        
    void DX11ComputeResourceSchema::CreateResourceSet( DX11ComputeResourceSet* pSet, ID3D11Device* pDevice ) const
    {
        DX11ResourceSchema::CreateResourceSet( pSet, pDevice );
    }

}