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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file execContext.cpp
/// \brief Implementation execution context
#include "processor/execContext.hpp"
#include "filter/typedfilter.hpp"
#include "langbind/formFunction.hpp"

using namespace _Wolframe;
using namespace _Wolframe::proc;

class AuthorizationArg
	:public langbind::TypedInputFilter
{
public:
	explicit AuthorizationArg( const std::string& authorizationResource_)
		:utils::TypeSignature("proc::AuthorizationArg", __LINE__)
		,langbind::TypedInputFilter("autharg")
		,m_iterator(0)
		,m_authorizationResource(authorizationResource_)
		{}

	AuthorizationArg( const AuthorizationArg& o)
		:utils::TypeSignature("proc::AuthorizationArg", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_iterator(0)
		,m_authorizationResource(o.m_authorizationResource)
		{}

	virtual ~AuthorizationArg(){}

	virtual langbind::TypedInputFilter* copy() const
	{
		return new AuthorizationArg(*this);
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		switch (m_iterator++)
		{
			case 0: type = langbind::FilterBase::Attribute; element = "resource"; return true;
			case 1: type = langbind::FilterBase::Value; element = m_authorizationResource; return true;
			case 2: type = langbind::FilterBase::CloseTag; element.init(); return true;
		}
		return false;
	}

	virtual void resetIterator()
	{
		m_iterator = 0;
	}

private:
	int m_iterator;
	std::string m_authorizationResource;
};

bool ExecContext::checkAuthorization( const std::string& authorizationFunction, const std::string& authorizationResource)
{
	if (authorizationFunction.empty()) return true;
	try
	{
		const langbind::FormFunction* func = m_provider->formFunction( authorizationFunction);
		if (func == 0)
		{
			return false;
		}
		langbind::FormFunctionClosureR clos( func->createClosure());
		langbind::TypedInputFilterR input( new AuthorizationArg( authorizationResource));
		clos->init( this, input);
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
	catch (std::runtime_error& e)
	{
		return false;
	}
}

