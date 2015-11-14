
//=====================================================================================================================
//
//   DX11PipelineStateBuilder.cpp
//
//   Builds pipeline state objects and de-duplicates shaders and states
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_PIPELINE_STATE_BUILDER_H_
#define _DX11_PIPELINE_STATE_BUILDER_H_

#include <ComPtr.h>
#include <vector>

namespace Simpleton
{
    class DX11PipelineState;
    class DX11ComputeState;
    class DX11PipelineResourceSchema;

    class DX11PipelineStateBuilder
    {
    public:
        ~DX11PipelineStateBuilder( );
        
        void BeginState( ID3D11Device* pDevice );
        void SetVertexShader( const void* pBytecode, UINT nBytecodeLength );
        void SetGeometryShader( const void* pBytecode, UINT nBytecodeLength );
        void SetPixelShader( const void* pBytecode, UINT nBytecodeLength );
        void SetInputLayout( const D3D11_INPUT_ELEMENT_DESC* pInputLayout, uint nElements );
        void AppendInputElements( const D3D11_INPUT_ELEMENT_DESC* pInputLayout, uint nElements );
        void SetRasterizerState( const D3D11_RASTERIZER_DESC& state );
        void SetDepthStencilState( const D3D11_DEPTH_STENCIL_DESC& state );
        void SetBlendState( const D3D11_BLEND_DESC& state );
        void SetBlendState_ZOnly();
        void SetCullMode( D3D11_CULL_MODE eCull );
        void DisableZAndStencil();
        void SetupAlphaBlending();

        void EndState( DX11PipelineState* pState );

        
        void Destroy();

    private:
        
        ID3D11RasterizerState* GetRasterState();
        ID3D11DepthStencilState* GetDepthState();
        ID3D11BlendState* GetBlendState();
        ID3D11VertexShader* GetVertexShader();
        ID3D11GeometryShader* GetGeometryShader();
        ID3D11PixelShader* GetPixelShader();
        ID3D11InputLayout* GetInputLayout();
        DX11PipelineResourceSchema* GetResourceSchema( );

        template< class T >
        class Object
        {
        public:
            typedef T ThingType;
            ComPtr<T> pObject;
            uint nHash;
        };

        struct ResourceSchema
        {
            uint nVSCRC;
            uint nPSCRC;
            uint nGSCRC;
            uint nVertexBuffers;
            DX11PipelineResourceSchema* pSchema;
        };

        std::vector< Object<ID3D11VertexShader> >       m_VertexShaders;
        std::vector< Object<ID3D11PixelShader>  >       m_PixelShaders;
        std::vector< Object<ID3D11GeometryShader>  >    m_GeometryShaders;
        std::vector< Object<ID3D11RasterizerState>  >   m_RasterStates;
        std::vector< Object<ID3D11DepthStencilState>  > m_DepthStates;
        std::vector< Object<ID3D11BlendState>  >        m_BlendStates;
        std::vector< Object<ID3D11InputLayout>  >       m_InputLayouts;       
        std::vector< ResourceSchema > m_SchemII;

        ComPtr<ID3D11Device> m_pDevice;
        D3D11_RASTERIZER_DESC    m_RasterDesc;
        D3D11_DEPTH_STENCIL_DESC m_DSDesc;
        D3D11_BLEND_DESC         m_BlendDesc;
        const void* m_pVSCode;
        const void* m_pPSCode;
        const void* m_pGSCode;
        uint m_nVSLength;
        uint m_nPSLength;
        uint m_nGSLength;
        uint m_nInputElements;
        D3D11_INPUT_ELEMENT_DESC m_InputElements[D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT];
    };
}

#endif