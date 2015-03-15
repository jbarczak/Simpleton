
//=====================================================================================================================
//
//   DX11Texture.cpp
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

#include "PPMImage.h"
#include "DX11/DX11Texture.h"

#include "Texture.h"
#include "MiscMath.h"

namespace Simpleton
{
    static uint GetBytesPerPixel( DXGI_FORMAT eFormat )
    {
        
        switch( eFormat )
        {
        default: // TODO: Implement me!
            return 0;
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_TYPELESS:
            return 1;
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_TYPELESS:
            return 2;

        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return 4;
        }


    }

    bool DX11Texture::InitRaw( ID3D11Device* pDevice, DXGI_FORMAT eFormat, const void* pPixels, uint nWidth, uint nHeight, uint nDepth, uint nMips )
    {
        m_pTexture=0;
        m_pSRV=0;

        uint nBytesPerPixel=0;
        switch( eFormat )
        {
        default: // TODO: Implement me!
            return false;
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_TYPELESS:
            nBytesPerPixel = 1;
            break;
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_TYPELESS:
            nBytesPerPixel = 2;
            break;
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            nBytesPerPixel=4;
            break;
        }

        uint w = nWidth;
        uint h = nHeight;
        D3D11_SUBRESOURCE_DATA data[16];
        uint nByteOffset=0;
        const byte* pBytes = (const byte*)pPixels;
        for( uint i=0; i<nMips; i++ )
        {
            data[i].SysMemPitch = w*nBytesPerPixel;
            data[i].SysMemSlicePitch = w*h*nBytesPerPixel;
            data[i].pSysMem = pBytes;
            pBytes += data[i].SysMemSlicePitch;
            w  = MAX(1, w>>1);
            h = MAX(1, h>>1);
        }

        ID3D11Resource* pTexture=0;
        ID3D11ShaderResourceView* pSRV =0;
        if( nDepth > 1 )
        {
            D3D11_TEXTURE3D_DESC texture;
            texture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texture.CPUAccessFlags = 0;
            texture.Format = eFormat;
            texture.Height = nHeight;
            texture.Width = nWidth;
            texture.Depth = nDepth;
            texture.MipLevels = nMips;
            texture.MiscFlags = 0;
            texture.Usage = D3D11_USAGE_DEFAULT;
            
            D3D11_SHADER_RESOURCE_VIEW_DESC srv;
            srv.Format                    = texture.Format;
            srv.Texture3D.MipLevels       = nMips;
            srv.Texture3D.MostDetailedMip = 0;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                        
            HRESULT hr = pDevice->CreateTexture3D( &texture, data, (ID3D11Texture3D**) &pTexture );
            if( FAILED(hr))
                return false;

            hr = pDevice->CreateShaderResourceView( pTexture, &srv, &pSRV );        
            if( FAILED(hr) )
            {
                pTexture->Release();
                return false;
            }
        }
        else
        {
            D3D11_TEXTURE2D_DESC texture;
            texture.ArraySize = 1;
            texture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texture.CPUAccessFlags = 0;
            texture.Format = eFormat;
            texture.Height = nHeight;
            texture.Width = nWidth;
            texture.MipLevels = nMips;
            texture.MiscFlags = 0;
            texture.SampleDesc.Count = 1;
            texture.SampleDesc.Quality = 0;
            texture.Usage = D3D11_USAGE_DEFAULT;

            D3D11_SHADER_RESOURCE_VIEW_DESC srv;
            srv.Format                    = texture.Format;
            srv.Texture2D.MipLevels       = nMips;
            srv.Texture2D.MostDetailedMip = 0;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

            HRESULT hr = pDevice->CreateTexture2D( &texture, data, (ID3D11Texture2D**) &pTexture );
            if( FAILED(hr))
                return false;

            hr = pDevice->CreateShaderResourceView( pTexture, &srv, &pSRV );        
            if( FAILED(hr) )
            {
                pTexture->Release();
                return false;
            }
        }
        
        m_pTexture.Owns(pTexture);
        m_pSRV.Owns(pSRV);
        return true;
    }

    bool DX11Texture::InitFromPPMFile( ID3D11Device* pDevice, const char* pFile )
    {
        PPMImage img;
        if( !img.LoadFile(pFile) )
            return false;
        return InitFromImage( pDevice, img );
    }

    bool DX11Texture::InitFromImage( ID3D11Device* pDevice, const PPMImage& ppm )
    {
        m_pTexture=0;
        m_pSRV=0;

        uint nWidth       = ppm.GetWidth();
        uint nHeight      = ppm.GetHeight();
        uint nMips        = CountTextureMips( nWidth, nHeight, 1 );
        uint nTotalPixels = CountTexturePixels( nWidth, nHeight, 1, nMips );
        
        void* pScratch = malloc( nTotalPixels*4 );
        GenerateMips_RGB_To_RGBA( pScratch, ppm.GetRawBytes(), ppm.GetWidth(), ppm.GetHeight(), nMips );

        bool b = InitRaw( pDevice, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, pScratch, nWidth, nHeight, 1, nMips );
        free(pScratch);
        return b;
    }

