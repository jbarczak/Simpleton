//=====================================================================================================================
//
//   LockFreeStack.h
//
//   Definition of class: Simpleton::LockFreeStack
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _LOCKFREESTACK_H_
#define _LOCKFREESTACK_H_

#include <atomic>

namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief Lockfree stack data structure
    ///  
    ///   Memory management for stack nodes is the responsibility of the user
    ///     This class is simply a wrapper around a head pointer, which abstracts away the atomic operations
    ///
    //=====================================================================================================================
    class LockFreeStack
    {
    public:
    
        class Node
        {
        private:
            friend class LockFreeStack;
            Node* m_pNext_LFS;
        };

          
        //=====================================================================================================================
        //=====================================================================================================================
        LockFreeStack() : m_pHead(0)
        {
        }

        //=====================================================================================================================
        //=====================================================================================================================
        void push_front( Node* pNode )
        {
            Node* pHead = m_pHead.load( std::memory_order_relaxed );
            do
            {
                pNode->m_pNext_LFS = pHead;
            } while( !m_pHead.compare_exchange_weak( pHead, pNode,
                                                     std::memory_order_release,
                                                     std::memory_order_relaxed) );
        }

        //=====================================================================================================================
        //=====================================================================================================================
        Node* pop_front( )
        {
            Node* pPop;
            do
            {
                pPop = m_pHead.load( std::memory_order_relaxed );
                if( pPop == 0 )
                    return pPop;

            } while( !m_pHead.compare_exchange_weak( pPop, pPop->m_pNext_LFS,
                                                     std::memory_order_release,
                                                     std::memory_order_relaxed) );

            return pPop;
        }

        //=====================================================================================================================
        /// Non-synchronized push_front operation.  For use in single-threaded code
        //=====================================================================================================================
        void push_front_serial( Node* pNode )
        {
            pNode->m_pNext_LFS = m_pHead.load( std::memory_order_relaxed );
            m_pHead.store( pNode, std::memory_order_relaxed );
        }

        //=====================================================================================================================
        /// Non-synchronized push_front operation.  For use in single-threaded code
        //=====================================================================================================================
        Node* pop_front_serial()
        {
            Node* pN = m_pHead.load( std::memory_order_relaxed );
            if( pN )
                m_pHead.store( pN->m_pNext_LFS, std::memory_order_relaxed );
            return pN;
        }

        //=====================================================================================================================
        /// Eliminate all nodes
        //=====================================================================================================================
        void clear()
        {
            m_pHead = 0;
        }

        //=====================================================================================================================
        /// Replace 'dest' with contents of this stack
        //=====================================================================================================================
        void CopyFrom( const LockFreeStack& src )
        {
            m_pHead.store( src.m_pHead );
        }

    private:
        std::atomic<Node*> m_pHead;
    };
    
}

#endif // _LOCKFREESTACK_H_
