#ifndef _Wolframe_FILTER_BASE_HPP_INCLUDED
#define _Wolframe_FILTER_BASE_HPP_INCLUDED
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

struct CharIterator
{
public:
	CharIterator( const char* src, unsigned int size) :m_src(src),m_size(size),m_pos(0){}
	char operator* () {return (m_pos < m_size)?m_src[m_pos]:0;}
	CharIterator& operator++() {m_pos++; return *this;}
	unsigned int pos() const {return m_pos;}

private:
	const char* m_src;
	unsigned int m_size;
	unsigned int m_pos;
};

///\class FilterBase
///\brief Filter base template
///\tparam IOCharset Character set encoding of input and output
///\tparam AppCharset Character set encoding of the application processor
///\tparam BufferType STL back insertion sequence to use for printing output
template <class IOCharset, class AppCharset>
struct FilterBase
{
	typedef std::size_t size_type;
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> BufferType;

	///\param [in] src pointer to string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	static void printToBuffer( const char* src, size_type srcsize, BufferType& buf)
	{
		CharIterator itr( src, srcsize);
		textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			IOCharset::print( ch, buf);
			++ts;
		}
	}

	///\brief prints a character to an STL back insertion sequence buffer in the IO character set encoding
	///\param [in] ch character to print
	///\param [in,out] buf buffer to print to
	static void printToBuffer( char ch, BufferType& buf)
	{
		IOCharset::print( (textwolf::UChar)(unsigned char)ch, buf);
	}
};

}}//namespace
#endif


