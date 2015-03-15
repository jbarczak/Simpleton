
#include "Simpleton.h"
using namespace Simpleton;


class GDITester : public GDIWindowController
{
public:

    virtual void OnPaint( GDIWindow* pWindow, WindowPainter* pPainter ) 
    {
        uint nWidth;
        uint nHeight;
        pPainter->GetClientSize(&nWidth, &nHeight);

        int x0;
        int y0;
        int width;
        int height;
        pPainter->GetPaintRect(&y0,&x0,&width,&height);

        unsigned char* pBytes = (unsigned char*) malloc( width*height*4 );
        unsigned char* pBuffer = pBytes;
        for( int y=0; y<height; y++ )
        {
            for( int x=0; x<width; x++ )
            {
                float r = (float)(x0+x) / (nWidth);
                float g = (float)(y0+y) / (nHeight);
                pBytes[0] = 0;
                pBytes[1] = 255*g;
                pBytes[2] = 255*r;
                pBytes[3] = 255;
                pBytes += 4;
            }
        }

        WindowPainter::BlitParameters blit;
        blit.pBits = pBuffer;
        blit.nLeft = x0;
        blit.nTop = y0;
        blit.nWidth = width;
        blit.nHeight = height;
        blit.nSrcTop = 0;
        blit.nSrcLeft = 0;
        blit.nSrcWidth = width;
        blit.nSrcHeight = height;
        blit.nSrcImageHeight = height;
        blit.nSrcImageWidth = width;
        pPainter->BlitBGRAPixels( blit );

        free(pBuffer);
    }

};

#include "DX11/SimpletonDX11.h"
#include "shaders/test_vs.h"
#include "shaders/test_ps.h"

class DX11Tester : public DX11WindowController
{
public:

    Simpleton::DX11Texture m_Texture;
    Simpleton::DX11Mesh m_Mesh;
    Simpleton::DX11PipelineStateBuilder m_PSOBuilder;
    Simpleton::DX11PipelineState m_PSO;
    Simpleton::DX11PipelineResourceSet m_Bindings;
    ComPtr<ID3D11SamplerState> m_pSampler;
    
    virtual bool OnCreate( DX11Window* pWin )
    {
        m_Mesh.InitXYQuad( pWin->GetDevice() );
        
        m_PSOBuilder.BeginState( pWin->GetDevice() );
        m_PSOBuilder.SetVertexShader( test_vs, sizeof(test_vs) );
        m_PSOBuilder.SetInputLayout( m_Mesh.GetVertexElements(), m_Mesh.GetVertexElementCount() );
        m_PSOBuilder.SetPixelShader( test_ps, sizeof(test_ps) );
        
        D3D11_RASTERIZER_DESC rd;
        rd.AntialiasedLineEnable = false;
        rd.CullMode = D3D11_CULL_NONE;
        rd.DepthBias = 0;
        rd.DepthBiasClamp = 0;
        rd.DepthClipEnable = true;
        rd.FillMode = D3D11_FILL_SOLID;
        rd.FrontCounterClockwise = true;
        rd.MultisampleEnable = false;
        rd.ScissorEnable = true;
        rd.SlopeScaledDepthBias = 0;
        m_PSOBuilder.SetRasterizerState(rd);

        m_PSOBuilder.EndState( &m_PSO );
      
        m_Texture.InitFromPPMFile(pWin->GetDevice(), "out.ppm");
        m_pSampler = CreateLinearSampler( pWin->GetDevice() );
      
        m_PSO.GetResourceSchema()->CreateResourceSet( &m_Bindings, pWin->GetDevice() );
        uint nSamplerName = m_PSO.GetResourceSchema()->LookupSampler( "sSampler" );
        uint nSRVName     = m_PSO.GetResourceSchema()->LookupSRV( "tTexture" );
        
        m_Bindings.BeginUpdate( pWin->GetDeviceContext() );
        m_Bindings.BindSampler( nSamplerName, m_pSampler );
        m_Bindings.BindSRV( nSRVName, m_Texture.GetSRV() );
        m_Bindings.EndUpdate( pWin->GetDeviceContext() );

        return true;
    }

    virtual void OnFrame( DX11Window* pWindow )
    {
        float fAspect = pWindow->GetViewportWidth() / (float)pWindow->GetViewportHeight();
        Matrix4f mView = Simpleton::MatrixLookAtLH( Vec3f(0,0,-3), Vec3f(0,0,1), Vec3f(0,1,0) );
        Matrix4f mProj = MatrixPerspectiveFovLH( fAspect, 60.0f, 1, 500 );

        Matrix4f mViewProj = mProj*mView;

        double rot = (fmod( pWindow->GetAccumulatedTime(), 3.0 ) / 3.0 )*2.0*3.1415926;
        Matrix4f mWorld = MatrixIdentity();
        mWorld = MatrixRotate(0,1,0, rot );

        ID3D11DeviceContext* pContext = pWindow->GetDeviceContext() ;
        m_Bindings.BeginUpdate( pContext );
        m_Bindings.BindConstant( "g_ViewProj", &mViewProj, sizeof(mViewProj) );
        m_Bindings.BindConstant( "g_World", &mWorld, sizeof(mWorld) );
        m_Bindings.EndUpdate(pContext);

        m_PSO.Apply( pContext );
        m_Bindings.Apply( pContext );

        float back[] = {1,0,0,0};

        D3D11_VIEWPORT vp = pWindow->BuildViewport();
        D3D11_RECT rect = pWindow->BuildScissorRect();
        ID3D11RenderTargetView* pRTV = pWindow->GetBackbufferRTV();
        ID3D11DepthStencilView* pDSV = pWindow->GetBackbufferDSV();
        pContext->OMSetRenderTargets( 1, &pRTV, pWindow->GetBackbufferDSV() );
        pContext->ClearRenderTargetView( pRTV, back );
        pContext->ClearDepthStencilView( pDSV,D3D11_CLEAR_DEPTH,1,0);
        pContext->RSSetViewports( 1, &vp );
        pContext->RSSetScissorRects( 1, &rect );
       
        ID3D11Buffer* pVB = m_Mesh.GetVB();
        uint nStride = m_Mesh.GetVertexSize();
        uint nOffset = 0;
        pContext->IASetIndexBuffer( m_Mesh.GetIB(), m_Mesh.GetIndexFormat(), 0 );
        pContext->IASetVertexBuffers( 0, 1, &pVB, &nStride, &nOffset );
        m_Mesh.Draw( pContext );

    }

  

};

int main()
{
    GDITester t;
    GDIWindow* pWin = Simpleton::GDIWindow::Create( 512,512,&t);
    while( pWin->DoEvents() )
        pWin->RepaintAll();

    DX11Tester tester;
    DX11Window* pDX11Win = Simpleton::DX11Window::Create( 512,512,0,&tester);
    while( pDX11Win->DoEvents() )
        pDX11Win->DoFrame();

    return 0;
}
