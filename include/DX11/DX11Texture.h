//=====================================================================================================================
//
//   DX11Texture.h
//
//   Texture helper classes
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_TEXTURE_H_
#define _DX11_TEXTURE_H_

#include <d3d11.h>
#include "ComPtr.h"
#include "VectorMath.h"


typedef unsigned int uint;

namespace Simpleton
{
    class PPMImage;



    class DX11Texture
    {
    public:

        bool InitRaw( ID3D11Device* pDevice, DXGI_FORMAT eFormat, const void* pPixels, uint nWidth, uint nHeight, uint nDepth, uint nMips );
        bool InitFromPPMFile( ID3D11Device* pDevice, const char* pFile );
        bool InitFromImage( ID3D11Device* pDevice, const PPMImage& ppm );
        bool InitFromImageWithAlpha( ID3D11Device* pDevice, const PPMImage& ppm, const PPMImage& alpha );
        bool InitTBuffer( ID3D11Device* pDevice, DXGI_FORMAT eFormat, const void* pData, uint nElementCount );
        ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; };

    private:

        ComPtr<ID3D11ShaderResourceView> m_pSRV;
        ComPtr<ID3D11Resource> m_pTexture;
    };

    class DX11ShadowMap
    {
    public:

        enum Flags
        {
            HIGH_PRECISION,
            WITH_MIPS
        };

        bool Init( uint nWidth, uint nHeight, uint nMips, ID3D11Device* pDevice );

        ID3D11ShaderResourceView* GetSRV() { return m_pSRV; };
        ID3D11DepthStencilView* GetDSV() { return m_pDSV; };
        ID3D11Texture2D* GetTexture() { return m_pTexture; };
        const D3D11_VIEWPORT& GetViewport() const { return m_Viewport; };
        const D3D11_RECT& GetScissor() const { return m_Scissor; }
        Simpleton::Vec2f GetSize() const { return Vec2f(m_Viewport.Width, m_Viewport.Height); }
    private:
                
        ComPtr<ID3D11ShaderResourceView> m_pSRV;
        ComPtr<ID3D11DepthStencilView> m_pDSV;
        ComPtr<ID3D11Texture2D> m_pTexture;
        D3D11_VIEWPORT m_Viewport;
        D3D11_RECT m_Scissor;

    };
};


#endif