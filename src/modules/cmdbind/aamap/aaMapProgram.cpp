/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file aaMapProgram.cpp
/// \brief Implementation of  programs mapping authorization/audit calls to form function calls

#include "aaMapProgram.hpp"
#include "langbind/authorizationFunction.hpp"
#include "langbind/auditFunction.hpp"
#include "processor/execContext.hpp"
#include "langbind/formFunction.hpp"
#include "utils/fileUtils.hpp"
#include "utils/fileLineInfo.hpp"
#include "utils/parseUtils.hpp"
#include "filter/joinfilter.hpp"
#include "filter/execContextInputFilter.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

namespace
{
struct DatabaseScope
{
	const std::string* dbname;
	proc::ExecContext* ctx;

	DatabaseScope( const std::string* dbname_, proc::ExecContext* ctx_)
		:dbname(dbname_),ctx(ctx_)
	{
		if (!dbname->empty())
		{
			ctx->push_database( *dbname);
		}
	}
	~DatabaseScope()
	{
		if (!dbname->empty())
		{
			ctx->pop_database();
		}
	}
};

typedef boost::shared_ptr<DatabaseScope> DatabaseScopeR;



/// \class AuthorizationFunctionImpl
/// \brief Description of a an authorization function
class AuthorizationFunctionImpl
	:public langbind::AuthorizationFunction
{
public:
	/// \brief Constructor
	AuthorizationFunctionImpl( const langbind::FormFunction* function_, const std::vector<langbind::ExecContextElement>& params_, const std::string& database_)
		:m_function(function_)
		,m_params(params_)
		,m_database(database_){}

	/// \brief Copy constructor
	AuthorizationFunctionImpl( const AuthorizationFunctionImpl& o)
		:m_function(o.m_function)
		,m_params(o.m_params)
		,m_database(o.m_database){}

	/// \brief Default constructor
	AuthorizationFunctionImpl(){}

	/// \brief Destructor
	virtual ~AuthorizationFunctionImpl(){}

	const langbind::FormFunction* function() const
	{
		return m_function;
	}
	const std::vector<langbind::ExecContextElement>& params() const
	{
		return m_params;
	}

	virtual bool call( proc::ExecContext* ctx, const std::string& resource, std::vector<Attribute>& attributes) const;

private:
	const langbind::FormFunction* m_function;		///< form function to call
	std::vector<langbind::ExecContextElement> m_params;	///< parameter description
	std::string m_database;					///< database used for authorization functions
};


/// \brief Foward declaration
class AuditFunctionImpl;

/// \class AuditFunctionClosureImpl
/// \brief Closure of a an auditing function
class AuditFunctionClosureImpl
	:public langbind::FormFunctionClosure
{
public:
	AuditFunctionClosureImpl( const AuditFunctionImpl* auditfunc_, const langbind::FormFunctionClosureR& formFunctionClosure_)
		:m_auditfunc(auditfunc_)
		,m_formFunctionClosure(formFunctionClosure_)
		,m_context(0){}

	/// \brief Destructor
	virtual ~AuditFunctionClosureImpl(){}

	/// \brief Calls the function with the input from the input filter specified
	/// \return true when completed
	virtual bool call();

	/// \brief Initialization of call context for a new call
	/// \param[in] c execution context reference
	/// \param[in] i call input
	/// \param[in] f serialization flags for validating form functions depending on caller context (directmap "strict",lua relaxed)
	virtual void init( proc::ExecContext* c, const langbind::TypedInputFilterR& i, serialize::Flags::Enum f);

	/// \brief Get the iterator for the function result
	/// \remark MUST be standalone (alive after destruction of this 'FormFunctionClosure'!)
	virtual langbind::TypedInputFilterR result() const
	{
		return m_formFunctionClosure->result();
	}

private:
	const AuditFunctionImpl* m_auditfunc;
	langbind::FormFunctionClosureR m_formFunctionClosure;
	proc::ExecContext* m_context;
	DatabaseScopeR m_dbscope;
};

/// \class AuditFunctionImpl
/// \brief Description of a an auditing function
class AuditFunctionImpl
	:public langbind::AuditFunction
{
public:
	/// \brief Constructor
	AuditFunctionImpl( const langbind::FormFunction* function_, const std::vector<langbind::ExecContextElement>& params_, const std::string& database_)
		:m_function(function_)
		,m_params(params_)
		,m_database(database_){}

	/// \brief Copy constructor
	AuditFunctionImpl( const AuditFunctionImpl& o)
		:m_function(o.m_function)
		,m_params(o.m_params)
		,m_database(o.m_database){}

	/// \brief Default constructor
	AuditFunctionImpl(){}

	/// \brief Destructor
	virtual ~AuditFunctionImpl(){}

	virtual langbind::FormFunctionClosure* createClosure() const
	{
		langbind::FormFunctionClosureR fc( m_function->createClosure());
		return new AuditFunctionClosureImpl( this, fc);
	}

	const std::vector<langbind::ExecContextElement>& params() const
	{
		return m_params;
	}

	const std::string& database() const
	{
		return m_database;
	}

private:
	const langbind::FormFunction* m_function;		///< form function to call
	std::vector<langbind::ExecContextElement> m_params;	///< parameter description
	std::string m_database;					///< database used for authorization functions
};

}//anonymous namespace


