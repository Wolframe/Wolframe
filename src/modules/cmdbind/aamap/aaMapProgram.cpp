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
#include "processor/execContext.hpp"
#include "langbind/formFunction.hpp"
#include "utils/fileUtils.hpp"
#include "utils/fileLineInfo.hpp"
#include "utils/parseUtils.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

struct ExecContextParameter
{
	enum Value
	{
		Const,
		UserName,
		SocketId,
		RemoteHost,
		ConnectionType,
		Authenticator,
		Resource
	};
	enum {MaxValue=(int)Resource};

	static const char* valueName( Value v)
	{
		static const char* ar[] = {"Const","UserName","SocketId","RemoteHost","ConnectionType","Authenticator","Resource"};
		return ar[v];
	}
	static Value valueFromId( const std::string& v)
	{
		int rt = (int)UserName; //... do not accept reserved "Const"

		while (rt <= (int)MaxValue)
		{
			if (boost::algorithm::iequals( v, valueName((Value)rt)))
			{
				return (Value)rt;
			}
			++rt;
		}
		throw std::runtime_error( std::string("unknown identifier for value in expression '") + v + "'");
	}

	std::string name;		///< name of the parameter
	Value value;			///< value taken from execution context
	std::string const_value;	///< value in case of const
	char value_delim;		///< delimiter in case of array access (0 else)
	std::size_t value_idx;		///< index >= 1 in case of array access (0 else)

	ExecContextParameter()
		:value(Const),value_delim(0),value_idx(0){}
	ExecContextParameter( const ExecContextParameter& o)
		:name(o.name),value(o.value),const_value(o.const_value),value_delim(o.value_delim),value_idx(o.value_idx){}
	ExecContextParameter( const std::string& name_, const std::string& const_value_)
		:name(name_),value(Const),const_value(const_value_),value_delim(0),value_idx(0){}
	ExecContextParameter( const std::string& name_, Value value_)
		:name(name_),value(value_),value_delim(0),value_idx(0){}
	ExecContextParameter( const std::string& name_, Value value_, char value_delim_, std::size_t value_idx_)
		:name(name_),value(value_),value_delim(value_delim_),value_idx(value_idx_){}
};

