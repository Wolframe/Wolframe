/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file modules/formfunction/template/formfunctionObjectBuilder.hpp
///\brief Interface template for object builder of form functions
#ifndef _Wolframe_MODULE_FORMFUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_FORMFUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "langbind/appFormFunction.hpp"
#include "moduleInterface.hpp"
#include "object.hpp"

namespace _Wolframe {
namespace module {

class FormFunctionObject :public Object, public langbind::FormFunction
{
public:
	FormFunctionObject( const char* name_, const langbind::FormFunction& func)
		:langbind::FormFunction(func)
		,m_name(name_){}

	virtual ~FormFunctionObject(){}

	virtual const char* objectName() const
	{
		return m_name.c_str();
	}
private:
	std::string m_name;
};

class FormFunctionObjectBuilder :public ObjectBuilder
{
public:
	FormFunctionObjectBuilder( const char* name_, langbind::FormFunction::Function f, const serialize::FiltermapDescriptionBase* p, const serialize::FiltermapDescriptionBase* r)
		:ObjectBuilder( name_)
		,m_func(name_,langbind::FormFunction( f,p,r)){}

	virtual ~FormFunctionObjectBuilder(){}

	virtual Object* object()
	{
		return &m_func;
	}
private:
	FormFunctionObject m_func;
};

}}//namespace

#define DECLARE_FUNCTION(NAME,CPPID,FUNC,PARAM,RESULT) \
namespace {\
struct CPPID\
{\
	static ObjectBuilder* constructor()\
	{\
		return new FormFunctionObjectBuilder(NAME,FUNC,PARAM,RESULT);\
	}\
};\
}//anonymous namespace
//end DECLARE_FUNCTION

#endif

