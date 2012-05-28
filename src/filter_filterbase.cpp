#include "filter/filterbase.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace langbind;

void FilterBase::setError( const char* msg)
{
	if (msg)
	{
		std::size_t msglen = std::strlen( msg);
		if (msglen >= ErrorBufSize) msglen = (std::size_t)ErrorBufSize-1;
		std::memcpy( m_errorbuf, msg, msglen);
		m_errorbuf[ msglen] = 0;
	}
	else
	{
		m_errorbuf[ 0] = 0;
	}
}

const char* FilterBase::elementTypeName( ElementType i)
{
	static const char* ar[] = {"OpenTag","Attribute","Value","CloseTag"};
	return ar[(int)i];
}

const char* FilterBase::getError() const
{
	return m_errorbuf[0]?m_errorbuf:0;
}

