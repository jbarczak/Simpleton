//=====================================================================================================================
//
//   SpinLock.h
//
//   Spin-wait primitive
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2016 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <xmmintrin.h>
#include <atomic>

namespace Simpleton
{
    class SpinLock
    {
    public:

        SpinLock() :  m_Flag ( ATOMIC_FLAG_INIT )
        {
        }

        void Take()
        {
            while( !m_Flag.test_and_set(std::memory_order_acquire) )
                _mm_pause();
        }


        void Release()
        {
            m_Flag.clear( std::memory_order_release );
        }

    private:
        std::atomic_flag m_Flag;
    };

}