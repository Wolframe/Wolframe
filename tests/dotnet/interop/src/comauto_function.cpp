#include "comauto_function.hpp"
#include "comauto_utils.hpp"

comauto::Function::Function( ITypeInfo* typeinfo_, const std::string& classname_, unsigned short fidx)
	:m_typeinfo(typeinfo_),m_funcdesc(0),m_classname(classname_)
{
	WRAP( m_typeinfo->GetFuncDesc( fidx, &m_funcdesc))
	struct Local	//exception safe memory allocation of local variables
	{
		BSTR* pnames;
		UINT size;

		Local()	:pnames(0){}
		~Local()
		{
			if (pnames)
			{
				for (UINT ii=0; ii<size; ++ii) if (pnames[ii]) ::SysFreeString( pnames[ii]);
				delete [] pnames;
			}
		}
	};
	Local local;
	local.pnames = new BSTR[ local.size = m_funcdesc->cParams+1];
	UINT ii,nn;
	WRAP( m_typeinfo->GetNames( m_funcdesc->memid, local.pnames, m_funcdesc->cParams+1, &nn))

	m_methodname = comauto::asciistr( local.pnames[0]);
	for (ii=1; ii<nn; ++ii)
	{
		m_parameterlist.push_back( Parameter( comauto::asciistr(local.pnames[ii]), &m_funcdesc->lprgelemdescParam[ii-1].tdesc));
	}
	m_returntype = &m_funcdesc->elemdescFunc.tdesc;
}

comauto::Function::~Function()
{
	m_typeinfo->ReleaseFuncDesc( m_funcdesc);
}

void comauto::Function::print( std::ostream& out)
{
	if (m_classname.empty())
	{
		out << "FUNCTION " << m_methodname << "( ";
	}
	else
	{
		out << "METHOD " << m_classname << "." << m_methodname << "( ";
	}
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi != m_parameterlist.begin()) out << ", ";
		out << pi->first << " :" << comauto::typestr( m_typeinfo, pi->second);
	}
	out << " ) :" << comauto::typestr( m_typeinfo, m_returntype) << std::endl;
}


