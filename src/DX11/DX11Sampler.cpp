
//=====================================================================================================================
//
//   DX11Sampler.cpp
//
//   Sampler utility functions
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include <ComPtr.h>
namespace Simpleton
{
    ComPtr<ID3D11SamplerState> CreatePointSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sd.MinLOD = 0;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.MipLODBias = 0;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }

    ComPtr<ID3D11SamplerState> CreateLinearSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.MinLOD = 0;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.MipLODBias = 0;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }

    ComPtr<ID3D11SamplerState> CreateLinearWrapSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.MinLOD = 0;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.MipLODBias = 0;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }

    ComPtr<ID3D11SamplerState> CreateAnisoSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter   = D3D11_FILTER_ANISOTROPIC;;
        sd.MinLOD = 0;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.MipLODBias = 0;
        sd.MaxAnisotropy = 16;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }

    ComPtr<ID3D11SamplerState> CreateBilinearComparisonSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter   = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT ;
        sd.MinLOD = 0;
        sd.MaxLOD = 0;
        sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        sd.MipLODBias = 0;
        sd.MaxAnisotropy = 0;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }
      
    ComPtr<ID3D11SamplerState> CreateTrilinearComparisonSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter   = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        sd.MinLOD = 0;
        sd.MaxAnisotropy = 0;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        sd.MipLODBias = 0;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }
    
 

    ComPtr<ID3D11SamplerState> CreateAnisoComparisonSampler( ID3D11Device* pDevice )
    {
        D3D11_SAMPLER_DESC sd;
        memset(&sd,0,sizeof(sd));
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.Filter   = D3D11_FILTER_COMPARISON_ANISOTROPIC;
        sd.MinLOD = 0;
        sd.MaxAnisotropy = 16;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        sd.MipLODBias = 0;
        ComPtr<ID3D11SamplerState> pOut;
        pDevice->CreateSamplerState( &sd, (ID3D11SamplerState**)&pOut );
        return pOut;
    }

  
}