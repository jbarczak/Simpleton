//=====================================================================================================================
//
//   Timer.h
//
//   Definition of class: Simpleton::Timer
//     Very ancient code, some of which I wrote in college.  Don't judge me...
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _TIMER_H_
#define _TIMER_H_


namespace Simpleton
{

    //=====================================================================================================================
    /// \ingroup Simpleton
    /// \brief Simple utility timer for benchmarking
    //=====================================================================================================================
    class Timer
    {
    public:
        
        Timer();

        ~Timer();

        /// Returns the number of milliseconds elapsed since this object was created, or since the last reset
        inline unsigned int Tick() const { return m_pImpl->Tick(); };

        /// Same as tick, but in microseconds
        inline unsigned long TickMicroSeconds() const { return m_pImpl->TickMicroSeconds(); };

        /// Resets the timer
        inline void Reset() { return m_pImpl->Reset(); };

        // Windows and linux need to use different member types, so we use the PIMPL idiom here to avoid
        //   including that dastardly windows.h header on Win32 builds
        class TimerImpl
        {
        public:
           
            virtual unsigned int Tick() const = 0;
            virtual unsigned long TickMicroSeconds() const = 0;
            virtual void Reset() = 0;
        };


    private:

        inline Timer( const Timer&  ) {};
        inline const Timer& operator=( const Timer&  ) {};
       
        TimerImpl* m_pImpl;
    };

    
}

#endif // _TIMER_H_
