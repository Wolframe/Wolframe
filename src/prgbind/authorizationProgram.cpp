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
/// \file prgbind/authorizationProgram.cpp
/// \brief Implementation of authorization programs

#include "types/authorizationFunction.hpp"
#include "prgbind/authorizationProgram.hpp"
#include "processor/execContext.hpp"
#include "langbind/formFunction.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

/// \class AuthorizationFunctionImpl
/// \brief Description of a an authorization function
class AuthorizationFunctionImpl
	:public types::AuthorizationFunction
{
public:
	struct Parameter
	{
		std::string name;		///< name of the parameter
		enum Value {Const,UserName,SocketIdentifier,Authenticator};
		Value value;			///< value taken from execution context
		std::string const_value;	///< value in case of const

		Parameter()
			:value(Const){}
		Parameter( const Parameter& o)
			:name(o.name),value(o.value),const_value(o.const_value){}
		Parameter( const std::string& name_, Value value_, const std::string& const_value_)
			:name(name_),value(value_),const_value(const_value_){}
		Parameter( const std::string& name_, Value value_)
			:name(name_),value(value_){}
	};

public:
	/// \brief Constructor
	AuthorizationFunctionImpl( const langbind::FormFunction* function_, const std::vector<Parameter>& params_)
		:m_function(function_)
		,m_params(params_){}

	/// \brief Copy constructor
	AuthorizationFunctionImpl( const AuthorizationFunctionImpl& o)
		:m_function(o.m_function)
		,m_params(o.m_params){}

	/// \brief Default constructor
	AuthorizationFunctionImpl(){}

	/// \brief Destructor
	virtual ~AuthorizationFunctionImpl(){}

	const langbind::FormFunction* function() const	{return m_function;}
	const std::vector<Parameter>& params() const	{return m_params;}

	virtual bool call( proc::ExecContext* ctx, const std::string& resource) const;

private:
	const langbind::FormFunction* m_function;	///< form function to call
	std::vector<Parameter> m_params;		///< parameter description
};


class AuthorizationArg
	:public langbind::TypedInputFilter
{
public:
	explicit AuthorizationArg( const AuthorizationFunctionImpl& function_, const proc::ExecContext& ctx_, const std::string& authorizationResource_)
		:utils::TypeSignature("proc::AuthorizationArg", __LINE__)
		,langbind::TypedInputFilter("autharg")
		,m_state(0)
		,m_paramidx(0)
		,m_function(&function_)
		,m_ctx(&ctx_)
		,m_authorizationResource(authorizationResource_)
		{}

	AuthorizationArg( const AuthorizationArg& o)
		:utils::TypeSignature("proc::AuthorizationArg", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_state(o.m_state)
		,m_paramidx(o.m_paramidx)
		,m_function(o.m_function)
		,m_ctx(o.m_ctx)
		,m_authorizationResource(o.m_authorizationResource)
		{}

	virtual ~AuthorizationArg(){}

	virtual langbind::TypedInputFilter* copy() const
	{
		return new AuthorizationArg(*this);
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		for (;;)
		switch (m_state++)
		{
			case 0: type = langbind::FilterBase::Attribute; element = "resource"; return true;
			case 1: type = langbind::FilterBase::Value; element = m_authorizationResource; return true;
			case 2: if (m_paramidx == m_function->params().size())
				{
					m_state = 3/*4*/;
					continue;
				}
				else
				{
					type = langbind::FilterBase::Attribute;
					element = m_function->params().at( m_paramidx).name;
					return true;
				}
			case 3: switch (m_function->params().at( m_paramidx).value)
				{
					case AuthorizationFunctionImpl::Parameter::Const:
						element = m_function->params().at( m_paramidx).const_value;
						break;
					case AuthorizationFunctionImpl::Parameter::UserName:
						element = m_ctx->user()->uname().c_str();
						break;
					case AuthorizationFunctionImpl::Parameter::SocketIdentifier:
						element = m_ctx->socketIdentifier();
						break;
					case AuthorizationFunctionImpl::Parameter::Authenticator:
						element = m_ctx->user()->authenticator().c_str();
						break;
				}
				type = langbind::FilterBase::Value;
				++m_paramidx;
				m_state -= 2;
				return true;

			case 4: type = langbind::FilterBase::CloseTag; element.init(); return true;
			case 5: type = langbind::FilterBase::CloseTag; element.init(); return true;
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
	const AuthorizationFunctionImpl* m_function;
	const proc::ExecContext* m_ctx;
	std::string m_authorizationResource;
};


bool AuthorizationFunctionImpl::call( proc::ExecContext* ctx, const std::string& resource) const
{
	langbind::FormFunctionClosureR clos( m_function->createClosure());
	langbind::TypedInputFilterR input( new AuthorizationArg( *this, *ctx, resource));
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
			//...no result => authotization accepted
			return true;
		}
		if (res_type == langbind::FilterBase::Value)
		{
			//...boolean result is interpreted as result of authorization
			return res_elem.tobool();
		}
		return false;
	}
	else
	{
		//...no result => authotization accepted
		return true;
	}
}

bool AuthorizationProgram::is_mine( const std::string& ) const
{
	//... not implemented yet
	return false;
}

void AuthorizationProgram::loadProgram( ProgramLibrary&, db::Database*, const std::string&)
{
	throw std::runtime_error( "authorization program not implemented yet");
}


