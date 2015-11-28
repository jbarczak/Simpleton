//=====================================================================================================================
//
//   TextStream.h
//
//   
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2015 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include <istream>
#include <string>

namespace Simpleton
{
    class ITextStream
    {
    public:
        virtual const char* NextLine() = 0;
    };

    class SlowTextStream
    {
    public:

        SlowTextStream( std::istream& stream )
            : m_rStream(stream)
        {
        }

        virtual const char* NextLine() = 0
        {
            std::getline( m_rStream, m_Buffer );
            return m_Buffer.c_str();
        }

    private:

        std::istream& m_rStream;
        std::string m_Buffer;
    };

    class FastTextStream
    {
    public:

    };

}