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
///\file comauto/function.hpp
///\brief Interface to execute form functions written in .NET
#ifndef _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "langbind/formFunction.hpp"
#include "processor/execContext.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>

struct ITypeInfo;

namespace _Wolframe {
namespace comauto {

///\brief Forward declarations
class TypeLib;
class CommonLanguageRuntime;
class DotnetFunction;

class DotnetFunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	DotnetFunctionClosure( const DotnetFunction* func_);
	virtual ~DotnetFunctionClosure();

	virtual bool call();

	virtual void init( const proc::ProcessorProviderInterface* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f=serialize::Context::None);

	virtual langbind::TypedInputFilterR result() const;

private:
	friend class DotnetFunction;
	class Impl;
	Impl* m_impl;
};


class DotnetFunction
	:public langbind::FormFunction
{
public:
	DotnetFunction( CommonLanguageRuntime* clr_, const TypeLib* typelib_, const ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx);
	virtual ~DotnetFunction();

	virtual void print( std::ostream& out) const;
	virtual langbind::FormFunctionClosure* createClosure() const;

	const std::string& assemblyname() const;
	const std::string& classname() const;
	const std::string& methodname() const;

private:
	friend class DotnetFunctionClosure;
	friend class DotnetFunctionClosure::Impl;
	class Impl;
	Impl* m_impl;
};

typedef boost::shared_ptr<DotnetFunction> DotnetFunctionR;

std::vector<comauto::DotnetFunctionR> loadFunctions( const TypeLib* typelib, CommonLanguageRuntime* clr, const std::string& assemblyname);


}} //namespace
#endif

