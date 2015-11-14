//=====================================================================================================================
//
//   DX11ComputeResourceSet.h
//
//   Implementation of class: Simpleton::DX11ComputeResourceSet
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_COMPUTE_RESOURCE_SSET_H_
#define _DX11_COMPUTE_RESOURCE_SSET_H_

#include "DX11/DX11ResourceSet.h"
#include "DX11/DX11ComputeResourceSchema.h"

struct ID3D11DeviceContext;

namespace Simpleton
{
    class DX11ComputeResourceSchema;

    class DX11ComputeResourceSet : public DX11ResourceSet
    {
    public:

        void Apply( ID3D11DeviceContext* pCtx );

        const DX11ComputeResourceSchema* GetSchema() const { 
            return static_cast<const DX11ComputeResourceSchema*>( m_pSchema );
        }

    private:

    };
}

#endif