    bool DX11Texture::InitFromImageWithAlpha( ID3D11Device* pDevice, const PPMImage& ppm, const PPMImage& alpha )
    {
        m_pTexture=0;
        m_pSRV=0;
        if( ppm.GetWidth() != alpha.GetWidth() || ppm.GetHeight() != alpha.GetHeight() )
            return false;

        uint nWidth       = ppm.GetWidth();
        uint nHeight      = ppm.GetHeight();
        uint nMips        = CountTextureMips( nWidth, nHeight, 1 );
        uint nTotalPixels = CountTexturePixels( nWidth, nHeight, 1, nMips );
        
        void* pScratch = malloc( nTotalPixels*4 );
        for( uint y=0; y<nHeight; y++ )
        {
            for( uint x=0; x<nWidth; x++ )
            {
                unsigned char rgb[3];
                unsigned char a[3];
                ppm.GetPixelBytes(x,y,rgb);
                alpha.GetPixelBytes(x,y,a);
                
                char* pRGBA = (char*)pScratch;
                pRGBA += 4*(y*nWidth+x);
                pRGBA[0] = rgb[0];
                pRGBA[1] = rgb[1];
                pRGBA[2] = rgb[2];
                pRGBA[3] = a[0];
            }
        }

        GenerateMips_RGBA_InPlace( pScratch, ppm.GetWidth(), ppm.GetHeight(), nMips );

        bool b = InitRaw( pDevice, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, pScratch, nWidth, nHeight, 1, nMips );
        free(pScratch);
        return b;
    }

    bool DX11Texture::InitTBuffer( ID3D11Device* pDevice, DXGI_FORMAT eFormat, const void* pData, uint nElementCount )
    {
        uint nSize = nElementCount*GetBytesPerPixel(eFormat);
        if( !nSize )
            return false;

        D3D11_BUFFER_DESC bd;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.CPUAccessFlags = 0;
        bd.ByteWidth = nSize;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        bd.Usage = D3D11_USAGE_DEFAULT;
        
        ID3D11Buffer* pBuffer = 0;
        ID3D11ShaderResourceView* pSRV=0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = pData;
        data.SysMemPitch = nSize;
        data.SysMemSlicePitch = 0;
        if( FAILED(pDevice->CreateBuffer( &bd, &data, &pBuffer )) )
            return false;

        D3D11_SHADER_RESOURCE_VIEW_DESC srv;
        srv.Buffer.FirstElement=0;
        srv.Buffer.NumElements = nElementCount;
        srv.Format = eFormat;
        srv.ViewDimension= D3D11_SRV_DIMENSION_BUFFER;
        if( FAILED(pDevice->CreateShaderResourceView( pBuffer, &srv, &pSRV )))
        {
            pBuffer->Release();
            return false;
        }

        m_pTexture.Owns(pBuffer);
        m_pSRV.Owns(pSRV);
        return true;
    }
 


    bool DX11ShadowMap::Init( uint nWidth, uint nHeight, uint nMips, ID3D11Device* pDev )
    {
        if( !nMips )
            nMips = CountTextureMips( nWidth, nHeight, 1 );

        D3D11_TEXTURE2D_DESC texture;
        texture.ArraySize = 1;
        texture.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_DEPTH_STENCIL;
        texture.CPUAccessFlags = 0;
        texture.Format = DXGI_FORMAT_R16_TYPELESS;
        texture.Height = nHeight;
        texture.Width = nWidth;
        texture.MipLevels = nMips;
        texture.MiscFlags = 0;
        texture.SampleDesc.Count = 1;
        texture.SampleDesc.Quality = 0;
        texture.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SHADER_RESOURCE_VIEW_DESC srv;
        srv.Texture2D.MipLevels = nMips;
        srv.Texture2D.MostDetailedMip = 0;
        srv.Format = DXGI_FORMAT_R16_UNORM;
        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        D3D11_DEPTH_STENCIL_VIEW_DESC dsv;
        dsv.Flags  = 0;
        dsv.Format = DXGI_FORMAT_D16_UNORM;
        dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsv.Texture2D.MipSlice = 0;

        ID3D11DepthStencilView*   pDSV=0;
        ID3D11ShaderResourceView* pSRV=0;
        ID3D11Texture2D* pResource=0;

        if( FAILED(pDev->CreateTexture2D( &texture, 0, &pResource )) )
            return false;
        
        if( FAILED(pDev->CreateDepthStencilView( pResource,&dsv, &pDSV)))
        {
            pResource->Release();
            return false;
        }
        if( FAILED(pDev->CreateShaderResourceView(pResource, &srv, &pSRV )))
        {
            pResource->Release();
            pDSV->Release();
            return false;
        }
        
        m_Viewport.Height = nHeight;
        m_Viewport.Width = nWidth;
        m_Viewport.TopLeftX=0;
        m_Viewport.TopLeftY=0;
        m_Viewport.MinDepth=0;
        m_Viewport.MaxDepth=1;
        m_Scissor.bottom = nHeight;
        m_Scissor.top=0;
        m_Scissor.left=0;
        m_Scissor.right=nWidth;

        m_pSRV.Owns(pSRV);
        m_pDSV.Owns(pDSV);
        m_pTexture.Owns(pResource);
        return true;
    }
}