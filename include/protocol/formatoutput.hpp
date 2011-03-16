#ifndef _Wolframe_PROTOCOL_FORMATOUTPUT_INTERFACE_HPP_INCLUDED
#define _Wolframe_PROTOCOL_FORMATOUTPUT_INTERFACE_HPP_INCLUDED
#include <cstddef>

namespace _Wolframe {
namespace protocol {

//@section protocolFormatOutputInterface

struct FormatOutput
{
	typedef std::size_t size_type;

	enum ElementType
	{
		OpenTag,	//Open new hierarchy level
		Attribute,	//Attribute Name
		Value,		//Content or attribute Value
		CloseTag	//Close current hierarchy level
	};

	//Print next element call
	typedef bool (*Print)( FormatOutput* this_, ElementType type, void* element, size_type elementsize);

	FormatOutput( const Print& op) :m_ptr(0),m_pos(0),m_size(0),m_print(op){}

	FormatOutput& operator = (const FormatOutput& o)
	{
		m_ptr = o.m_ptr;
		m_pos = o.m_pos;
		m_size = o.m_size;
		return *this;
	}

	void init( void* data, size_type datasize)
	{
		m_ptr = data;
		m_size = datasize;
		m_pos = 0;
	}

	void init()
	{
		init( 0, 0);
	}

	void* cur() const				{return (void*)((char*)m_ptr+m_pos);}
	size_type restsize() const			{return (m_pos<m_size)?(m_size-m_pos):0;}
	size_type pos() const			{return m_pos;}
	size_type size() const			{return m_size;}
	void* ptr() const				{return m_ptr;}
	void incr( size_type n)			{if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}

	bool print( ElementType type, void* element, size_type elementsize)
	{
		return m_print( this, type, element, elementsize);
	}

private:
	void* m_ptr;
	size_type m_pos;
	size_type m_size;
	Print m_print;
};

}}//namespace
#endif

