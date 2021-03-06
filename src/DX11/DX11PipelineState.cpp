//=====================================================================================================================
//
//   DX11PipelineState.cpp
//
//   Implementation of class: Simpleton::DX11PipelineState
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "Types.h"
#include "DX11/DX11PipelineState.h"
#include "DX11/DX11PipelineResourceSchema.h"

namespace Simpleton
{

    
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
    //=====================================================================================================================
    void DX11PipelineState::Apply( ID3D11DeviceContext* pContext )
    {
        float m_BlendFactor[4] = {0,0,0,0};
        pContext->IASetInputLayout( m_pIL );
        pContext->VSSetShader(m_pVS,0,0);
        pContext->GSSetShader(m_pGS,0,0);
        pContext->PSSetShader(m_pPS,0,0);
        pContext->RSSetState(m_pRaster);
        pContext->OMSetDepthStencilState(m_pDepthStencil, 0);
        pContext->OMSetBlendState(m_pBlend,m_BlendFactor, 0xff );
    }
    
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
}

