//=====================================================================================================================
//
//   ComPtr.h
//
//   Smart pointer for COM reference counting
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _COMPTR_H_
#define _COMPTR_H_

#include <new>

namespace Simpleton
{
    template< class T >
    class ComPtr
    {
    public:
        ComPtr() : m_p(0){};
        ComPtr( const ComPtr<T>& p ) : m_p(p.m_p) { if(m_p) m_p->AddRef(); }
        ComPtr( T* p ) : m_p(p) { if(m_p) m_p->AddRef(); }
        ~ComPtr() { if( m_p ) m_p->Release(); }

        const ComPtr<T>& operator=(const ComPtr<T>& p)
        {
            this->~ComPtr();
            return *(new(this) ComPtr<T>(p));
        };
        const ComPtr<T>& operator=(T* p)
        {
            this->~ComPtr();
            return *(new(this) ComPtr<T>(p));
        };
        
        operator T*() const { return m_p; }
        T* operator->() const { return m_p; }
        
        void Release() { 
            if( m_p ) 
                m_p->Release(); 
            m_p=0; 
        }

        /// Assign without adding a reference, used for transferring ownership
        void Owns( T* p ) {
            if( m_p )
                m_p->Release();
            m_p = p;
        }

        T** Address() { return &m_p; }

    private:
        T* m_p;
    };

}

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x){ x->Release(); x =0; }
#endif

#endif