void AuditFunctionClosureImpl::init( proc::ExecContext* c, const langbind::TypedInputFilterR& i, serialize::Flags::Enum f)
{
	m_context = c;
	langbind::TypedInputFilterR argfilter( new langbind::ExecContextInputFilter( m_auditfunc->params(), *c, ""));
	langbind::TypedInputFilterR input( new langbind::JoinInputFilter( "auditfilterarg", argfilter, i));
	m_formFunctionClosure->init( c, input, f);
}

bool AuditFunctionClosureImpl::call()
{
	if (!m_context)
	{
		throw std::runtime_error( "execution context not initialized");
	}
	if (!m_dbscope.get())
	{
		m_dbscope.reset( new DatabaseScope( &m_auditfunc->database(), m_context));
	}
	if (m_formFunctionClosure->call())
	{
		m_dbscope.reset();
		return true;
	}
	else
	{
		return false;
	}
}


bool AuthorizationFunctionImpl::call( proc::ExecContext* ctx, const std::string& resource, std::vector<Attribute>& attributes) const
{
	try
	{
		DatabaseScope databaseScope( &m_database, ctx);

		langbind::FormFunctionClosureR clos( m_function->createClosure());
		langbind::TypedInputFilterR input( new langbind::ExecContextInputFilter( params(), *ctx, resource));
		clos->init( ctx, input);
		if (!clos->call())
		{
			return false;
		}
		langbind::TypedInputFilterR output = clos->result();
		langbind::FilterBase::ElementType res_type;
		types::VariantConst res_elem;
	
		if (output->getNext( res_type, res_elem))
		{
			if (res_type == langbind::FilterBase::CloseTag)
			{
				//...no result => authorization accepted
				return true;
			}
			if (res_type == langbind::FilterBase::Value)
			{
				//...boolean result is interpreted as result of authorization
				return res_elem.tobool();
			}
			while (res_type == langbind::FilterBase::OpenTag
			||	res_type == langbind::FilterBase::Attribute)
			{
				bool expectCloseTag = (res_type == langbind::FilterBase::OpenTag);
				std::string key = res_elem.tostring();
				if (!output->getNext( res_type, res_elem)
				||	res_type != langbind::FilterBase::Value)
				{
					throw std::runtime_error( "cannot interpret output of authorize function (key value pairs expected)");
				}
				attributes.push_back( Attribute( key, res_elem));
				if (expectCloseTag)
				{
					if (!output->getNext( res_type, res_elem)
					||	res_type != langbind::FilterBase::Value)
					{
						throw std::runtime_error( "cannot interpret output of authorize function (close expected)");
					}
				}
				if (!output->getNext( res_type, res_elem))
				{
					throw std::runtime_error( "cannot interpret output of authorize function (final close expected)");
				}
			}
			return true;
		}
		else
		{
			//...no result => authorization accepted
			return true;
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string("authorization failed: ") + e.what());
	}
}

