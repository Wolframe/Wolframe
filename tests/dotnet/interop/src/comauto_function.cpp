#include "comauto_function.hpp"
#include "comauto_utils.hpp"
#include "comauto_clr.hpp"

using namespace _Wolframe;

comauto::Function::Function( comauto::CommonLanguageRuntime* clr_, ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx)
	:m_clr(clr_)
	,m_typeinfo(typeinfo_)
	,m_funcdesc(0)
	,m_assemblyname(assemblyname_)
	,m_classname(classname_)
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

std::size_t comauto::Function::getParameterIndex( const std::string& name) const
{
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi->first == name) return pi-m_parameterlist.begin();
	}
	throw std::runtime_error( std::string("unknown parameter name '") + name + "'");
}

const std::string& comauto::Function::getParameterName( std::size_t idx) const
{
	return m_parameterlist.at(idx).first;
}

TYPEDESC* comauto::Function::getParameter( std::size_t idx) const
{
	if (idx >= m_parameterlist.size()) throw std::runtime_error( "parameter index out of range");
	return m_parameterlist.at( idx).second;
}

comauto::FunctionClosure::FunctionClosure( const Function* func_)
		:m_func(func_)
		,m_flags(serialize::Context::None)
		,m_provider(0)
		,m_parameter(0)
		,m_resultdata(0){}

comauto::FunctionClosure::~FunctionClosure()
{
	if (m_parameter) delete [] m_parameter;
}

void comauto::FunctionClosure::init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
{
	m_provider = p;
	m_input = i;
	m_flags = f;
	std::size_t ii = 0,nn = m_func->nofParameter();
	if (m_parameter) delete [] m_parameter;
	m_parameter = new VARIANT[ m_func->nofParameter()];
	for (; ii < nn; ++ii)
	{
		m_parameter[ii].vt = VT_EMPTY;
	}
}

bool comauto::FunctionClosure::call()
{
	langbind::TypedFilterBase::ElementType elemtype;
	langbind::TypedFilterBase::Element elemvalue;

	while (m_input->getNext( elemtype, elemvalue))
	{
		switch (elemtype)
		{
			case langbind::InputFilter::OpenTag:
				break;
			case langbind::InputFilter::CloseTag:
				throw std::runtime_error( "unexpected close tag");
			case langbind::InputFilter::Attribute:
				break;
			case langbind::InputFilter::Value:
				throw std::runtime_error( "tag or attribute name expected");
		}
	}
	switch (m_input->state())
	{
		case langbind::InputFilter::Error: return false;
		case langbind::InputFilter::EndOfMessage: return false;
		case langbind::InputFilter::Open: break;
	}
	if ((m_flags & serialize::Context::ValidateAttributes) != 0)
	{
		// ... don't know yet what to do here, because attributes are not defined for .NET functions ...
	}
	if ((m_flags & serialize::Context::ValidateInitialization) != 0)
	{
		std::size_t ii = 0,nn = m_func->nofParameter();
		for (; ii < nn; ++ii)
		{
			if (m_parameter[ii].vt == VT_EMPTY)
			{
				TYPEDESC* td = m_func->getParameter( ii);
				if ((td->vt & VT_ARRAY) == VT_ARRAY || (td->vt & VT_VECTOR) == VT_VECTOR) continue;

				throw std::runtime_error( std::string( "missing parameter '") + m_func->getParameterName(ii) + "'");
			}
		}
	}
	VARIANT* m_resultdata;
	FunctionResult* resptr;
	m_result.reset( resptr = new FunctionResult());
	m_resultdata = resptr->data();

	*m_resultdata = m_func->clr()->call( m_func->assemblyname(), m_func->classname(), m_func->methodname(), m_func->nofParameter(), m_parameter);
	return true;
}

langbind::TypedInputFilterR comauto::FunctionClosure::result() const
{
	langbind::TypedInputFilterR rt;
	return rt;
}

