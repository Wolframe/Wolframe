#ifndef _Wolframe_FILTER_BASE_HPP_INCLUDED
#define _Wolframe_FILTER_BASE_HPP_INCLUDED
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

struct StrIterator
{
public:
	StrIterator( const char* src, unsigned int size) :m_src(src),m_size(size),m_pos(0){}
	char operator* () {return (m_pos < m_size)?m_src[m_pos]:0;}
	StrIterator& operator++() {m_pos++; return *this;}
	unsigned int pos() const {return m_pos;}

private:
	const char* m_src;
	unsigned int m_size;
	unsigned int m_pos;
};

///\class SrcIterator
///\brief Input iterator as source for the XML scanner (throws EndOfMessageException on EoM)
struct SrcIterator
{
	///\class EoM
	///\brief End of message exception
	struct EoM{};

	protocol::InputFilter* m_gen;		///< input for the iterator (from network message)

	///\brief Empty constructor
	SrcIterator()
		:m_gen(0) {}

	///\brief Constructor
	SrcIterator( protocol::InputFilter* gen)
		:m_gen(gen) {}

	///\brief Copy constructor
	///\param [in] o iterator to copy
	SrcIterator( const SrcIterator& o)
		:m_gen(o.m_gen) {}

	///\brief access operator (required by textwolf for an input iterator)
	char operator*()
	{
		if (!m_gen->size()) throw EoM();
		return *(char*)m_gen->ptr();
	}

	///\brief prefix increment operator (required by textwolf for an input iterator)
	SrcIterator& operator++()
	{
		m_gen->skip(1);
		return *this;
	}
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
		StrIterator itr( src, srcsize);
		textwolf::TextScanner<StrIterator,AppCharset> ts( itr);

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


