
//=====================================================================================================================
//
//   DX11PipelineStateBuilder.cpp
//
//   Builds pipeline state objects and caches shaders
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <d3d11.h>
#include <d3dcompiler.h>
#include "Types.h"
#include "DX11/DX11PipelineStateBuilder.h"
#include "DX11/DX11PipelineState.h"
#include "DX11/DX11PipelineResourceSchema.h"

#include "CRC.h"
#include "MiscMath.h"

#include <assert.h>

namespace Simpleton
{
    //=====================================================================================================================
    //=====================================================================================================================    
    DX11PipelineStateBuilder::~DX11PipelineStateBuilder()
    {
        Destroy();
    }

    //=====================================================================================================================
    //=====================================================================================================================       
    void DX11PipelineStateBuilder::Destroy()
    {
        for( auto& it : m_SchemII )
            it.pSchema->Destroy();        
        m_SchemII.clear();
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11PipelineStateBuilder::BeginState( ID3D11Device* pDevice )
    {
        memset(&m_RasterDesc, 0, sizeof(m_RasterDesc));
        memset(&m_DSDesc,     0, sizeof(m_DSDesc));
        memset(&m_BlendDesc,  0, sizeof(m_BlendDesc));

        m_RasterDesc.AntialiasedLineEnable = false;
        m_RasterDesc.CullMode = D3D11_CULL_BACK;
        m_RasterDesc.DepthBias = 0;
        m_RasterDesc.DepthBiasClamp = 0;
        m_RasterDesc.SlopeScaledDepthBias = 0;
        m_RasterDesc.FillMode = D3D11_FILL_SOLID;
        m_RasterDesc.FrontCounterClockwise = false;
        m_RasterDesc.MultisampleEnable = false;
        m_RasterDesc.ScissorEnable = true;

        m_DSDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        m_DSDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        m_DSDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        m_DSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        m_DSDesc.FrontFace = m_DSDesc.BackFace;
        m_DSDesc.DepthEnable = true;
        m_DSDesc.DepthFunc = D3D11_COMPARISON_LESS;
        m_DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        m_DSDesc.StencilEnable = false;
        m_DSDesc.StencilReadMask = 0xff;
        m_DSDesc.StencilWriteMask = 0;

        m_BlendDesc.AlphaToCoverageEnable = false;
        m_BlendDesc.IndependentBlendEnable = false;
        m_BlendDesc.RenderTarget[0].BlendEnable = false;
        m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        m_BlendDesc.RenderTarget[0].DestBlendAlpha  = D3D11_BLEND_ZERO;
        m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = 0xf;
        for( uint i=1; i<D3D11_PS_OUTPUT_REGISTER_COUNT; i++ )
            m_BlendDesc.RenderTarget[i] = m_BlendDesc.RenderTarget[0];

        m_pVSCode=0;
        m_nVSLength=0;
        m_pPSCode=0;
        m_nPSLength=0;
        m_pGSCode=0;
        m_nGSLength=0;
        m_nInputElements=0;

        m_pDevice = pDevice;
    }

    void DX11PipelineStateBuilder::SetVertexShader( const void* pBytecode, uint nBytecodeLength )
    {
        m_pVSCode = pBytecode;
        m_nVSLength = nBytecodeLength;
    }

    void DX11PipelineStateBuilder::SetInputLayout( const D3D11_INPUT_ELEMENT_DESC* pInputLayout, uint nVertexElements )
    {
        m_nInputElements = nVertexElements;
        for( uint i=0; i<nVertexElements; i++ )
            m_InputElements[i] = pInputLayout[i];
    }
    void DX11PipelineStateBuilder::AppendInputElements( const D3D11_INPUT_ELEMENT_DESC* pIL, uint nElems )
    {
        for( uint i=0; i<nElems; i++ )
            m_InputElements[i+m_nInputElements] = pIL[i];
        m_nInputElements += nElems;
    }
    void DX11PipelineStateBuilder::SetGeometryShader( const void* pBytecode, uint nBytecodeLength )
    {
        m_pGSCode = pBytecode;
        m_nGSLength = nBytecodeLength;
    }

    void DX11PipelineStateBuilder::SetPixelShader( const void* pBytecode, uint nBytecodeLength )
    {
        m_pPSCode = pBytecode;
        m_nPSLength = nBytecodeLength;
    }

    void DX11PipelineStateBuilder::SetRasterizerState( const D3D11_RASTERIZER_DESC& state )
    {
        m_RasterDesc = state;
    }

    void DX11PipelineStateBuilder::SetDepthStencilState( const D3D11_DEPTH_STENCIL_DESC& state)
    {
        m_DSDesc = state;
    }

    void DX11PipelineStateBuilder::SetBlendState( const D3D11_BLEND_DESC& state )
    {
        m_BlendDesc = state;
    }

    //=====================================================================================================================
    //=====================================================================================================================    
    void DX11PipelineStateBuilder::SetBlendState_ZOnly()
    {
        D3D11_BLEND_DESC zonly;
        zonly.AlphaToCoverageEnable                 = false;
        zonly.IndependentBlendEnable                = false;
        zonly.RenderTarget[0].BlendEnable           = false;
        zonly.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO;
        zonly.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
        zonly.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
        zonly.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
        zonly.RenderTarget[0].RenderTargetWriteMask = 0;
        SetBlendState(zonly);
    }

    //=====================================================================================================================
    //=====================================================================================================================    
    void DX11PipelineStateBuilder::SetCullMode( D3D11_CULL_MODE eCull )
    {
        m_RasterDesc.CullMode = eCull;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11PipelineStateBuilder::DisableZAndStencil()
    {
        D3D11_DEPTH_STENCIL_DESC ds;
        ds.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        ds.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        ds.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds.FrontFace = ds.BackFace;
        ds.DepthEnable = false;
        ds.DepthFunc = D3D11_COMPARISON_ALWAYS;
        ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        ds.StencilEnable = false;
        ds.StencilReadMask = 0;
        ds.StencilWriteMask = 0;
        m_DSDesc = ds;
    }

    //=====================================================================================================================
    //=====================================================================================================================    
    void DX11PipelineStateBuilder::SetupAlphaBlending( )
    {
        D3D11_BLEND_DESC bd;
        bd.AlphaToCoverageEnable = false;
        bd.IndependentBlendEnable = false;
        bd.RenderTarget[0].BlendEnable = true;
        bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bd.RenderTarget[0].RenderTargetWriteMask= 0xf;
        bd.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
        bd.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_DEST_ALPHA;
        bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        bd.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
        m_BlendDesc = bd;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    void DX11PipelineStateBuilder::EndState( DX11PipelineState* pState )
    {
        pState->m_pRaster            = GetRasterState();
        pState->m_pDepthStencil      = GetDepthState();
        pState->m_pBlend             = GetBlendState();
        pState->m_pVS                = GetVertexShader();
        pState->m_pPS                = GetPixelShader();
        pState->m_pGS                = GetGeometryShader();
        pState->m_pIL                = GetInputLayout();
        pState->m_pResourceSchema    = GetResourceSchema();
        m_pDevice=0;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11RasterizerState* DX11PipelineStateBuilder::GetRasterState()
    {
        uint32 nRSHash    = crcOf(m_RasterDesc);
        for( auto& it : m_RasterStates )
            if( it.nHash == nRSHash )
                return it.pObject;
        
        ID3D11RasterizerState* pRS=0;
        m_pDevice->CreateRasterizerState( &m_RasterDesc, &pRS );
        m_RasterStates.emplace_back();
        m_RasterStates.back().nHash = nRSHash;
        m_RasterStates.back().pObject.Owns(pRS);
        return pRS;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11DepthStencilState* DX11PipelineStateBuilder::GetDepthState()
    {
        uint32 nDSHash    = crcOf(m_DSDesc);
        for( auto& it : m_DepthStates )
            if( it.nHash == nDSHash )
                return it.pObject;
        
        ID3D11DepthStencilState* pDS=0;
        m_pDevice->CreateDepthStencilState( &m_DSDesc, &pDS );
        m_DepthStates.emplace_back();
        m_DepthStates.back().nHash = nDSHash;
        m_DepthStates.back().pObject.Owns(pDS);
        return pDS;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11BlendState* DX11PipelineStateBuilder::GetBlendState()
    {
        uint32 nBSHash    = crcOf(m_BlendDesc);
        for( auto& it : m_BlendStates )
            if( it.nHash == nBSHash )
                return it.pObject;
        
        ID3D11BlendState* pBS=0;
        m_pDevice->CreateBlendState( &m_BlendDesc, &pBS );
        m_BlendStates.emplace_back();
        m_BlendStates.back().nHash = nBSHash;
        m_BlendStates.back().pObject.Owns(pBS);
        return pBS;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11VertexShader* DX11PipelineStateBuilder::GetVertexShader()
    {
        assert( m_pVSCode && m_nVSLength );
        uint32 nVSHash = crc32(m_pVSCode,m_nVSLength);
        for( auto& it : m_VertexShaders )
            if( it.nHash == nVSHash )
                return it.pObject;

        ID3D11VertexShader* pVS=0;
        m_pDevice->CreateVertexShader( m_pVSCode, m_nVSLength,0,&pVS);
        m_VertexShaders.emplace_back();
        m_VertexShaders.back().nHash = nVSHash;
        m_VertexShaders.back().pObject.Owns(pVS);
        return pVS;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11GeometryShader* DX11PipelineStateBuilder::GetGeometryShader()
    {
        if( !m_pGSCode || !m_nGSLength )
            return 0;

        uint32 nPSHash = crc32(m_pGSCode,m_nGSLength);
        for( auto& it : m_GeometryShaders )
            if( it.nHash == nPSHash )
                return it.pObject;

        ID3D11GeometryShader* pGS=0;
        m_pDevice->CreateGeometryShader( m_pGSCode, m_nGSLength,0,&pGS);
        m_GeometryShaders.emplace_back();
        m_GeometryShaders.back().nHash = nPSHash;
        m_GeometryShaders.back().pObject.Owns(pGS);
        return pGS;
    }
    
    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11PixelShader* DX11PipelineStateBuilder::GetPixelShader()
    {
        if( !m_pPSCode || !m_nPSLength )
            return 0;

        uint32 nPSHash = crc32(m_pPSCode,m_nPSLength);
        for( auto& it : m_PixelShaders )
            if( it.nHash == nPSHash )
                return it.pObject;

        ID3D11PixelShader* pPS=0;
        m_pDevice->CreatePixelShader( m_pPSCode, m_nPSLength,0,&pPS);
        m_PixelShaders.emplace_back();
        m_PixelShaders.back().nHash = nPSHash;
        m_PixelShaders.back().pObject.Owns(pPS);
        return pPS;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    ID3D11InputLayout* DX11PipelineStateBuilder::GetInputLayout()
    {
        uint nILHash = crc32( m_InputElements, sizeof(m_InputElements[0])*m_nInputElements );
        
        ID3DBlob* pBlob = 0;
        D3DGetBlobPart( m_pVSCode, m_nVSLength, D3D_BLOB_INPUT_SIGNATURE_BLOB, 0, &pBlob );
        nILHash = crc32( nILHash, pBlob->GetBufferPointer(), pBlob->GetBufferSize() );
        uint nSize = pBlob->GetBufferSize();

        for( auto& it : m_InputLayouts )
            if( it.nHash == nILHash )
                return it.pObject;

        ID3D11InputLayout* pIL=0;
        m_pDevice->CreateInputLayout( m_InputElements, m_nInputElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pIL );
        m_InputLayouts.emplace_back();
        m_InputLayouts.back().nHash = nILHash;
        m_InputLayouts.back().pObject.Owns(pIL);
        return pIL;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    DX11PipelineResourceSchema* DX11PipelineStateBuilder::GetResourceSchema( )
    {
        uint nVSCRC = crc32( m_pVSCode, m_nVSLength );
        uint nPSCRC = crc32( m_pPSCode, m_nPSLength );
        uint nGSCRC = crc32( m_pGSCode, m_nGSLength );

        // count vertex buffers
        uint nVBs=0;
        for( uint i=0; i<m_nInputElements; i++ )
            nVBs = MAX(nVBs,m_InputElements[i].InputSlot+1 );

        // find existing
        for( auto& it : m_SchemII )
            if( it.nVSCRC == nVSCRC && it.nPSCRC == nPSCRC && it.nVertexBuffers == nVBs && it.nGSCRC == nGSCRC )
                return it.pSchema;
     
        // create new if needed
        DX11PipelineResourceSchema* pSchema = DX11PipelineResourceSchema::Create(m_pVSCode,m_pGSCode,m_pPSCode,m_nVSLength, m_nGSLength, m_nPSLength,nVBs);
        m_SchemII.emplace_back();
        m_SchemII.back().nVertexBuffers = nVBs;
        m_SchemII.back().nPSCRC = nPSCRC;
        m_SchemII.back().nVSCRC = nVSCRC;
        m_SchemII.back().nGSCRC = nGSCRC;
        m_SchemII.back().pSchema = pSchema;
        return pSchema;
    }

}
