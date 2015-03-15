
//=====================================================================================================================
//
//   DX11PipelineState.h
//
//   Helper class which encapsulates all of the various pipeline states for DX11
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include "ComPtr.h"

namespace Simpleton
{
    class DX11PipelineResourceSchema;
    class DX11PipelineResourceSet;

    //=====================================================================================================================
    //=====================================================================================================================
    class DX11PipelineState
    {
    public:

        void Apply( ID3D11DeviceContext* pContext );
        
        const DX11PipelineResourceSchema* GetResourceSchema() const { return m_pResourceSchema; }
        
    private:

        friend class DX11PipelineStateBuilder;

        ComPtr<ID3D11InputLayout>          m_pIL;
        ComPtr<ID3D11VertexShader>         m_pVS;
        ComPtr<ID3D11GeometryShader>       m_pGS;
        ComPtr<ID3D11PixelShader>          m_pPS;
        ComPtr<ID3D11RasterizerState>      m_pRaster;
        ComPtr<ID3D11DepthStencilState>    m_pDepthStencil;
        ComPtr<ID3D11BlendState>           m_pBlend;
        const DX11PipelineResourceSchema*  m_pResourceSchema;
    };


}