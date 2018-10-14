//=====================================================================================================================
//
//   ThreadPool.h
//
//   Definition of class: Simpleton::ThreadPool
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2016 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <deque>
#include "SpinLock.h"

namespace Simpleton
{
    class Thread;

    class WorkItem
    {
    public:
        virtual void Do() = 0;
    };

    class ThreadPool
    {
    public:

        void Start( size_t nWorkers );

        void Shutdown();

        /// Insert work into the thread pool
        void PushWork( WorkItem* pItem )
        {
            m_Lock.Take();
            m_WorkItems.push_back(pItem);
            m_Lock.Release();
        }

        /// Execute at most one queued work item
        ///  Return true if work was done
        bool DoWork()
        {
            m_Lock.Take();
            if( m_WorkItems.empty() )
            {
                m_Lock.Release(); 
                return false;
            }
            WorkItem* pWork = m_WorkItems.front();
            m_WorkItems.pop_front();
            m_Lock.Release();

            pWork->Do();
            return true;
        }

        /// Block caller until thread pool drains
        void WaitAll()
        {
            while( DoWork() )
                ;
        }

    private:

        std::vector<Thread*> m_WorkerThreads;

        SpinLock m_Lock;
        std::deque<WorkItem*> m_WorkItems;
    };


}


#endif