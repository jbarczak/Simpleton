//=====================================================================================================================
//
//   Mutex.h
//
//   Definition of class: Simpleton::Mutex
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _MUTEX_H_
#define _MUTEX_H_


namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A class to encapsulate a critical section for thread synchronization
    ///
    ///  This class provides mutual exclusion by serializing threads based on calls to the 'Take()' method.
    ///  Threads that are about to enter the critical section should 'Take()' the mutex, then 'Release()' it after
    ///  leaving the critical section.  This class will use OS synchronization mechanisms to provide mutual exclusion
    ///
    //=====================================================================================================================
    class Mutex
    {
    public:

        Mutex();

        ~Mutex();

        void Take( );

        void Release( );
    
    private:

        // so we don't have to #include <windows.h>, with all its namespace pollution, in the header
        // This is a tad ugly, but should be stable
        static const int SIZEOF_CRITICAL_SECTION = 24;
        unsigned char m_criticalSection[SIZEOF_CRITICAL_SECTION];
    };
    
    
}

#endif // _MUTEX_H_
