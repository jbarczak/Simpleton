//=====================================================================================================================
//
//   DX11BufferPool.h
//
//   Definition of class: Simpleton::DX11BufferPool
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11BUFFERPOOL_H_
#define _DX11BUFFERPOOL_H_

#include "LockFreeStack.h"

namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief Helper class to manage a chain of dynamic buffers
    //=====================================================================================================================
    class DX11BufferPool
    {
    public:

        ~DX11BufferPool();

        void Init( ID3D11Device* pDevice, const D3D11_BUFFER_DESC& desc )
        {
            m_pDevice = pDevice;
            m_Desc = desc;
        }

        ID3D11Buffer* GetBuffer();

        void RecycleBuffers();
        void ReleaseBuffers();
    
    private:
        struct Buffer : public LockFreeStack::Node
        {
            ComPtr<ID3D11Buffer> pBuffer;
        };

        D3D11_BUFFER_DESC m_Desc;

        Simpleton::ComPtr<ID3D11Device> m_pDevice;
        LockFreeStack m_AvailableBuffers;
        LockFreeStack m_AllBuffers;
        
    };
    
}

#endif // _DX11BUFFERPOOL_H_