bool AaMapProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (boost::algorithm::iequals( ext, ".aamap"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

static const utils::CharTable g_optab( "#=;)(,");

static char gotoNextToken( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	for (;;)
	{
		char ch = utils::gotoNextToken( si, se);
		if (ch == '#')
		{
			utils::parseLine( si, se);
			continue; //... comment
		}
		return ch;
	}
}

static char parseNextToken( std::string& tok, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	for (;;)
	{
		char ch = utils::parseNextToken( tok, si, se, g_optab);
		if (ch == '#')
		{
			utils::parseLine( si, se);
			continue; //... comment
		}
		return ch;
	}
}

static const char* g_keyword_ids[] = {"AUDIT","AUTHORIZE",0};
enum AaMapKeyword{ m_NONE, m_AUDIT, m_AUTHORIZE};
static const utils::IdentifierTable g_keyword_idtab( false, g_keyword_ids);

static void parseArrayAccess( char& delim, std::size_t& idx, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	delim = ' '; //... default delimiter
	for (++si; si != se && *si <= 32; ++si){}
	if (si == se) throw std::runtime_error( "unexpected end of authorization program");
	if (*si == ']' || *si == '[') throw std::runtime_error( "unexpected square bracket in array access");
	if (*si < '0' || *si > '9')
	{
		delim = *si;
		++si;
	}
	std::string indexstr;
	for (; si != se && *si >= '0' && *si < '9'; ++si)
	{
		indexstr.push_back(*si);
	}
	idx = boost::lexical_cast<std::size_t>( indexstr);
	if (idx == 0) throw std::runtime_error( "index in array expression must be >= 1");
	if (gotoNextToken( si, se) != ']') throw std::runtime_error( "expected square end bracket ']' (end of array access)");
	++si;
}

static langbind::ExecContextElement parseParameter( std::string::const_iterator& si, const std::string::const_iterator& se, bool knowsResource)
{
	std::string name;
	char ch = parseNextToken( name, si, se);
	if (ch == '"' || ch == '\'') throw std::runtime_error("identifier expected as parameter name instead of string");
	if (g_optab[ch]) throw std::runtime_error( std::string( "identifier expected as parameter name instead of '") + ch + "'");

	ch = gotoNextToken( si, se);
	if (ch == '=')
	{
		++si;
		std::string valuestr;
		ch = parseNextToken( valuestr, si, se);
		if (g_optab[ch]) throw std::runtime_error( std::string( "identifier or string expected as parameter value of '") + ch + "'");
		if (ch == '"' || ch == '\'')
		{
			return langbind::ExecContextElement( name, valuestr);
		}
		else
		{
			langbind::ExecContextElement::Value value = langbind::ExecContextElement::valueFromId( valuestr);
			if (!knowsResource && value == langbind::ExecContextElement::Resource)
			{
				throw std::runtime_error( std::string("unknown parameter '") + langbind::ExecContextElement::valueName( langbind::ExecContextElement::Resource) + "'");
			}
			if (gotoNextToken( si, se) == '[')
			{
				char delim;
				std::size_t idx;
				parseArrayAccess( delim, idx, si, se);
				return langbind::ExecContextElement( name, value, delim, idx);
			}
			else
			{
				return langbind::ExecContextElement( name, value);
			}
		}
	}
	else if (ch == ',' || ch == ')')
	{
		langbind::ExecContextElement::Value value = langbind::ExecContextElement::valueFromId( name);
		if (!knowsResource && value == langbind::ExecContextElement::Resource)
		{
			throw std::runtime_error( std::string("unknown parameter '") + langbind::ExecContextElement::valueName( langbind::ExecContextElement::Resource) + "'");
		}
		if (gotoNextToken( si, se) == '[')
		{
			char delim;
			std::size_t idx;
			parseArrayAccess( delim, idx, si, se);
			return langbind::ExecContextElement( name, value, delim, idx);
		}
		else
		{
			return langbind::ExecContextElement( name, value);
		}
	}
	else
	{
		throw std::runtime_error("unexpected token after parameter name");
	}
}

struct AaMapExpression
{
	enum FuncType
	{
		AuditFunc,
		AuthorizeFunc
	};

	AaMapExpression()
		:aafunctype(AuthorizeFunc),formfunc(0){}
	AaMapExpression( FuncType ft)
		:aafunctype(ft),formfunc(0){}
	AaMapExpression( const AaMapExpression& o)
		:aafunctype(o.aafunctype),aafunc(o.aafunc),formfunc(o.formfunc),params(o.params){}

	FuncType aafunctype;
	std::string aafunc;
	const langbind::FormFunction* formfunc;
	std::vector<langbind::ExecContextElement> params;
};

struct AaMapProgramDescription
{
	AaMapProgramDescription(){}
	AaMapProgramDescription( const AaMapProgramDescription& o)
		:database(o.database),expressions(o.expressions){}

	std::string database;
	std::vector<AaMapExpression> expressions;
};

static AaMapProgramDescription parseProgram( const ProgramLibrary& library, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	AaMapProgramDescription rt;
	char ch = gotoNextToken( si, se);
	for (; ch != 0; ch = gotoNextToken( si, se))
	{
		AaMapKeyword kw = (AaMapKeyword)utils::parseNextIdentifier( si, se, g_keyword_idtab);
		switch (kw)
		{
			case m_NONE:
				if (ch == ';')
				{
					throw std::runtime_error( "unexpected semicolon without statement");
				}
				else
				{
					throw std::runtime_error( std::string( "unexpected token in authorization program. keyword (") + g_keyword_idtab.tostring() + ") expected");
				}

			case m_AUDIT:
			case m_AUTHORIZE:
			{
				AaMapExpression expr( kw==m_AUDIT?AaMapExpression::AuditFunc:AaMapExpression::AuthorizeFunc);
				ch = parseNextToken( expr.aafunc, si, se);
				if (ch == 0 || g_optab[ch])
				{
					throw std::runtime_error( "string or identifier expected for authorization function name after AUTHORIZE");
				}
				if (library.getAuthorizationFunction( expr.aafunc))
				{
					throw std::runtime_error( std::string("duplicate definition of authorization function '") + expr.aafunc + "'");
				}
				std::string formfunc;
				ch = parseNextToken( formfunc, si, se);
				if (ch == 0 || g_optab[ch])
				{
					throw std::runtime_error( "string or identifier expected for form function name after AUTHORIZE and authorization function name");
				}
				expr.formfunc = library.getFormFunction( formfunc);
				if (!expr.formfunc)
				{
					throw std::runtime_error( "referenced form function assigned to authorization function is not defined");
				}
				ch = gotoNextToken( si, se);
				if (ch != '(')
				{
					throw std::runtime_error( "function parameter declaration open bracket '(' expeced after form function name assigned to authorization function");
				}
				++si;
				ch = gotoNextToken( si, se);
				for (; ch != ')' && ch != 0; gotoNextToken( si, se))
				{
					bool knowsResource = (expr.aafunctype == AaMapExpression::AuthorizeFunc);
					langbind::ExecContextElement param = parseParameter( si, se, knowsResource);
					ch = gotoNextToken( si, se);
					if (ch == ',')
					{
						++si;
						ch = gotoNextToken( si, se);
						if (ch == ')')
						{
							throw std::runtime_error( "unexpected close bracket ')' after argument list separator ','");
						}
					}
					expr.params.push_back( param);
				}
				if (ch != ')')
				{
					throw std::runtime_error( "function parameter declaration close bracket ')' expeced after parameter declaration list");
				}
				++si;
				ch = gotoNextToken( si, se);
				if (ch != ';')
				{
					throw std::runtime_error( "semicolon ';' expected after function declaration");
				}
				++si;
				rt.expressions.push_back( expr);
				break;
			}
		}
	}
	return rt;
}

void AaMapProgram::loadProgram( ProgramLibrary& library, db::Database*, const std::string& filename)
{
	std::string source;
	try
	{
		source = utils::readSourceFileContent( filename);
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "failed to load authorization program: ") + e.what());
	}
	std::string::const_iterator si = source.begin(), se = source.end();

	utils::FileLineInfo posinfo( utils::getFileStem( filename) + utils::getFileExtension( filename));
	std::string::const_iterator posinfo_si = si;

	try
	{
		AaMapProgramDescription prg = parseProgram( library, si, se);
		std::vector<AaMapExpression>::const_iterator xi = prg.expressions.begin(), xe = prg.expressions.end();
		for (; xi != xe; ++xi)
		{
			switch (xi->aafunctype)
			{
				case AaMapExpression::AuditFunc:
				{
					langbind::AuditFunctionR aaf( new AuditFunctionImpl( xi->formfunc, xi->params, prg.database));
					library.defineAuditFunction( xi->aafunc, aaf);
					break;
				}
				case AaMapExpression::AuthorizeFunc:
				{
					langbind::AuthorizationFunctionR aaf( new AuthorizationFunctionImpl( xi->formfunc, xi->params, prg.database));
					library.defineAuthorizationFunction( xi->aafunc, aaf);
					break;
				}
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		posinfo.update( posinfo_si, si);
		throw std::runtime_error( std::string( "error in authorization program at ") + posinfo.logtext() + ": " + e.what());
	}
}


