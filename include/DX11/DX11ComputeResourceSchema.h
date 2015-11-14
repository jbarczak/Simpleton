//=====================================================================================================================
//
//   DX11ComputeResourceSchema.h
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

#ifndef _DX11_COMPUTE_RESOURCE_SCHEMA_H_
#define _DX11_COMPUTE_RESOURCE_SCHEMA_H_

#include "DX11/DX11ResourceSchema.h"

namespace Simpleton
{
    class DX11ComputeResourceSet;

    class DX11ComputeResourceSchema : public DX11ResourceSchema
    {
    public:

        bool Init( const void* pCS, size_t nLength );
        
        void CreateResourceSet( DX11ComputeResourceSet* pSet, ID3D11Device* pDevice ) const;


    };
}

#endif