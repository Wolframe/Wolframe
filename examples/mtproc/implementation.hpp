#ifndef _Wolframe_MTPROC_IMPLEMENTATION_HPP_INCLUDED
#define _Wolframe_MTPROC_IMPLEMENTATION_HPP_INCLUDED
#include "methodtable.hpp"

namespace _Wolframe {
namespace mtproc {

struct Implementation :public Instance
{
	//* constructor/destructor of data
	static Method::Data* createData();
	static void destroyData( Method::Data* data);

	//* methods (context->data points to this)

	//echo the content
	static int echo( Method::Context* context, unsigned int argc, const char** argv);

	//echo the arguments without referencing content
	static int printarg( Method::Context* context, unsigned int argc, const char** argv);

	Implementation()
	{
		static Method mt[3] = {{"echo",&echo,true},{"parg",&printarg,false},{0,0,false}};
		m_mt = mt;
		m_data = 0;
		m_createData = &createData;
		m_destroyData = &destroyData;
	}

	bool init()
	{
		if (m_data) destroyData( m_data);
		m_data = createData();
		return (!!m_data);
	}

	void done()
	{
		if (m_data)
		{
			destroyData( m_data);
			m_data = 0;
		}
	}
}; 

}}//namespace
#endif


