//=====================================================================================================================
//
//   Thread.cpp
//
//   Implementation of class: Simpleton::Thread
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "Thread.h"
#include <windows.h>
#include <process.h>

namespace Simpleton
{

    //=====================================================================================================================
    /// Thread procedure for thread classes.  
    //=====================================================================================================================
    unsigned int __stdcall SimpletonThreadProc( void* pThreadData )
    {
        Thread* pThread = (Thread*) pThreadData;
        pThread->OnExecuteThread();     // do work
        pThread->m_bDeadFlag = true;    // set 'dead' flag to indicate that work is done
        return 0;
    }

    //=====================================================================================================================
    //
    //         Constructors/Destructors
    //
    //=====================================================================================================================


    //=====================================================================================================================
    //=====================================================================================================================
    Thread::Thread( ) : m_nThreadHandle(0), m_bDeadFlag(false)
    {

    }

    //=====================================================================================================================
    //=====================================================================================================================
    Thread::~Thread()
    {
        // close thread handle upon destruction
        if( m_nThreadHandle )
        {
            CloseHandle( m_nThreadHandle );
        }
    }


    //=====================================================================================================================
    //
    //            Public Methods
    //
    //=====================================================================================================================

    //=====================================================================================================================
    /// This method starts the thread, and then returns.
    ///
    /// If the thread is running, or has already run to completion, this call has no effect.
    ///
    /// Once the thread starts, its 'OnExecuteThread' method will be invoked
    ///
    /// It is illegal to call this method twice on the same thread.  Thread objects may only be run once.
    ///  To repeatedly start the same thread, you must instantiate new thread objects.  
    /// 
    /// The thread implementation will not deal correctly with race conditions 
    ///    (two different threads trying to call 'Start' on a third thread)
    ///
    /// \return True if successful, false if thread creation fails, or if the thread was already started
    //=====================================================================================================================
    bool Thread::Start()
    {
        if( m_nThreadHandle )
        {
            // Can't execute threads twice
            return false;
        }

        // create thread
        m_nThreadHandle = (void*) _beginthreadex( 0, 0, &SimpletonThreadProc, this, 0, 0 );

        if( m_nThreadHandle == 0 )
        {
            // couldn't start the thread
            return false;
        }

        return true;
    }

    //=====================================================================================================================
    /// Determines whether or not the 'OnExecuteThread()' method has finished executing
    /// \return True if the thread is running and not finished, false if not
    //=====================================================================================================================
    bool Thread::IsActive()
    {
        if( !m_nThreadHandle )
        {
            // thread was not started yet...        
            return false;
        }

        // thread was started, see if it stopped
        return !m_bDeadFlag;
    }

    //=====================================================================================================================
    /// Waits for a running thread to complete
    //=====================================================================================================================
    void Thread::WaitForCompletion()
    {
        if( !m_nThreadHandle )
        {
            // called before starting thread.  probably a logic error
            return;
        }

        WaitForSingleObject( m_nThreadHandle, INFINITE );
    }
}


