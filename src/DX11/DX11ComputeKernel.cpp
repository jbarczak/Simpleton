//=====================================================================================================================
//
//   DX11ComputeKernel.cpp
//
//   Implementation of class: Simpleton::DX11ComputeKernel
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "DX11/DX11ComputeKernel.h"

namespace Simpleton
{
    bool DX11ComputeKernel::Init( const void* pCSCode, size_t nCodeSize, ID3D11Device* pDevice )
    {
        HRESULT hr = pDevice->CreateComputeShader( pCSCode, nCodeSize, 0, m_pShader.Address() );
        if( !SUCCEEDED(hr) )
            return false;

        if( !m_Schema.Init( pCSCode, nCodeSize ) )
        {
            m_pShader=0;
            return false;
        }

        return true;
    }

    void DX11ComputeKernel::Bind( ID3D11DeviceContext* pDevice )
    {
        pDevice->CSSetShader( m_pShader, 0, 0 );
    }
}