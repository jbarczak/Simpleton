//=====================================================================================================================
//
//   DX11Sampler.h
//
//   Sampler helper functions
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_SAMPLER_H_
#define _DX11_SAMPLER_H_

#include <d3d11.h>
#include "ComPtr.h"

namespace Simpleton
{
    ComPtr<ID3D11SamplerState> CreatePointSampler( ID3D11Device* pDevice );
    ComPtr<ID3D11SamplerState> CreateLinearSampler( ID3D11Device* pDevice );        
    ComPtr<ID3D11SamplerState> CreateLinearWrapSampler( ID3D11Device* pDevice );        
    ComPtr<ID3D11SamplerState> CreateAnisoSampler( ID3D11Device* pDevice );        
    
    ComPtr<ID3D11SamplerState> CreateBilinearComparisonSampler( ID3D11Device* pDevice );    
    ComPtr<ID3D11SamplerState> CreateTrilinearComparisonSampler( ID3D11Device* pDevice );
    ComPtr<ID3D11SamplerState> CreateAnisoComparisonSampler( ID3D11Device* pDevice );
        
}

#endif