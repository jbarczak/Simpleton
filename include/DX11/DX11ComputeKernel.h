//=====================================================================================================================
//
//   DX11ComputeKernel.h
//
//   Compute shader
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _DX11_COMPUTE_KERNEL_H_
#define _DX11_COMPUTE_KERNEL_H_

#include <d3d11.h>
#include "ComPtr.h"

#include "DX11ComputeResourceSchema.h"

namespace Simpleton
{
    
    class DX11ComputeKernel
    {
    public:

        bool Init( const void* pCSCode, size_t nCodeSize, ID3D11Device* pDevice );

        void Bind( ID3D11DeviceContext* pDevice );

        const DX11ComputeResourceSchema* GetResourceSchema() const { return &m_Schema; }
        
        const UINT* GetThreadGroupDims() const { return m_Dims;}
    private:
        ComPtr<ID3D11ComputeShader> m_pShader;
        DX11ComputeResourceSchema m_Schema;
        UINT m_Dims[3];
    };
}

#endif