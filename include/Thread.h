//=====================================================================================================================
//
//   Thread.h
//
//   Definition of class: Simpleton::Thread
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _THREAD_H_
#define _THREAD_H_


namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief A base class which encapsulates thread creation and execution
    ///
    /// This class is modeled after the java threading system.  To create a thread, simply subclass the Thread class 
    /// and overload the OnExecute method, and then instantiate the class and call 'Start'.  
    /// The base class is intended to hide the details of thread creation from the subclasses
    ///
    //=====================================================================================================================
    class Thread
    {
    public:

        /// The thread is created by not executed by the constructor.  To start executing a thread, call Start()
        Thread( );

        virtual ~Thread();

        /// Starts the thread
        bool Start();

        /// Tests the thread for completion
        bool IsActive();

        /// Waits for the thread to complete
        void WaitForCompletion();

    protected:

        // Protected interface for use by thread sub-classes

        /// This method is called on thread start to execute the thread.  Thread execution stops when this method returns.
        virtual void OnExecuteThread() =0;


    private:

        /// Thread execution procedure, its a friend so that it can call OnExecuteThread
        friend unsigned int __stdcall  SimpletonThreadProc( void* pThreadData );

        bool m_bDeadFlag;       ///< Set when thread terminates
        void* m_nThreadHandle;  ///< Thread handle

    };
    
}

#endif // _THREAD_H_