class ExecContextArg
	:public langbind::TypedInputFilter
{
public:
	explicit ExecContextArg( const std::vector<ExecContextParameter>& params_, const proc::ExecContext& ctx_, const std::string& authorizationResource_)
		:utils::TypeSignature("prgbind::ExecContextArg", __LINE__)
		,langbind::TypedInputFilter("authzarg")
		,m_state(0)
		,m_paramidx(0)
		,m_params(&params_)
		,m_ctx(&ctx_)
		,m_authorizationResource(authorizationResource_)
		{}

	ExecContextArg( const ExecContextArg& o)
		:utils::TypeSignature("prgbind::ExecContextArg", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_state(o.m_state)
		,m_paramidx(o.m_paramidx)
		,m_params(o.m_params)
		,m_ctx(o.m_ctx)
		,m_authorizationResource(o.m_authorizationResource)
		,m_elembuf(o.m_elembuf)
		{}

	virtual ~ExecContextArg(){}

	virtual langbind::TypedInputFilter* copy() const
	{
		return new ExecContextArg(*this);
	}

	const std::string& getElement( const std::string& ar, char delim, std::size_t idx)
	{
		if (!delim) return ar;
		const char* cc = ar.c_str();
		const char* ee = std::strchr( cc, delim);
		for (; idx > 1 && ee != 0; --idx,cc=ee+1,ee=std::strchr(cc,delim)){}
		m_elembuf.clear();
		if (idx == 1)
		{
			if (ee == 0) ee = ar.c_str() + ar.size();
			m_elembuf.append( cc, ee-cc);
		}
		return m_elembuf;
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		for (;;)
		switch (m_state++)
		{
			case 0: if (m_paramidx == m_params->size())
				{
					m_state = 2/*3*/;
					continue;
				}
				else
				{
					type = langbind::FilterBase::OpenTag;
					element = m_params->at( m_paramidx).name;
					return true;
				}
			case 1:
			{
				const ExecContextParameter& pp = m_params->at( m_paramidx);
				switch (pp.value)
				{
					case ExecContextParameter::Const:
						element = getElement( pp.const_value, pp.value_delim, pp.value_idx);
						break;
					case ExecContextParameter::UserName:
						if (!m_ctx->user()) continue;
						element = getElement( m_ctx->user()->uname(), pp.value_delim, pp.value_idx);
						break;
					case ExecContextParameter::SocketId:
						if (!m_ctx->socketIdentifier()) continue;
						element = getElement( m_ctx->socketIdentifier(), pp.value_delim, pp.value_idx);
						break;
					case ExecContextParameter::RemoteHost:
						if (!m_ctx->remoteEndpoint()) continue;
						element = getElement( m_ctx->remoteEndpoint()->host(), pp.value_delim, pp.value_idx);
						break;
					case ExecContextParameter::ConnectionType:
						if (!m_ctx->remoteEndpoint()) continue;
						element = getElement( net::ConnectionEndpoint::connectionTypeName( m_ctx->remoteEndpoint()->type()), pp.value_delim, pp.value_idx);
						break;
					case ExecContextParameter::Authenticator:
						if (!m_ctx->user()) continue;
						element = getElement( m_ctx->user()->authenticator(), pp.value_delim, pp.value_idx);
						break;
					case ExecContextParameter::Resource:
						element = getElement( m_authorizationResource, pp.value_delim, pp.value_idx);
						break;
				}
				type = langbind::FilterBase::Value;
				return true;
			}
			case 2: type = langbind::FilterBase::CloseTag; element.init(); m_state -= 3; ++m_paramidx; return true;
			case 3: type = langbind::FilterBase::CloseTag; element.init(); return true;
			case 4: type = langbind::FilterBase::CloseTag; element.init(); return true;
			default: return false;
		}
	}

	virtual void resetIterator()
	{
		m_state = 0;
		m_paramidx = 0;
	}

private:
	int m_state;
	std::size_t m_paramidx;
	const std::vector<ExecContextParameter>* m_params;
	const proc::ExecContext* m_ctx;
	std::string m_authorizationResource;
	std::string m_elembuf;
};


/// \class AuthorizationFunctionImpl
/// \brief Description of a an authorization function
class AuthorizationFunctionImpl
	:public langbind::AuthorizationFunction
{
public:
	/// \brief Constructor
	AuthorizationFunctionImpl( const langbind::FormFunction* function_, const std::vector<ExecContextParameter>& params_, const std::string& database_)
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
	const std::vector<ExecContextParameter>& params() const
	{
		return m_params;
	}

	virtual bool call( proc::ExecContext* ctx, const std::string& resource, std::vector<Attribute>& attributes) const;

private:
	const langbind::FormFunction* m_function;	///< form function to call
	std::vector<ExecContextParameter> m_params;	///< parameter description
	std::string m_database;				///< database used for authorization functions
};




bool AuthorizationFunctionImpl::call( proc::ExecContext* ctx, const std::string& resource, std::vector<Attribute>& attributes) const
{
	try
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
		DatabaseScope databaseScope( &m_database, ctx);

		langbind::FormFunctionClosureR clos( m_function->createClosure());
		langbind::TypedInputFilterR input( new ExecContextArg( params(), *ctx, resource));
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

static const char* g_keyword_ids[] = {"AUDIT","AUTHORIZE","DATABASE",0};
enum AaMapKeyword{ m_NONE, m_AUDIT, m_AUTHORIZE, m_DATABASE};
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

static ExecContextParameter parseParameter( std::string::const_iterator& si, const std::string::const_iterator& se, bool knowsResource)
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
			return ExecContextParameter( name, valuestr);
		}
		else
		{
			ExecContextParameter::Value value = ExecContextParameter::valueFromId( valuestr);
			if (!knowsResource && value == ExecContextParameter::Resource)
			{
				throw std::runtime_error( std::string("unknown parameter '") + ExecContextParameter::valueName( ExecContextParameter::Resource) + "'");
			}
			if (gotoNextToken( si, se) == '[')
			{
				char delim;
				std::size_t idx;
				parseArrayAccess( delim, idx, si, se);
				return ExecContextParameter( name, value, delim, idx);
			}
			else
			{
				return ExecContextParameter( name, value);
			}
		}
	}
	else if (ch == ',' || ch == ')')
	{
		ExecContextParameter::Value value = ExecContextParameter::valueFromId( name);
		if (!knowsResource && value == ExecContextParameter::Resource)
		{
			throw std::runtime_error( std::string("unknown parameter '") + ExecContextParameter::valueName( ExecContextParameter::Resource) + "'");
		}
		if (gotoNextToken( si, se) == '[')
		{
			char delim;
			std::size_t idx;
			parseArrayAccess( delim, idx, si, se);
			return ExecContextParameter( name, value, delim, idx);
		}
		else
		{
			return ExecContextParameter( name, value);
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
	std::vector<ExecContextParameter> params;
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
	bool databaseDefined = false;
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

			case m_DATABASE:
				if (databaseDefined)
				{
					throw std::runtime_error( "duplicate definition of DATABASE");
				}
				databaseDefined = true;
				ch = parseNextToken( rt.database, si, se);
				if (ch == 0 || g_optab[ch])
				{
					throw std::runtime_error( "string or identifier expected after DATABASE");
				}
				break;

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
					ExecContextParameter param = parseParameter( si, se, knowsResource);
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
			langbind::AuthorizationFunctionR aaf( new AuthorizationFunctionImpl( xi->formfunc, xi->params, prg.database));
			library.defineAuthorizationFunction( xi->aafunc, aaf);
		}
	}
	catch (const std::runtime_error& e)
	{
		posinfo.update( posinfo_si, si);
		throw std::runtime_error( std::string( "error in authorization program at ") + posinfo.logtext() + ": " + e.what());
	}
}


